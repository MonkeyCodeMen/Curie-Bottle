
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


#include "Debug.hpp"
#include "helper.h"
#include "LittleFS.h"

#if defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_ARCH_MBED_RP2040)|| defined(ARDUINO_ARCH_RP2040)
  #include "malloc.h"
#endif


volatile bool   Debug::_initDone = false;
HardwareSerial *Debug::_pOut = NULL;
Mutex           Debug::_mutex;
String Debug::_outString = "";

Debug debug;
Dumper& dumper = Dumper::getInstance();

SystemInfo systemInfo;


Debug::Debug()
{
    
}

void Debug::begin(HardwareSerial * pOut,uint32_t baud){
  if (pOut == NULL){
    _initDone = false;
  }else{
    _pOut=pOut;
    _pOut->begin(baud);
    _mutex.free();
    dumper.registerDumpFunction("DebugLog", [this](uint32_t now_msec, uint32_t userID) { return this->dumpLogBuffer(now_msec, userID); });
    _initDone = true;
  }
}

String Debug::dumpLogBuffer(uint32_t now_msec, uint32_t userID) const {
    String out = "Debug Log Buffer Dump at " + String(now_msec) + " ms:\n";
    uint16_t idx = (_logBufferHead + DEBUG_LOG_BUFFER_SIZE - _logBufferCount) % DEBUG_LOG_BUFFER_SIZE;
    for (uint16_t i = 0; i < _logBufferCount; ++i) {
        out += _logBuffer[idx] + "\n";
        idx = (idx + 1) % DEBUG_LOG_BUFFER_SIZE;
    }
    return out;
}

bool Debug::_check(){
    if (_initDone == false){ return false;    }
    return true;
}

void Debug::logToBuffer(const String& msg) {
    _logBuffer[_logBufferHead] = msg;
    _logBufferHead = (_logBufferHead + 1) % DEBUG_LOG_BUFFER_SIZE;
    if (_logBufferCount < DEBUG_LOG_BUFFER_SIZE) {
        _logBufferCount++;
    }
}

void Debug::log(const char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out(F("LOG("));
    _out(time.c_str());
    _out(F(")::"));
    _out(text);
    _outEnd();
    _mutex.free();
}


void Debug::log(const char * file,int line,const char * text,int value){
    String str;
    str = text;
    str += String(value);
    Debug::log(file,line,str.c_str());
}


void Debug::log(const char * file,int line,const char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out(F("LOG("));
    _out((char *)time.c_str());
    _out(F("):"));
    _out(file);
    _out(F(":"));
    String lineStr(line);
    _out((char *)lineStr.c_str());
    _out(F("::"));
    _out(text);
    _outEnd();
    _mutex.free();
}



void Debug::assertTrue(bool cond ,const char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());

        _mutex.lock();
        _out(F("ASSERT failed("));
        _out(time);
        _out(F(")::"));
        _out(text);
        _outEnd();
        _mutex.free();
    }
}

void Debug::assertTrue(bool cond ,const char * file,int line,const char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());
        
        _mutex.lock();
        _out(F("ASSERT failed("));
        _out(time);
        _out(F("):"));
        _out(file);
        _out(F(":"));
        String lineStr(line);
        _out(lineStr);
        _out(F("::"));
        _out(text);
        _outEnd();
        _mutex.free();
    }
}

void Debug::_outEnd(){
    // at the moment only simply dump to terminal
    debug.logToBuffer(_outString);
    _outString = "";
    _pOut->println();
    _pOut->flush();
}

void Debug::logMem(const char * file,int line,const char * text){
    if (_check() == false) return;
    int usedHeap = 0; 
    int freeHeap = 0; 
     #if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP8266)

    #elif defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_ARCH_MBED_RP2040)|| defined(ARDUINO_ARCH_RP2040)
      usedHeap = rp2040.getUsedHeap();
      freeHeap = rp2040.getFreeHeap();
    #else
    
    #endif
    String temp(millis());

    _mutex.lock();
    
    _out(F("LOG memory("));
    _out(temp);
    _out(F("):"));
    _out(file);
    _out(F(":"));
    temp = String(line);
    //temp = line;
    _out(temp);
    _out(F("::"));
    _out(text);
    temp="-memory status:\n\tused heap: "+String(usedHeap)+"\n\tfree heap: "+String(freeHeap);
    _out(temp);
    
    _outEnd();
    _mutex.free();
}




void Debug::dump(const char * pName,void *pIn, uint8_t length){
    _mutex.lock();        
    String out = debug.hexDump((uint8_t*)pIn,length);
    _out(pName);
    _out(F(" : "));
    _out(out);
    _outEnd();
    _mutex.free();
}

void Debug::dump(const char * pName,uint32_t value){
    _mutex.lock();
    _out(pName);
    _out(F(" : "));
    _out(String(value));  
    _outEnd();
    _mutex.free();
}


void Debug::dump(const char * pName,uint32_t value, int base){
    _mutex.lock();
    _out(pName);
    _out(F(" : "));
    _out(String(value,base));  
    _outEnd();
    _mutex.free();
}

void Debug::dump(const char * pName,const char * value){
    _mutex.lock();
    _out(pName);
    _out(F(" : "));
    _out(String(value));  
    _outEnd();
    _mutex.free();
}


String Debug::hexDump(uint8_t  * p,uint8_t length,const char * sep,const char * prefix){
    const char * transTable = "0123456789ABCDEF";
    bool first = true;
    uint8_t value,index;

    String out = "";
    
    for(uint8_t i=0;i < length;i++){
        // interspace (not at first byte)
        if (first == true){
        first = false;
        } else {
        out+=sep;
        }

        // get byte and add prefix
        value = *p;
        out+=prefix;
        
        // bits 7..4
        index = (value >> 4) & 0x0F;
        out+=transTable[index];

        // bits 3..0
        index = value & 0x0F;
        out+=transTable[index];
        p++;
    }
    return out;
}

void Debug::stop(const char * file,int line,const char * message){
    // buffer makes no sense here (endless loop)
    _pOut->print(F("### critical error - system stop ### file: <"));
    _pOut->print(file);
    _pOut->print(F("> in line :"));
    _pOut->print(line);
    _pOut->print(F(" :: "));
    _pOut->print(message);
    while (1){ };
}



SystemInfo::SystemInfo(const String& name)
    : Dump(name) {}

String SystemInfo::dump(uint32_t now_ms, uint32_t userID) const {
    // Get system time
    uint32_t systemTime = now_ms;

    // Get heap information
    uint32_t freeHeap = rp2040.getFreeHeap(); // Replace with the correct function for your platform
    uint32_t usedHeap = rp2040.getTotalHeap() - freeHeap;
    uint32_t totalHeap = rp2040.getTotalHeap();
    
    // Calculate the RAM usage percentage
    float ramUsagePercentage = (float)usedHeap / totalHeap * 100;

    // Get CPU frequency
    uint32_t cpuFrequency = rp2040.f_cpu(); // Replace with the correct function for your platform      
    
    // Get CPU cycle count
    uint32_t cpuCycleCount = rp2040.getCycleCount(); // Replace with the correct function for your platform

    // Format the output
    String output = "System Info Dump:\n";
    output += "  System Time: " + String(systemTime) + " ms\n";
    output += "  Free Heap: " + String(freeHeap) + " bytes\n";
    output += "  Used Heap: " + String(usedHeap) + " bytes\n";
    output += "  Total Heap: " + String(totalHeap) + " bytes\n";
    output += "  CPU Frequency: " + String(cpuFrequency) + " Hz\n";
    output += "  CPU ID: " + String(rp2040.getChipID()) + "\n";
    output += "  CPU Cycle Count: " + String(cpuCycleCount) + "\n";
    
    // Add RAM usage as a percentage with a bar
    int barLength = 20;  // Length of the bar (20 characters wide)
    int filledLength = (int)(ramUsagePercentage / 100.0 * barLength);
    String ramBar = "";
    for (int i = 0; i < filledLength; i++) {
        ramBar += "=";
    }
    for (int i = filledLength; i < barLength; i++) {
        ramBar += " ";
    }

    output += "RAM:   [" + ramBar + "]   " + String(ramUsagePercentage, 1) + "% (used " + String(usedHeap) + " bytes from " + String(totalHeap) + " bytes)\n";

    // Get the file system information
    LittleFS.begin();

    // Get the file system information
    FSInfo info;
    if (LittleFS.info(info)){
        uint32_t fileSystemSize = info.totalBytes;
        uint32_t fileSystemUsed = info.usedBytes;
        uint32_t fileSystemFreeSpace = fileSystemSize - fileSystemUsed;

        // Calculate the file system usage percentage
        float fsUsagePercentage = (float)fileSystemUsed / fileSystemSize * 100;

        // Format the file system usage as a progress bar
        int fsBarLength = 20;  // Length of the bar (20 characters wide)
        int fsFilledLength = (int)(fsUsagePercentage / 100.0 * fsBarLength);
        String fsBar = "";
        for (int i = 0; i < fsFilledLength; i++) {
            fsBar += "=";
        }
        for (int i = fsFilledLength; i < fsBarLength; i++) {
            fsBar += " ";
        }

        output += "\nFile System Info:\n";
        output += "  File System Size: " + String(fileSystemSize) + " bytes\n";
        output += "  File System Used: " + String(fileSystemUsed) + " bytes\n";
        output += "  File System Free Space: " + String(fileSystemFreeSpace) + " bytes\n";
        output += "File System: [" + fsBar + "] " + String(fsUsagePercentage, 1) + "% (used " + String(fileSystemUsed) + " bytes from " + String(fileSystemSize) + " bytes)\n";

        // Get the file list from the root directory
        String fileList = _getFileList("/","    ");

        output += "  File List:\n" + fileList;
    }

    return output;
}

String SystemInfo::_getFileList(const String& path, const String & ident) const{
    String result = "";
    Dir dir = LittleFS.openDir(path);

    while (dir.next()) {
        if (dir.isDirectory()) {
            result += ident + path + dir.fileName() + "/\n";
            result += _getFileList(path + dir.fileName() + "/", ident + String("  ")); // Recursive call for subdirectories
        } else {
            result += ident + path + dir.fileName() + " : " + String(dir.fileSize()) + "bytes  \n";
        }
    }
    return result;
}


