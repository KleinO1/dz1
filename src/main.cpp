#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

const int tasksCount = 20;
const int threadsCount = 3;
const int taskDelaySeconds = 1;

void dobavitZadachi(std::queue<int>& ochered) {
    for (int i = 1; i <= tasksCount; ++i) {
        ochered.push(i);
    }
}

void printresult(int id, int zadacha, std::mutex& coutmutex) {
    std::lock_guard<std::mutex> lock(coutmutex);

    std::cout << "Поток " << id << " завершил задачу "
              << zadacha << std::endl;
}

void rabotaPotoka(int id,
                  std::queue<int>& ochered,
                  std::mutex& mtx,
                  std::mutex& coutmutex) {
    while (true) {
        int zadacha = 0;

        {
            std::lock_guard<std::mutex> lock(mtx);

            if (ochered.empty()) {
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
                  std::mutex& coutmutex) {
    for (int i = 1; i <= threadsCount; ++i) {
        potoki.push_back(std::thread(rabotaPotoka, i,
                                     std::ref(ochered),
                                     std::ref(mtx),
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
    std::vector<std::thread> potoki;

    dobavitZadachi(ochered);
    startworkers(potoki, ochered, mtx, coutmutex);
    joinworkers(potoki);

    std::cout << "Все потоки закончили работу" << std::endl;

    return 0;
}