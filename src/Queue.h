/*
 * Copyright 2013 Esrille Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ES_QUEUE_H_INCLUDED
#define ES_QUEUE_H_INCLUDED

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

// A thread-safe queue template
template<typename T>
class Queue
{
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cond;

public:
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    void push(T const& value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        cond.notify_one();
    }

    bool tryPop(T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        if(queue.empty())
            return false;
        value = queue.front();
        queue.pop();
        return true;
    }

    void pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty())
            cond.wait(lock);
        value = queue.front();
        queue.pop();
    }
};

#endif  // ES_QUEUE_H_INCLUDED
