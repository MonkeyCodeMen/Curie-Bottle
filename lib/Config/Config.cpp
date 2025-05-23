#include "config.hpp"
#include <Debug.hpp>
#include <ArduinoJson.h>
#include <helper.h>

Config::Config(String filename, String objectName) : Dump(objectName), _filename(filename) {}

void Config::begin() {
    if (!LittleFS.begin()) {
        LOG("Failed to mount LittleFS");
    }
}

bool Config::load() {
    File file = LittleFS.open(_filename, "r");
    if (!file) {
        LOG("Failed to open config file for reading");
        return false;
    }

    DeserializationError error = deserializeJson(_configData, file);
    file.close();

    if (error) {
        LOG("Failed to parse JSON content");
        return false;
    }

    return true;
}

bool Config::save() {
    File file = LittleFS.open(_filename, "w");
    if (!file) {
        LOG("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(_configData, file) == 0) {
        LOG("Failed to write JSON content");
        file.close();
        return false;
    }

    file.close();
    return true;
}

String Config::dump(uint32_t now_ms, uint32_t userID) const {
    String output = "Config Dump at " + String(now_ms) + " ms:\n";

    // Füge den Dateinamen hinzu
    output += "  Filename: " + _filename + "\n";

    // Füge die Konfigurationsdaten als JSON-String hinzu
    String jsonString;
    serializeJson(_configData, jsonString);
    output += "  Configuration Data:\n" + jsonString + "\n";

    return output;
}

// Getter for string values with default
bool Config::getStringOrDefault(const String& key, const String& defaultValue, String& value) {
    if (_configData[key].is<String>()) {
        value = _configData[key].as<String>();
        return true;
    }
    // Add the key with the default value to the JSON document
    setString(key, defaultValue);
    value = defaultValue;
    LOG("Key not found: " + key + ", setting default value: " + defaultValue);
    return false;
}


// Getter for boolean values with default
bool Config::getBoolOrDefault(const String& key, bool defaultValue, bool& value) {
        // If the value is a boolean, return it directly
    if (_configData[key].is<bool>()) {
        value = _configData[key].as<bool>();
        return true;
    }
    // If the value is an integer (e.g., 0 or 1)
    if (_configData[key].is<int>()) {
        value = _configData[key].as<int>() != 0;
        return true;
    }
    // If the value is a string, try to interpret it as a boolean
    if (_configData[key].is<const char*>()) {
        String val = _configData[key].as<const char*>();
        val.toLowerCase();
        if      (val == "true"  || val == "1" || val == "yes" || val == "on" ) value = true;
        else if (val == "false" || val == "0" || val == "no"  || val == "off") value = false;
        else {
            // Fallback: return false if the string cannot be interpreted
            value = defaultValue;
            setBool(key, defaultValue);
            LOG("Key not found: " + key);
            return false;
        }
        return true;
    }
    if (_configData[key].is<String>()) {
        String val = _configData[key].as<String>();
        val.toLowerCase();
        if      (val == "true"  || val == "1" || val == "yes" || val == "on" ) value = true;
        else if (val == "false" || val == "0" || val == "no"  || val == "off") value = false;
        else {
            // Fallback: return false if the string cannot be interpreted
            value = defaultValue;
            setBool(key, defaultValue);
            LOG("Key not found: " + key);
            return false;
        }
        return true;
    }
    // Fallback: return false if the key does not exist or cannot be interpreted
    value = defaultValue;
    setBool(key, defaultValue);
    LOG("Key not found: " + key);
    return false;
}


// Getter for integer values with default
bool Config::getIntOrDefault(const String& key, int defaultValue, int& value) {
    // If the value is an integer, return it directly
    if (_configData[key].is<int>()) {
        value = _configData[key].as<int>();
        return true;    
    }
    // If the value is a string (const char*), try to convert it to int
    if (_configData[key].is<const char*>()) {
        value = atoi(_configData[key].as<const char*>());
        return true;
    }
    // If the value is an Arduino String, try to convert it to int
    if (_configData[key].is<String>()) {
        value = _configData[key].as<String>().toInt();
        return true;
    }
    // Fallback: return 0 if the key does not exist or cannot be interpreted as int
    value = defaultValue;
    setInt(key, defaultValue);
    LOG("Key not found: " + key);
    return false;
}


// Getter for hexadecimal values with default
bool Config::getHexOrDefault(const String& key, int defaultValue, int& value) {
    String temp;
    // If the value is a string (const char*), try to convert it to int
    if (_configData[key].is<const char*>()) {
        temp = _configData[key].as<const char*>();
        value = convertStrToInt(temp);
        return true;
    }
    // If the value is an Arduino String, try to convert it to int
    if (_configData[key].is<String>()) {
        temp = _configData[key].as<String>();
        value = convertStrToInt(temp);
        return true;
    }

    // Add the key with the default value to the JSON document
    setHex(key, defaultValue);
    value = defaultValue;
    LOG("Key not found: " + key + ", setting default value: 0x" + String(defaultValue, HEX));
    return false;
}

// Setter for string values
void Config::setString(const String& key, const String& value) {
    _configData[key] = value;
}

// Setter for boolean values
void Config::setBool(const String& key, bool value) {
    _configData[key] = value;
}

// Setter for integer values
void Config::setInt(const String& key, int value) {
    _configData[key] = value;
}

// Setter for hexadecimal values
void Config::setHex(const String& key, int value) {
    char hexString[10];
    snprintf(hexString, sizeof(hexString), "0x%X", value);
    _configData[key] = hexString;
}

// Converts the configuration to a JSON string
String Config::toString() {
    String jsonString;
    serializeJson(_configData, jsonString);
    return jsonString;
}

// Rebuilds the configuration from a JSON string
bool Config::fromString(const String& jsonString) {
    DeserializationError error = deserializeJson(_configData, jsonString);
    if (error) {
        LOG("Failed to parse JSON string");
        return false;
    }
    return true;
}

// Merges a JSON string into the current configuration
bool Config::mergeFromString(const String& jsonString) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
        LOG("Failed to parse JSON string for merging");
        return false;
    }

    for (JsonPair kv : doc.as<JsonObject>()) {
        _configData[kv.key()] = kv.value();
    }

    return true;
}


String Config::getKeyFromID(uint32_t id) const {
    const char* key = StringID::getInstance().getString(id);
    if (key == nullptr) {
        LOG(F("Invalid ID: Key not found"));
        return String(CONFIG_DEFAULT_KEY);
    }
    return String(key);
}
