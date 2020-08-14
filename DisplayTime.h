#ifndef DisplayTime_h
#define DisplayTime_h

#include "Arduino.h"

class DisplayTime
{
  public:
    void setup();
    String getTime();
    bool isRunning();
    void loop();
  
  //private:
    
};

#endif