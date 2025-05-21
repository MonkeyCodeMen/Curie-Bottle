/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "ComDispatch.hpp"
#include <Debug.hpp>
#include <helper.h>

ComDispatch::ComDispatch() : _moduleCount(0) {}

void ComDispatch::registerModule(ComModule* module) {
    if (_moduleCount < COM_MAX_MODULES) {
        _modules[_moduleCount].moduleId = module->getModuleId();
        _modules[_moduleCount].module = module;
        _moduleCount++;
    }
}

bool ComDispatch::dispatchFrame(ComFrame *pFrame) {
    char moduleId = pFrame->module;

    // Suche nach dem Modul mit der passenden Kennung
    for (uint8_t i = 0; i < _moduleCount; i++) {
        if (_modules[i].moduleId == moduleId) {
            return _modules[i].module->dispatchFrame(pFrame);
        }
    }

    // Kein passendes Modul gefunden
    pFrame->res = "Error: Unknown module ID.";
    return false;
}