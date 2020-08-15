#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include <ArduinoJson.h>

const uint8_t MAX_MENU_DEPTH = 8;

class Menu
{
  public:
    void setup();
    void loop();
    void initMenu();
    JsonObject getActiveMenuItem();
    String commitMenu();
    void rotateMenu(int rotation);
  
  private:
    int menuSelection[MAX_MENU_DEPTH] = {0, -1, -1, -1, -1, -1, -1, -1};
    StaticJsonDocument<1000> json;
    JsonObject currentObject;
    JsonObject parentObject;
    uint8_t currentDepth = 0;
};

#endif