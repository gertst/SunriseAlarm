#include "Menu.h"
#include "Arduino.h"
#include <ArduinoJson.h>


void Menu::setup() {

    const char* jsonString = "{\"label\":\"Menu\",\"items\":[{\"label\":\"Alarm\",\"items\":[{\"id\":\"AlarmOn\",\"label\":\"on\"},{\"id\":\"AlarmOff\",\"label\":\"off\"}]},{\"label\":\"Light\",\"items\":[{\"id\":\"LightOn\",\"label\":\"on\"},{\"id\":\"LightOff\",\"label\":\"off\"}]}]}";

    DeserializationError error = deserializeJson(json, jsonString);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    
}

JsonObject Menu::getActiveMenuItem() {
    //const char* label = "?";
    JsonObject obj = json.as<JsonObject>();
    parentObject = obj;
    Serial.print("-----cur :");
    serializeJson(obj["label"], Serial);
    int i;
    for (i = 0; i <= MAX_MENU_DEPTH; i++) {
        if (menuSelection[i] > -1 && obj.containsKey("items")) {
            parentObject = obj;
            obj = obj["items"][menuSelection[i]];
        } else {
            break;
        }
    }
    currentDepth = i;
    currentObject = obj;
    return currentObject;
}

void Menu::initMenu() {
    for (uint8_t i = 1; i <= MAX_MENU_DEPTH; i++) {
        menuSelection[i] = -1;
    }
    menuSelection[0] = 0;
}

String Menu::commitMenu() {
    getActiveMenuItem();
    //can we go deeper into the menu? (=items exist)   
    if (currentObject.containsKey("items")) {
        menuSelection[currentDepth + 1] = 0;
        return "";
    } else {
        return currentObject["id"];
    }
}

void Menu::rotateMenu(int rotation) {
    getActiveMenuItem();
    Serial.print(F("menuSelection: "));
    for(int i = 0; i < MAX_MENU_DEPTH; i++) {
        Serial.print(menuSelection[i]);
        Serial.print(", ");
    }
    Serial.println("");
    Serial.print(F("currentDepth:"));
    Serial.println(currentDepth);
    Serial.print(F("menuSelection[currentDepth]:"));
    Serial.println(menuSelection[currentDepth]);
    Serial.print(F("parentObject label:"));
    Serial.println(parentObject["label"].as<String>());
    JsonArray items = parentObject["items"];
    menuSelection[currentDepth] = (menuSelection[currentDepth] + rotation) % items.size();
}