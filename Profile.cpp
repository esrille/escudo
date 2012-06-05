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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>

Profile::Profile(const std::string& path) :
    profile(path),
    error(0)
{
    while (0 < profile.length() && profile[profile.length() - 1] == '/')
        profile.erase(profile.length() - 1);
    if (createDirectories(profile) == -1)
        return;

    std::string lockPath(createPath("lock"));
    int lock = open(lockPath.c_str(), O_CREAT | O_RDWR, 0600);
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
    if (!error) {
        std::string lockPath(createPath("lock"));
        remove(lockPath.c_str());
    }
    close(lock);
}

int Profile::createDirectories(const std::string& path)
{
    if (path.empty()) {
        error = EACCES;
        return -1;
    }

    std::string p;
    if (path[0] == '/')
        p = "/";
    struct stat buf;
    for (;;) {
        size_t pos = path.find('/', p.length());
        if (pos != std::string::npos) {
            p = path.substr(0, pos);
            if (stat(p.c_str(), &buf) == -1) {
                if (mkdir(p.c_str(), 0700) == -1)
                    return -1;
                stat(p.c_str(), &buf);
            }
            if (!S_ISDIR(buf.st_mode)) {
                error = ENOTDIR;
                return -1;
            }
            p += '/';
        } else {
            if (stat(path.c_str(), &buf) == -1) {
                if (mkdir(path.c_str(), 0700) == -1)
                    return -1;
                stat(path.c_str(), &buf);
            }
            if (!S_ISDIR(buf.st_mode)) {
                error = ENOTDIR;
                return -1;
            }
            if ((buf.st_mode & 0700) != 0700) {
                error = EACCES;
                return -1;
            }
            break;
        }
    }
    error = 0;
    return 0;
}

std::string Profile::createPath(const std::string& name) const
{
    std::string path(name);
    while (0 < path.length() && path[0] == '/')
        path.erase(0, 1);
    return profile + '/' + path;
}

int Profile::createDirectory(const std::string& name)
{
    std::string path(createPath(name));
    if (mkdir(path.c_str(), 0700) == 0) {
        error = 0;
        return 0;
    }
    if (errno == EEXIST) {
        if (isDirectory(path))
            return 0;
    }
    error = errno;
    return -1;
}

bool Profile::hasFile(const std::string& path, int mode)
{
    struct stat buf;

    if (stat(path.c_str(), &buf) == -1)
        return false;
    if (!S_ISREG(buf.st_mode)) {
        error = ENOENT;
        return false;
    }
    if ((buf.st_mode & mode) != mode) {
        error = EACCES;
        return false;
    }
    error = 0;
    return true;
}

bool Profile::isDirectory(const std::string& path)
{
    struct stat buf;

    if (stat(path.c_str(), &buf) == -1)
        return false;
    if (!S_ISDIR(buf.st_mode)) {
        error = ENOTDIR;
        return false;
    }
    if ((buf.st_mode & 0700) != 0700) {
        error = EACCES;
        return false;
    }
    error = 0;
    return true;
}
