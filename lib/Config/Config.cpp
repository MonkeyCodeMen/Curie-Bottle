#include "config.hpp"
#include <Debug.hpp>
#include <ArduinoJson.h>

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

// Getter for string values
String Config::getString(const String& key) {
    return _configData[key] | ""; // Returns an empty string if the key does not exist
}

bool Config::getString(const String& key, String& value) {
    if (_configData[key].is<String>()) {
        value = _configData[key].as<String>();
        return true;
    }
    LOG("Key not found: " + key);
    return false;
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

// Getter for boolean values
bool Config::getBool(const String& key) {
    return _configData[key] | false; // Returns false if the key does not exist
}

bool Config::getBool(const String& key, bool& value) {
    if (_configData[key].is<bool>()) {
        value = _configData[key].as<bool>();
        return true;
    }
    LOG("Key not found: " + key);
    return false;
}

// Getter for boolean values with default
bool Config::getBoolOrDefault(const String& key, bool defaultValue, bool& value) {
    if (_configData[key].is<bool>()) {
        value = _configData[key].as<bool>();
        return true;
    }
    // Add the key with the default value to the JSON document
    setBool(key, defaultValue);
    value = defaultValue;
    LOG("Key not found: " + key + ", setting default value: " + String(defaultValue));
    return false;
}

// Getter for integer values
int Config::getInt(const String& key) {
    return _configData[key] | 0; // Returns 0 if the key does not exist
}

bool Config::getInt(const String& key, int& value) {
    if (_configData[key].is<int>()) {
        value = _configData[key].as<int>();
        return true;
    }
    LOG("Key not found: " + key);
    return false;
}

// Getter for integer values with default
bool Config::getIntOrDefault(const String& key, int defaultValue, int& value) {
    if (_configData[key].is<int>()) {
        value = _configData[key].as<int>();
        return true;
    }
    // Add the key with the default value to the JSON document
    setInt(key, defaultValue);
    value = defaultValue;
    LOG("Key not found: " + key + ", setting default value: " + String(defaultValue));
    return false;
}

// Getter for hexadecimal values
int Config::getHex(const String& key) {
    return strtol(_configData[key] | "0", nullptr, 16); // Returns 0 if the key does not exist
}

bool Config::getHex(const String& key, int& value) {
    if (_configData[key].is<const char *>()) {
        value = strtol(_configData[key].as<const char*>(), nullptr, 16);
        return true;
    }
    LOG("Key not found: " + key);
    return false;
}

// Getter for hexadecimal values with default
bool Config::getHexOrDefault(const String& key, int defaultValue, int& value) {
    if (_configData[key].is<const char *>()) {
        value = strtol(_configData[key].as<const char*>(), nullptr, 16);
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
    ASSERT(key != nullptr, "Invalid ID: Key not found");
    return String(key);
}

// Getter for string values by ID
String Config::getString(uint32_t id) {
    return getString(getKeyFromID(id));
}

bool Config::getString(uint32_t id, String& value) {
    return getString(getKeyFromID(id), value);
}

bool Config::getStringOrDefault(uint32_t id, const String& defaultValue, String& value) {
    return getStringOrDefault(getKeyFromID(id), defaultValue, value);
}

// Getter for boolean values by ID
bool Config::getBool(uint32_t id) {
    return getBool(getKeyFromID(id));
}

bool Config::getBool(uint32_t id, bool& value) {
    return getBool(getKeyFromID(id), value);
}

bool Config::getBoolOrDefault(uint32_t id, bool defaultValue, bool& value) {
    return getBoolOrDefault(getKeyFromID(id), defaultValue, value);
}

// Getter for integer values by ID
int Config::getInt(uint32_t id) {
    return getInt(getKeyFromID(id));
}

bool Config::getInt(uint32_t id, int& value) {
    return getInt(getKeyFromID(id), value);
}

bool Config::getIntOrDefault(uint32_t id, int defaultValue, int& value) {
    return getIntOrDefault(getKeyFromID(id), defaultValue, value);
}

// Getter for hexadecimal values by ID
int Config::getHex(uint32_t id) {
    return getHex(getKeyFromID(id));
}

bool Config::getHex(uint32_t id, int& value) {
    return getHex(getKeyFromID(id), value);
}

bool Config::getHexOrDefault(uint32_t id, int defaultValue, int& value) {
    return getHexOrDefault(getKeyFromID(id), defaultValue, value);
}

// Setter for string values by ID
void Config::setString(uint32_t id, const String& value) {
    setString(getKeyFromID(id), value);
}

// Setter for boolean values by ID
void Config::setBool(uint32_t id, bool value) {
    setBool(getKeyFromID(id), value);
}

// Setter for integer values by ID
void Config::setInt(uint32_t id, int value) {
    setInt(getKeyFromID(id), value);
}

// Setter for hexadecimal values by ID
void Config::setHex(uint32_t id, int value) {
    setHex(getKeyFromID(id), value);
}