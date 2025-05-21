#include "StringId.hpp"
#include <helper.h> // for stringHash-Funktion
#include <Debug.hpp> // for ASSERT

// get Singleton-Instance
StringID& StringID::getInstance() {
    static StringID instance;
    return instance;
}
// register string and return the hash
uint32_t StringID::registerString(const char* name) {
    if (strlen(name) == 0) {
        // empty string, return 0
        LOG("StringID::registerString: empty string");
        return 0;
    }

    String nameStr = name;
    uint32_t hash = stringHash(nameStr);
    if (hash == 0) {
        // empty string, return 0
        LOG("StringID::registerString: empty string");
        return 0;
    }
    _mutex.lock();
    
    auto it = _hashToString.find(hash);
    if (it != _hashToString.end()) {
        // name already registered, return its ID
        _mutex.free();
        return hash;
    }
    // new ==> register the name
    _hashToString[hash] = name;

    _mutex.free();
    return hash;
}

// return name to id
const char* StringID::getString(uint32_t id) {
    _mutex.lock();
    auto it = _hashToString.find(id);
    const char* result = (it != _hashToString.end()) ? it->second : nullptr;
    _mutex.free();
    return result;
}

// return id to name .. return 0 if name not registered
uint32_t StringID::getID(const char* name) {
    _mutex.lock();
    String nameStr = name;
    uint32_t hash = stringHash(nameStr);
    ASSERT(hash != 0, "Hash is zero, invalid name");
    
    auto it = _hashToString.find(hash);
    if (it != _hashToString.end()) {
        // name found return ID
        _mutex.free();
        return hash;
    }

    _mutex.free();
    return 0;
}
