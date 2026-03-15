#include "ConfigManager.h"
#include <Arduino.h>

ConfigManager::ConfigManager(const char* filename) : config(),filename(filename) {}

void ConfigManager::begin(){
    if (!LittleFS.begin()) {
        Serial.println("LittleFS init failed!");
        return;
    }
    load();
}

bool ConfigManager::load() {
    Serial.println("LittleFS load!");
    if (!LittleFS.exists(filename)) {
        Serial.println("config.json missing");
        return true;
    }

    File file = LittleFS.open(filename, "r");
    if (!file) {
        Serial.println("Could not open config file");
        return false;
    }
    auto err = deserializeJson(config, file);
    file.close();

    if (err) {
        Serial.println("JSON parse error, creating default config");
        save();
        return false;
    }

    Serial.println("Config loaded.");
    displayFileContent();
    return true;
}

bool ConfigManager::save() {
    File file = LittleFS.open(filename, "w");
    if (!file) {
        Serial.println("Could not write config file");
        return false;
    }
    serializeJsonPretty(config, file);
    file.close();
    Serial.println("Config saved.");

    displayFileContent();
    return true;
}

void ConfigManager::displayFileContent(){
    File file = LittleFS.open(filename, "r");
    if (!file) {
        Serial.println("Could not open config file");
        return;
    }

    Serial.println("=== Zawartość pliku ===");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
    Serial.println("\n=== Koniec pliku ===");
}
JsonVariant ConfigManager::get(const char* path) {
    return getPath(config.as<JsonVariant>(), path);
}

JsonVariant ConfigManager::getPath(JsonVariant obj, const String& path) {
    int dotIndex = path.indexOf('.');
    if (dotIndex < 0) {
        return obj[path];
    }

    String key = path.substring(0, dotIndex);
    String rest = path.substring(dotIndex + 1);

    return getPath(obj[key], rest);
}

bool ConfigManager::exists(const char* path) {
    return existsPath(config.as<JsonVariant>(), path);
}

bool ConfigManager::existsPath(JsonVariant obj, const String& path) {
    int dotIndex = path.indexOf('.');
    if (dotIndex < 0) {
        return !obj[path].isNull();
    }
    String key = path.substring(0, dotIndex);
    String rest = path.substring(dotIndex + 1);

    return existsPath(obj[key], rest);
}

void ConfigManager::removeSection(const char* path) {
    JsonVariant obj = get(path);
    if(!obj.isNull()) {
        int dotIndex = String(path).lastIndexOf('.');
        if(dotIndex < 0) {
            config.remove(path);
        } else {
            String parentPath = String(path).substring(0, dotIndex);
            String key = String(path).substring(dotIndex + 1);
            JsonVariant parent = get(parentPath.c_str());
            if(parent.is<JsonObject>()) parent.as<JsonObject>().remove(key);
        }
    }
}

void ConfigManager::listFiles() {
    if (!LittleFS.begin()) {
    Serial.println("LittleFS init failed!");
    return;
    }

    Serial.println("=== Lista plików w LittleFS ===");

    #if defined(ESP8266)

    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
        Serial.print("Plik: ");
        Serial.print(dir.fileName());
        Serial.print("\tRozmiar: ");
        Serial.println(dir.fileSize());
    }

    #elif defined(ESP32)

    File root = LittleFS.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Nie można otworzyć katalogu");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        Serial.print("Plik: ");
        Serial.print(file.name());
        Serial.print("\tRozmiar: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }

    #endif

    Serial.println("=== Koniec listy ===");
}