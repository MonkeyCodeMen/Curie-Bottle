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

/**
 * @class Config
 * @brief A class for managing configuration data stored in a JSON file using LittleFS.
 * 
 * The Config class provides an easy-to-use interface for reading, writing, and managing
 * configuration data in a JSON file. It supports various data types such as strings, booleans,
 * integers, and hexadecimal values. The class also allows setting default values for missing keys
 * and automatically adds them to the configuration file.
 * 
 * Example usage:
 * 
 * @code
 * Config config("/config.json");
 * 
 * // Load configuration
 * if (config.load()) {
 *     Serial.println("Configuration loaded successfully");
 * } else {
 *     Serial.println("Failed to load configuration");
 * }
 * 
 * // Get or set default values
 * String hostname;
 * if (!config.getStringOrDefault("hostname", "default.local", hostname)) {
 *     Serial.println("Default hostname set: " + hostname);
 * }
 * 
 * int port;
 * if (!config.getIntOrDefault("port", 8080, port)) {
 *     Serial.println("Default port set: " + String(port));
 * }
 * 
 * // Save configuration
 * if (config.save()) {
 *     Serial.println("Configuration saved successfully");
 * }
 * @endcode
 */

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Debug.hpp>
#include <StringId.h>

#define CONFIG_DEFAULT_FILE "/config.json" // Default configuration file name

#define CONFIG_DEFAULT_STRING_VALUE     ""      // Default string value for missing keys
#define CONFIG_DEFAULT_BOOL_VALUE       false   // Default boolean value for missing keys
#define CONFIG_DEFAULT_INT_VALUE        0       // Default integer value for missing keys    
#define CONFIG_DEFAULT_UINT_VALUE       0       // Default integer value for missing keys    
#define CONFIG_DEFAULT_HEX_VALUE        0       // Default hexadecimal value for missing keys
#define CONFIG_DEFAULT_KEY              "default" // Default key for missing values

class Config : public Dump{
    public:
        /**
         * @brief Constructor for the Config class.
         * @param filename The name of the JSON configuration file (default: "/config.json").
         */
        Config(String filename = CONFIG_DEFAULT_FILE, String objectName = "config");
        void begin(); // Initialize the configuration file system

        /**
         * @brief Loads the configuration from the JSON file.
         * @return True if the configuration was loaded successfully, false otherwise.
         */
        bool load();

        /**
         * @brief Saves the current configuration to the JSON file.
         * @return True if the configuration was saved successfully, false otherwise.
         */
        bool save();
        /**
         * @brief Dumps the current configuration state as a JSON string.
         * @param now_ms The current timestamp in milliseconds.
         * @return A String containing the dump information.
         */
        String dump(uint32_t now_ms, uint32_t userID) const override;

                // JSON handling
        String toString();
        bool fromString(const String& jsonString);
        bool mergeFromString(const String& jsonString);


        // Getter for string values
        bool getStringOrDefault(const String& key, const String& defaultValue, String& value);  
        inline String getString(const String& key)              { String value; getStringOrDefault(key, CONFIG_DEFAULT_STRING_VALUE, value); return value;} 
        inline bool getString(const String& key, String& value) { return getStringOrDefault(key, CONFIG_DEFAULT_STRING_VALUE, value);}
        inline String getString(uint32_t id)                    { return getString(getKeyFromID(id));}    
        inline bool getString(uint32_t id, String& value)       { return getString(getKeyFromID(id), value); }
        inline bool getStringOrDefault(uint32_t id, const String& defaultValue, String& value) { return getStringOrDefault(getKeyFromID(id), defaultValue, value); } 
        
        // Getter for boolean values
        bool getBoolOrDefault(const String& key, bool defaultValue, bool& value);
        inline bool getBool(const String& key)                  { bool value; getBoolOrDefault(key, CONFIG_DEFAULT_BOOL_VALUE, value); return value;}
        inline bool getBool(const String& key, bool& value)     { return getBoolOrDefault(key, CONFIG_DEFAULT_BOOL_VALUE, value);  }
        inline bool getBool(uint32_t id)                        { return getBool(getKeyFromID(id));} 
        inline bool getBool(uint32_t id, bool& value)           { return getBool(getKeyFromID(id), value); }
        inline bool getBoolOrDefault(uint32_t id, bool defaultValue, bool& value) { return getBoolOrDefault(getKeyFromID(id), defaultValue, value);}

        // Getter for integer values
        bool getIntOrDefault(const String& key, int defaultValue, int& value);
        inline int getInt(const String& key)                    { int value; getIntOrDefault(key, CONFIG_DEFAULT_INT_VALUE, value); return value;}
        inline bool getInt(const String& key, int& value)       { return getIntOrDefault(key, CONFIG_DEFAULT_INT_VALUE, value);   }
        inline int getInt(uint32_t id)                          { return getInt(getKeyFromID(id)); }
        inline bool getInt(uint32_t id, int& value)             { return getIntOrDefault(getKeyFromID(id), 0, value); }  
        inline bool getIntOrDefault(uint32_t id, int defaultValue, int& value) {  return getIntOrDefault(getKeyFromID(id), defaultValue, value);}

        // Getter for hexadecimal values
        bool getHexOrDefault(const String& key, int defaultValue, int& value);
        inline int getHex(const String& key)                    { int value; getHexOrDefault(key, CONFIG_DEFAULT_HEX_VALUE, value); return value;}
        inline bool getHex(const String& key, int& value)       { return getHexOrDefault(key, CONFIG_DEFAULT_HEX_VALUE, value); }
        inline int getHex(uint32_t id)                          { return getHex(getKeyFromID(id)); }      
        inline bool getHex(uint32_t id, int& value)             { return getHexOrDefault(getKeyFromID(id), 0, value);}
        inline bool getHexOrDefault(uint32_t id, int defaultValue, int& value) { return getHexOrDefault(getKeyFromID(id), defaultValue, value);}

        // Setter for values
        void setString(const String& key, const String& value);
        void setBool(const String& key, bool value);
        void setInt(const String& key, int value);
        void setHex(const String& key, int value);
        inline void setString(uint32_t id, const String& value) { setString(getKeyFromID(id), value); }
        inline void setBool(uint32_t id, bool value)            { setBool(getKeyFromID(id), value); }
        inline void setInt(uint32_t id, int value)              { setInt(getKeyFromID(id), value); }
        inline void setHex(uint32_t id, int value)              { setHex(getKeyFromID(id), value); }

    private:
        String _filename;           // The filename of the JSON configuration file
        JsonDocument _configData; // The internal representation of the configuration data

        // Helper function to get the key string from an ID
        String getKeyFromID(uint32_t id) const;
};



