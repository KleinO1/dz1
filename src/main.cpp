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

void rabotaPotoka(int id, std::queue<int>& ochered, std::mutex& mtx) {
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

        std::cout << "Поток " << id << " завершил задачу "
                  << zadacha << std::endl;
    }
}

int main() {
    std::queue<int> ochered;
    std::mutex mtx;
    std::vector<std::thread> potoki;

    dobavitZadachi(ochered);

    for (int i = 1; i <= threadsCount; ++i) {
        potoki.push_back(std::thread(rabotaPotoka, i,
                                     std::ref(ochered),
                                     std::ref(mtx)));
    }

    for (std::size_t i = 0; i < potoki.size(); ++i) {
        potoki[i].join();
    }

    std::cout << "Все потоки закончили работу" << std::endl;

    return 0;
}