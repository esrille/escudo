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

#ifndef ORG_W3C_DOM_BOOTSTRAP_TASK_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_TASK_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/function.hpp>

#include "Queue.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// cf. http://www.w3.org/TR/html5/webappapis.html#task-queue
class Task
{
    Object target;
    boost::function<void (void)> handler;
public:
    Task() {}
    Task(Object target, boost::function<void (void)> func) :
        target(target),
        handler(func)
    {
    }
    Task(const Task& other) :
        target(other.target),
        handler(other.handler)
    {
    }
    ~Task() {
        handler.clear();
    }
    void run() {
        if (handler) {
            handler();
            handler.clear();
        }
    }
    Task& operator=(const Task& other)
    {
        if (this != &other) {
            target = other.target;
            handler = other.handler;
        }
        return *this;
    }
};

typedef Queue<Task> TaskQueue;

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_TASK_H_INCLUDED
