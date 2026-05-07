#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#include "TaskQueue.h"

const int tasksCount = 20;
const int threadsCount = 3;
const int taskDelaySeconds = 1;

void dobavitZadachi(TaskQueue& queue) {
    for (int i = 1; i <= tasksCount; ++i) {
        queue.push(i);
    }

    queue.stop();
}

void printresult(int id, int zadacha, std::mutex& coutmutex) {
    std::lock_guard<std::mutex> lock(coutmutex);

    std::cout << "Поток " << id << " завершил задачу "
              << zadacha << std::endl;
}

void rabotaPotoka(int id,
                  TaskQueue& queue,
                  std::mutex& coutmutex) {
    int zadacha = 0;

    while (queue.pop(zadacha)) {
        std::this_thread::sleep_for(std::chrono::seconds(taskDelaySeconds));
        printresult(id, zadacha, coutmutex);
    }
}

void startworkers(std::vector<std::thread>& potoki,
                  TaskQueue& queue,
                  std::mutex& coutmutex) {
    for (int i = 1; i <= threadsCount; ++i) {
        potoki.push_back(std::thread(rabotaPotoka, i,
                                     std::ref(queue),
                                     std::ref(coutmutex)));
    }
}

void joinworkers(std::vector<std::thread>& potoki) {
    for (std::size_t i = 0; i < potoki.size(); ++i) {
        potoki[i].join();
    }
}

int main() {
    TaskQueue queue;
    std::mutex coutmutex;
    std::vector<std::thread> potoki;

    startworkers(potoki, queue, coutmutex);
    dobavitZadachi(queue);
    joinworkers(potoki);

    std::cout << "Все потоки закончили работу" << std::endl;

    return 0;
}