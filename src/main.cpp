#include <chrono>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

const int tasksCount = 20;
const int threadsCount = 3;
const int taskDelaySeconds = 1;

void dobavitZadachi(std::queue<int>& ochered,
                    std::mutex& mtx,
                    std::condition_variable& cv,
                    bool& done) {
    for (int i = 1; i <= tasksCount; ++i) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            ochered.push(i);
        }

        cv.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }

    cv.notify_all();
}

void printresult(int id, int zadacha, std::mutex& coutmutex) {
    std::lock_guard<std::mutex> lock(coutmutex);

    std::cout << "Поток " << id << " завершил задачу "
              << zadacha << std::endl;
}

void rabotaPotoka(int id,
                  std::queue<int>& ochered,
                  std::mutex& mtx,
                  std::condition_variable& cv,
                  bool& done,
                  std::mutex& coutmutex) {
    while (true) {
        int zadacha = 0;

        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [&ochered, &done]() {
                return !ochered.empty() || done;
            });

            if (ochered.empty() && done) {
                break;
            }

            zadacha = ochered.front();
            ochered.pop();
        }

        std::this_thread::sleep_for(std::chrono::seconds(taskDelaySeconds));
        printresult(id, zadacha, coutmutex);
    }
}

void startworkers(std::vector<std::thread>& potoki,
                  std::queue<int>& ochered,
                  std::mutex& mtx,
                  std::condition_variable& cv,
                  bool& done,
                  std::mutex& coutmutex) {
    for (int i = 1; i <= threadsCount; ++i) {
        potoki.push_back(std::thread(rabotaPotoka, i,
                                     std::ref(ochered),
                                     std::ref(mtx),
                                     std::ref(cv),
                                     std::ref(done),
                                     std::ref(coutmutex)));
    }
}

void joinworkers(std::vector<std::thread>& potoki) {
    for (std::size_t i = 0; i < potoki.size(); ++i) {
        potoki[i].join();
    }
}

int main() {
    std::queue<int> ochered;
    std::mutex mtx;
    std::mutex coutmutex;
    std::condition_variable cv;
    std::vector<std::thread> potoki;
    bool done = false;

    startworkers(potoki, ochered, mtx, cv, done, coutmutex);
    dobavitZadachi(ochered, mtx, cv, done);
    joinworkers(potoki);

    std::cout << "Все потоки закончили работу" << std::endl;

    return 0;
}