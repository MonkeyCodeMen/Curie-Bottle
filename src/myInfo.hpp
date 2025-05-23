#pragma once
#include <Arduino.h>
//#include <MainConfig.h>
#include <Debug.hpp>

class MyInfo : public Dump
{
    public:
        MyInfo(const String& name="AppInfo"); 
        
        String dump(uint32_t now_ms, uint32_t userID) const override;
};

