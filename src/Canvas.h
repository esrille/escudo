/*
 * Copyright 2012, 2013 Esrille Inc.
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

#ifndef ES_CANVAS_H
#define ES_CANVAS_H

#include <memory>

class Canvas
{
    class Impl;
    std::unique_ptr<Impl> pimpl;

public:
    Canvas();
    ~Canvas();

    void setup(int width, int height);
    void shutdown();
    void beginRender(unsigned backgroundColor);
    void endRender();
    int getWidth() const;
    int getHeight() const;
    void render(int width, int height);
    void alphaBlend(int width, int height, float alpha);

    Impl* getPimple() const {
        return pimpl.get();
    }
};

#endif  // ES_CANVAS_H
