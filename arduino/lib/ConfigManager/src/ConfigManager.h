#pragma once
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>

class ConfigManager {
public:
    JsonDocument config;

    ConfigManager(const char* filename);
    void begin();
    bool load();
    bool save();

    template<typename T>
    void set(const char* path, const T& value) {
        setPath(config.as<JsonVariant>(), path, value);
    }

    JsonVariant get(const char* path);
    void displayFileContent();
    void listFiles();


private:
    const char* filename;

    template<typename T>
    void setPath(JsonVariant obj, const String& path, const T& value) {
        int dotIndex = path.indexOf('.');
        if (dotIndex < 0) {
            obj[path] = value;
            return;
        }
        String key = path.substring(0, dotIndex);
        String rest = path.substring(dotIndex + 1);
        if (!obj[key].is<JsonObject>()) obj[key].to<JsonObject>();
        setPath(obj[key], rest, value);
    }
    JsonVariant getPath(JsonVariant obj, const String& path);
};
