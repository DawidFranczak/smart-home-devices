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
    File file = LittleFS.open(filename, "r"); // <-- nowe otwarcie
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

void ConfigManager::listFiles() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS init failed!");
        return;
    }

    Serial.println("=== Lista plików w LittleFS ===");

    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
        Serial.print("Plik: ");
        Serial.print(dir.fileName());
        Serial.print("\tRozmiar: ");
        Serial.println(dir.fileSize());
    }

    Serial.println("=== Koniec listy ===");
}
