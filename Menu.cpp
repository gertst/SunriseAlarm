#include "Menu.h"
#include "Arduino.h"
#include <ArduinoJson.h>


void Menu::setup() {

    const char* jsonString = "{\"label\":\"Menu\",\"id\":\"Root\",\"items\":[{\"label\":\"Alarmpje!\",\"items\":[{\"id\":\"AlarmOn\",\"label\":\"On\"},{\"id\":\"AlarmOff\",\"label\":\"Off\"},{\"id\":\"AlarmSetHour\",\"label\":\"Set hours\",\"items\":[{\"id\":\"0 h\",\"label\":\"0 h\"},{\"id\":\"1 h\",\"label\":\"1 h\"},{\"id\":\"2 h\",\"label\":\"2 h\"},{\"id\":\"3 h\",\"label\":\"3 h\"},{\"id\":\"4 h\",\"label\":\"4 h\"},{\"id\":\"5 h\",\"label\":\"5 h\"},{\"id\":\"6 h\",\"label\":\"6 h\"},{\"id\":\"7 h\",\"label\":\"7 h\"},{\"id\":\"8 h\",\"label\":\"8 h\"},{\"id\":\"9 h\",\"label\":\"9 h\"},{\"id\":\"10 h\",\"label\":\"10 h\"},{\"id\":\"11 h\",\"label\":\"11 h\"},{\"id\":\"12 h\",\"label\":\"12 h\"},{\"id\":\"13 h\",\"label\":\"13 h\"},{\"id\":\"14 h\",\"label\":\"14 h\"},{\"id\":\"15 h\",\"label\":\"15 h\"},{\"id\":\"16 h\",\"label\":\"16 h\"},{\"id\":\"17 h\",\"label\":\"17 h\"},{\"id\":\"18 h\",\"label\":\"18 h\"},{\"id\":\"19 h\",\"label\":\"19 h\"},{\"id\":\"20 h\",\"label\":\"20 h\"},{\"id\":\"21 h\",\"label\":\"21 h\"},{\"id\":\"22 h\",\"label\":\"22 h\"},{\"id\":\"23 h\",\"label\":\"23 h\"}]},{\"id\":\"AlarmSetMinute\",\"label\":\"Set minutes\",\"items\":[{\"id\":\"0 m\",\"label\":\"0 m\"},{\"id\":\"5 m\",\"label\":\"5 m\"},{\"id\":\"10 m\",\"label\":\"10 m\"},{\"id\":\"15 m\",\"label\":\"15 m\"},{\"id\":\"20 m\",\"label\":\"20 m\"},{\"id\":\"25 m\",\"label\":\"25 m\"},{\"id\":\"30 m\",\"label\":\"30 m\"},{\"id\":\"35 m\",\"label\":\"35 m\"},{\"id\":\"40 m\",\"label\":\"40 m\"},{\"id\":\"45 m\",\"label\":\"45 m\"},{\"id\":\"50 m\",\"label\":\"50 m\"},{\"id\":\"55 m\",\"label\":\"55 m\"}]},{\"id\":\"Back\",\"label\":\"Back\"}]},{\"label\":\"Light\",\"items\":[{\"id\":\"LightOn\",\"label\":\"On\"},{\"id\":\"LightOff\",\"label\":\"Off\"},{\"id\":\"Back\",\"label\":\"Back\"}]},{\"id\":\"Back\",\"label\":\"Back\"}]}";

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
    // Serial.print("-----cur :");
    // serializeJson(obj["label"], Serial);
    int i = 0;
    for (i = 0; i <= MAX_MENU_DEPTH; i++) {
        if (menuSelection[i] > -1 && obj.containsKey("items")) {
            parentArray = obj["items"];
            obj = obj["items"][menuSelection[i]];
        } else {
            break;
        }
    }
    currentDepth = i - 1;
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
    if (currentObject["id"] == "Back") {
        menuSelection[currentDepth] = -1;
        return currentObject["id"];
    } else if (currentObject.containsKey("items")) {
        menuSelection[currentDepth + 1] = 0;
        return "";
    } else {
        //go back
        menuSelection[currentDepth] = -1;
        // if (currentDepth > 0) {
        //     menuSelection[currentDepth - 1] = -1;
        // }
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
    
    menuSelection[currentDepth] = (menuSelection[currentDepth] + rotation) % parentArray.size();
}