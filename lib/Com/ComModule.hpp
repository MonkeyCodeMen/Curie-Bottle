// ComModule.hpp
#pragma once
#include <Arduino.h>
#include "ComFrame.hpp"


class ComModule {
public:
    // no constructor, no copy constructor, no assignment operator
    ComModule(char id) : _com_module_id(id){}

    // default destructor
    virtual ~ComModule() = default;

    // virtual method to dispatch a frame to the module (interface)
    virtual bool dispatchFrame(ComFrame* pFrame) = 0;

    // virtual method to get the module ID (interface)
    const char getModuleId() const {return _com_module_id;}
private:
    char _com_module_id;

};
