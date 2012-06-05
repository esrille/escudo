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

#ifndef ES_PROFILE_H
#define ES_PROFILE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

class Profile
{
    std::string profile;
    int error;
    int lock;
public:
    Profile(const std::string& path);
    ~Profile();

    // RETURN VALUES
    //   ENOENT or ENOTDIR: there is no profile directory.
    //   EAGAIN: the lock file has been locked.
    int hasError() const {
        return error;
    }
    const std::string& getProfilePath() const {
        return profile;
    }

    std::string createPath(const std::string& name) const;
    int createDirectory(const std::string& name);

    bool hasFile(const std::string& path, int mode = 0600);
    bool isDirectory(const std::string& path);
    int createDirectories(const std::string& path);
};

#endif  // ES_PROFILE_H
