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
    
    // Getter for string values
    String getString(const String& key);
    bool getString(const String& key, String& value);
    bool getStringOrDefault(const String& key, const String& defaultValue, String& value);

    // Getter for boolean values
    bool getBool(const String& key);
    bool getBool(const String& key, bool& value);
    bool getBoolOrDefault(const String& key, bool defaultValue, bool& value);

    // Getter for integer values
    int getInt(const String& key);
    bool getInt(const String& key, int& value);
    bool getIntOrDefault(const String& key, int defaultValue, int& value);

    // Getter for hexadecimal values
    int getHex(const String& key);
    bool getHex(const String& key, int& value);
    bool getHexOrDefault(const String& key, int defaultValue, int& value);

    // Setter for values
    void setString(const String& key, const String& value);
    void setBool(const String& key, bool value);
    void setInt(const String& key, int value);
    void setHex(const String& key, int value);

    // JSON handling
    String toString();
    bool fromString(const String& jsonString);
    bool mergeFromString(const String& jsonString);

    // use hash value as key 

    String getString(uint32_t id);
    bool getString(uint32_t id, String& value);
    bool getStringOrDefault(uint32_t id, const String& defaultValue, String& value);
    
    bool getBool(uint32_t id);
    bool getBool(uint32_t id, bool& value);
    bool getBoolOrDefault(uint32_t id, bool defaultValue, bool& value);

    int getInt(uint32_t id);
    bool getInt(uint32_t id, int& value);
    bool getIntOrDefault(uint32_t id, int defaultValue, int& value);

    int getHex(uint32_t id);
    bool getHex(uint32_t id, int& value);
    bool getHexOrDefault(uint32_t id, int defaultValue, int& value);

    void setString(uint32_t id, const String& value);
    void setBool(uint32_t id, bool value);
    void setInt(uint32_t id, int value);
    void setHex(uint32_t id, int value);

private:
    String _filename;           // The filename of the JSON configuration file
    JsonDocument _configData; // The internal representation of the configuration data

    // Helper function to get the key string from an ID
    String getKeyFromID(uint32_t id) const;
};



