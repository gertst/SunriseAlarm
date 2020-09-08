#include "Menu.h"
#include "Arduino.h"
#include <ArduinoJson.h> //https://arduinojson.org/


void Menu::setup() {
    /**
     * REMARK: the menu.json file is NOT used! It's there as a source to
     * build the below json string. To do that, use https://arduinojson.org/v6/assistant/
    **/
    const char* jsonString = "{\"label\":\"Menu\",\"id\":\"Root\",\"items\":[{\"id\":\"Alarm\",\"label\":\"Alarm\",\"items\":[{\"id\":\"AlarmOnOff\",\"label\":\"On|Off\",\"back\":0},{\"id\":\"SetAlarm\",\"label\":\"Set\"},{\"id\":\"Back\",\"label\":\"Back\"}]},{\"label\":\"Light\",\"items\":[{\"id\":\"LightOn\",\"label\":\"On\"},{\"id\":\"LightOff\",\"label\":\"Off\"},{\"id\":\"Back\",\"label\":\"Back\"}]},{\"id\":\"Back\",\"label\":\"Back\"}]}";

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
        //go back?
        if (currentObject.containsKey("back")) {
            if (currentObject["back"] >= 1) {
                menuSelection[currentDepth] = -1;
            }
            if (currentObject["back"] >= 2) {
                menuSelection[currentDepth - 1] = -1;
            }
            if (currentObject["back"] >= 3) {
                menuSelection[currentDepth - 2] = -1;
            }
        } else {
            menuSelection[currentDepth] = -1;
        }
        
        return currentObject["id"];
    }
}

void Menu::rotateMenu(int rotation) {
    getActiveMenuItem();
    // Serial.print(F("menuSelection: "));
    // for(int i = 0; i < MAX_MENU_DEPTH; i++) {
    //     Serial.print(menuSelection[i]);
    //     Serial.print(", ");
    // }
    // Serial.println("");
    // Serial.print(F("currentDepth:"));
    // Serial.println(currentDepth);
    // Serial.print(F("menuSelection[currentDepth]:"));
    // Serial.println(menuSelection[currentDepth]);

    if (rotation < 0 && menuSelection[currentDepth] == 0) {
        menuSelection[currentDepth] = parentArray.size() - 1;
    } else {
        menuSelection[currentDepth] = (menuSelection[currentDepth] + rotation) % parentArray.size();
    }
}