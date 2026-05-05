#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

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

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Поток " << id << " завершил задачу "
                  << zadacha << std::endl;
    }
}

int main() {
    std::queue<int> ochered;
    std::mutex mtx;
    std::vector<std::thread> potoki;

    for (int i = 1; i <= 20; ++i) {
        ochered.push(i);
    }

    for (int i = 1; i <= 3; ++i) {
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