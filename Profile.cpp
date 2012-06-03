/*
 * Copyright 2012 Esrille Inc.
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

#include "Profile.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>

Profile::Profile(const char* path) :
    error(0)
{
    if (!path) {
        error = ENOENT;
        return;
    }

    profile = path;
    while (0 < profile.length() && profile[profile.length() - 1] == '/')
        profile.erase(profile.length() - 1);

    int lock = open((profile + "/lock").c_str(), O_CREAT | O_RDWR, 0600);
    if (lock == -1) {
        error = errno;
        return;
    }
    if (lockf(lock, F_TLOCK, 0) == -1) {
        error = errno;
        return;
    }
}

Profile::~Profile()
{
    // Note we don't have to remove the lock file, but it seems to be a good convention.
    if (!error)
        remove((this->profile + "/lock").c_str());
    close(lock);
}
