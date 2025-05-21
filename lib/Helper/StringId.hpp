#pragma once

#include <Arduino.h>
#include <unordered_map>
#include <Mutex.hpp> // Für Thread-Sicherheit

class StringID {
public:
    // Singleton-Instanz abrufen
    static StringID& getInstance();

    // Registriert einen String und gibt den zugehörigen Hash zurück
    uint32_t registerString(const char* name);
    uint32_t registerString(const String& name) { return registerString(name.c_str()); }

    // Gibt den String zu einer ID zurück
    const char* getString(uint32_t id);

    // Gibt die ID zu einem String zurück (0, wenn nicht gefunden)
    uint32_t getID(const char* name);
    uint32_t getID(const String& name) { return getID(name.c_str()); }


private:
    // Privater Konstruktor für Singleton
    StringID() {}

    // Kopieren und Zuweisung verhindern
    StringID(const StringID&) = delete;
    StringID& operator=(const StringID&) = delete;

    // Hash-zu-String-Mapping
    std::unordered_map<uint32_t, const char*> _hashToString;

    // Mutex für Thread-Sicherheit
    Mutex _mutex;
};

// macro to define a value and its text representation 
#ifdef DEFINE_STRING_ID_HERE
    #define DEFINE_STRING_ID(name)  extern const uint32_t name = StringID::getInstance().registerString(#name);
#else 
    #define DEFINE_STRING_ID(name)  extern const uint32_t name; 
#endif


