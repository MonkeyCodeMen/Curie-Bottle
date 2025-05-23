
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


#pragma once

#include <Arduino.h>
#include <Mutex.hpp>

#ifndef DEBUG_LOG_BUFFER_SIZE
#define DEBUG_LOG_BUFFER_SIZE 64
#endif


class Debug
{
public:
	Debug();
	~Debug() = default;

    void begin(HardwareSerial * pOut, uint32_t baud=115200);

    static void log(const char * text);
    static void log(const __FlashStringHelper * text)                                   { log(String(text));                                }
    static void log(char * text)                                                        { log((const char *)text);                          }
    static void log(String text)                                                        { log(text.c_str());                                }

    static void log(const char * file,int line,const char * text);
    static void log(const char * file,int line,const __FlashStringHelper * text)        { log(file,line,String(text));                      }
    static void log(const char * file,int line,char * text)                             { log(file,line,(const char *)text);                }
    static void log(const char * file,int line,String text)                             { log(file,line,text.c_str());                      }
    
    static void log(const char * file,int line,const char * text,int value);
    static void log(const char * file,int line,const __FlashStringHelper * text,int value){ log(file,line,String(text),value);              }
    static void log(const char * file,int line,char * text,int value)                   { log(file,line,(const char *)text,value);          }
    static void log(const char * file,int line,String text,int value)                   { log(file,line,text.c_str(),value);                }

    static void logMem(const char * file,int line,const char * text);
    static void logMem(const char * file,int line,const __FlashStringHelper * text)     { logMem(file,line,String(text));                   }
    static void logMem(const char * file,int line,char * text)                          { logMem(file,line,(const char*)text);              }
    static void logMem(const char * file,int line,String text)                          { logMem(file,line,text.c_str());                   }
    
    static void assertTrue(bool cond ,const char * text);
    static void assertTrue(bool cond ,const __FlashStringHelper * text)                 { assertTrue(cond,String(text));                    }
    static void assertTrue(bool cond ,char * text)                                      { assertTrue(cond,(const char*)text);               }
    static void assertTrue(bool cond ,String text)                                      { assertTrue(cond,text.c_str());                    }
    
    static void assertTrue(bool cond ,const char * file,int line,const char * text);
    static void assertTrue(bool cond ,const char * file,int line,const __FlashStringHelper * text){ assertTrue(cond,file,line,String(text));}
    static void assertTrue(bool cond ,const char * file,int line,char * text)           { assertTrue(cond,file,line,(const char *)text);    }
    static void assertTrue(bool cond ,const char * file,int line,String text)           { assertTrue(cond,file,line,text.c_str());          }

    static void stop(const char * file,int line,const char * message);
    static void stop(const char * file,int line,const __FlashStringHelper * message)    { stop(file,line,String(message));                  }
    static void stop(const char * file,int line,char * message)                         { stop(file,line,(const char*) message);            }
    static void stop(const char * file,int line,String message)                         { stop(file,line,message.c_str());                  }

    static void dump(const char * pName,void *p, uint8_t length);
    static void dump(char * pName,void *p,uint8_t length)                               { dump((const char *)pName,p,length);               }
    static void dump(const __FlashStringHelper * pName,void *p,uint8_t length)          { dump(String(pName),p,length);                     }
    static void dump(String name,void *p,uint8_t length)                                { dump(name.c_str(),p,length);                      }
    
    static void dump(const char * pName,uint32_t value);
    static void dump(char * pName,uint32_t value)                                       { dump((const char *)pName,value);                  }
    static void dump(const __FlashStringHelper * pName,uint32_t value)                  { dump(String(pName),value);                        }
    static void dump(String name,uint32_t value)                                        { dump(name.c_str(),value);                         }

    static void dump(const char * pName,uint32_t value,int base);
    static void dump(char * pName,uint32_t value,int base)                              { dump((const char *)pName,value,base);             }
    static void dump(const __FlashStringHelper * pName,uint32_t value,int base)         { dump(String(pName),value,base);                   }
    static void dump(String name,uint32_t value,int base)                               { dump(name.c_str(),value,base);                    }


    static void dump(const char * pName,const char * value);
    static void dump(char * pName,const char * value)                                   { dump((const char *)pName,value);                  }
    static void dump(const __FlashStringHelper * pName,const char * value)              { dump(String(pName),value);                        }
    static void dump(String name,const char * value)                                    { dump(name.c_str(),value);                         }
    static void dump(char * pName,char * value)                                         { dump((const char *)pName,(const char *)value);    }
    static void dump(const __FlashStringHelper * pName,char * value)                    { dump(String(pName),(const char *)value);          }
    static void dump(String name,char * value)                                          { dump(name.c_str(),(const char *)value);           }
    static void dump(char * pName,String value)                                         { dump((const char *)pName,value.c_str());          }
    static void dump(const __FlashStringHelper * pName,String value)                    { dump(String(pName),value.c_str());                }
    static void dump(String name,String value)                                          { dump(name.c_str(),value.c_str());                 }


    String hexDump(uint8_t * p,uint8_t length,const char * sep=" ", const char * prefix="");

    // Add log to buffer
    void logToBuffer(const String& msg);

    // Dump buffer content
    String dumpLogBuffer(uint32_t now_msec, uint32_t userID) const;


private:
    static void _out(char * text)                                                       {_pOut->print(text);    _outString += String(text);   }
    static void _out(const char * text)                                                 {_pOut->print(text);    _outString += String(text);   }
    static void _out(String  text)                                                      {_pOut->print(text);    _outString += text;           }
    static void _out(const __FlashStringHelper * text)                                  {_pOut->print(text);    _outString += String(text);   }
    static void _outEnd();
    static bool _check();

    static String _outString;

    volatile static bool    _initDone;
    static HardwareSerial * _pOut;
    static Mutex            _mutex;

    String _logBuffer[DEBUG_LOG_BUFFER_SIZE];
    uint16_t _logBufferHead = 0;
    uint16_t _logBufferCount = 0;

};


extern Debug debug;

#define WITH_DEBUG 1
#ifdef WITH_DEBUG
    #define LOG(text)               Debug::log((const char*)__FILE__,__LINE__,text)
    #define LOG_INT(text,value)     Debug::log((const char*)__FILE__,__LINE__,text,(int)value)
    #define LOG_MEM(text)           Debug::logMem((const char*)__FILE__,__LINE__,text)
    #define ASSERT(cond,text)       Debug::assertTrue(cond,(const char*)__FILE__,__LINE__,text)
    #define STOP(text)              Debug::stop((const char*)__FILE__,__LINE__,text)
    #define DUMP(...)               Debug::dump( __VA_ARGS__)
#else
    #define LOG(text)
    #define LOG_INT(text,value)
    #define LOG_MEM(text)
    #define ASSERT(cond,text)   
    #define STOP()                  Debug::stop((const char*)__FILE__,__LINE__,text)
    #define DUMP(...)
#endif


#include <map>
#include <functional> // Für std::function


/**
 * @typedef DumpFunctionPointer_t
 * @brief A function pointer type for dump functions.
 * 
 * A dump function takes a `uint32_t` timestamp (in milliseconds) as input
 * and returns a `String` containing the dump information.
 */
typedef std::function<String(uint32_t,uint32_t)> DumpFunctionPointer_t;

struct dumpEntry_struct{
    DumpFunctionPointer_t   p;
    uint32_t                userID;
};


/**
 * @class Dumper
 * @brief A centralized class for managing and invoking dump functions.
 * 
 * The `Dumper` class allows objects to register their dump functions with a unique name.
 * These dump functions can then be invoked from anywhere in the program using the name.
 * 
 * Example usage:
 * @code
 * // Define a class with a dump function
 * class MyObject {
 * public:
 *     MyObject(const String& name) : _name(name) {
 *         Dumper::getInstance().registerDumpFunction(_name, [this](uint32_t now_ms) {
 *             return this->dump(now_ms);
 *         });
 *     }
 * 
 *     ~MyObject() {
 *         Dumper::getInstance().unregisterDumpFunction(_name);
 *     }
 * 
 *     String dump(uint32_t now_ms) const {
 *         return "Dumping object '" + _name + "' at time " + String(now_ms) + " ms.";
 *     }
 * 
 * private:
 *     String _name;
 * };
 * 
 * // Use the Dumper
 * MyObject obj1("Object1");
 * MyObject obj2("Object2");
 * 
 * uint32_t now = millis();
 * String dump1 = Dumper::getInstance().callDumpFunction("Object1", now);
 * String dump2 = Dumper::getInstance().callDumpFunction("Object2", now);
 * @endcode
 */
class Dumper {
    public:
        /**
         * @brief Get the singleton instance of the Dumper class.
         * @return A reference to the Dumper instance.
         */
        static Dumper& getInstance() {
            static Dumper instance;
            return instance;
        }

        /**
         * @brief Register a dump function with a unique name.
         * @param name The unique name for the dump function.
         * @param func The dump function to register.
         * @param userID an optional user ID that can be used identify source of call if one functionm is registered more than one time
         * @throws throw log message if name already exist .. no overwrite / no registration
         */
        void registerDumpFunction(const String& name, DumpFunctionPointer_t func, uint32_t userID=0) {
            if (name =="") return;  // no registration wanted
            String _name = name;
            if (_dumpFunctions.find(_name) != _dumpFunctions.end()) {
                LOG("Dump function with name '" + _name + "' is already registered.");
            }
            dumpEntry_struct elm;
            elm.p = func;
            elm.userID = userID;
            _dumpFunctions[_name] = elm;
        }

        /**
         * @brief Unregister a dump function by its name.
         * @param name The name of the dump function to unregister.
         */
        void unregisterDumpFunction(const String& name) {
            if (name =="") return;  // no registration wanted
            String _name = name;
            _dumpFunctions.erase(_name);
        }

        /**
         * @brief Call a registered dump function by its name.
         * @param name The name of the dump function to call.
         * @param now_ms The current timestamp in milliseconds.
         * @return The result of the dump function as a String.
         * @throws runtime_error if no function is registered with the given name.
         */
        String callDumpFunction(const String& name, uint32_t now_ms) const {
            if (name =="") return "";  // no registration wanted
            String _name = name;
            auto it = _dumpFunctions.find(_name);
            if (it == _dumpFunctions.end()) {
                LOG("No dump function registered with name '" + _name + "'.");
            }
            return it->second.p(now_ms,it->second.userID);
        }

        /**
         * @brief List all registered dump function names.
         * @return A String containing all registered names, separated by commas.
         */
        String list() const {
            String result;
            for (const auto& entry : _dumpFunctions) {
                if (result.length() > 0) {
                    result += ", ";
                }
                result += entry.first;
            }
            return (result.length() == 0) ? "No registered dump functions." : result;
        }


    private:
        Dumper() = default; // Private constructor for Singleton
        ~Dumper() = default;

        Dumper(const Dumper&) = delete;            // Copy constructor deleted
        Dumper& operator=(const Dumper&) = delete; // Assignment operator deleted

        std::map<String, dumpEntry_struct> _dumpFunctions; // Map of registered dump functions
};

/**
 * @class Dump
 * @brief A base class that provides functionality for registering and managing dump functions.
 * 
 * Classes inheriting from `Dump` can easily integrate with the `Dumper` system by overriding
 * the `dump()` method and providing a unique name for registration.
 * 
 * @example:
class MyClass : public Dump {
    public:
        MyClass(const String& name, int value) : Dump(name), _value(value) {}
    
        String dump(uint32_t now_ms) const override {
            return "Object '" + _name + "' at time " + String(now_ms) + " ms, value: " + String(_value);
        }
    
    private:
        int _value; ///< An example value to include in the dump output.
    };
 */


class Dump {
    public:
        /**
         * @brief Constructor that registers the dump function with the Dumper.
         * @param name The unique name for this object's dump function.
         */
        explicit Dump(const String& name) : _dumpName(name) {
            Dumper::getInstance().registerDumpFunction(_dumpName, [this](uint32_t now_msec,uint32_t userID) {
                return this->dump(now_msec,userID);
            });
        }
    
        /**
         * @brief Destructor that unregisters the dump function from the Dumper.
         */
        virtual ~Dump() {
            Dumper::getInstance().unregisterDumpFunction(_dumpName);
        }
    
        /**
         * @brief The dump function to be overridden by derived classes.
         * @param now_ms The current timestamp in milliseconds.
         * @return A String containing the dump information.
         */
        virtual String dump(uint32_t now_ms,uint32_t userID) const = 0;

        String getDumpName() const {return _dumpName;}
    
    protected:
        String _dumpName; ///< The unique name for this object's dump function.
};




/**
 * @class SystemInfo
 * @brief A class that provides system status information for the RP2040.
 * 
 * This class is derived from `Dump` and registers itself with the `Dumper` system.
 * It provides a `dump` function that outputs the current system time, heap size,
 * used heap, stack size, and used stack.
 * 
 * Example usage:
 * @code
 * SystemStatus systemStatus("RP2040Status");
 * Serial.println(systemStatus.dump(millis(), 0));
 * @endcode
 */
class SystemInfo : public Dump {
    public:
        /**
         * @brief Constructor for SystemStatus.
         * @param name The unique name for this object's dump function.
         */
        explicit SystemInfo(const String& name = "SystemInfo");

        /**
         * @brief The dump function that provides system status information.
         * @param now_ms The current timestamp in milliseconds.
         * @param userID An optional user ID (not used in this implementation).
         * @return A String containing the system status information.
         */
        String dump(uint32_t now_ms, uint32_t userID) const override;
    private:
        String _getFileList(const String& path="/", const String & ident="") const;
};

extern Dumper& dumper;

