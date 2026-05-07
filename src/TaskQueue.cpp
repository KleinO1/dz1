#include "TaskQueue.h"

TaskQueue::TaskQueue() : closed(false) {
}

void TaskQueue::push(int zadacha) {
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (closed) {
            return;
        }

        ochered.push(zadacha);
    }

    cv.notify_one();
}

bool TaskQueue::pop(int& zadacha) {
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [this]() {
        return !ochered.empty() || closed;
    });

    if (ochered.empty() && closed) {
        return false;
    }

    zadacha = ochered.front();
    ochered.pop();

    return true;
}

void TaskQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (closed) {
            return;
        }

        closed = true;
    }

    cv.notify_all();
}