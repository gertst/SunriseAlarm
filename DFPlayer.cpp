#include "DFPlayer.h"
#include "Arduino.h"
#include "Mqtt.h"

// This line says to use the same gMyEventMgr defined elsewhere (in Main.cpp in this case)
extern Mqtt mqtt;


//https://github.com/PowerBroker2/DFPlayerMini_Fast
//https://forum.arduino.cc/index.php?topic=513128.15
#include <DFPlayerMini_Fast.h>


void DFPlayer::setup() {
    softwareSerial.begin(9600);
    dfPlayer.begin(softwareSerial);
}

void DFPlayer::loop() {
    if (this->timerTrigger != 0 && this->timerTrigger < millis()) {
        this->increaseVolume();
    }
}

void DFPlayer::increaseVolume() {
    if (volume < this->volumeTarget) {
        volume++;
        //dfPlayer.volume(volume);
        mqtt.publish("sunriseAlarm/volume", (String)volume);
        this->timerTrigger = millis() + 20000;
    } else {
        this->timerTrigger = 0; //disable
    }
}

void DFPlayer::command(String topic, String msg) {
    if (topic == "sunriseAlarm/volume") { 
        volume = msg.toInt();
        dfPlayer.volume(volume); //range: 0 - 30
        //mqtt.publish("sunriseAlarm/volume", (String)dfPlayer.currentVolume());
    } else if (topic == "sunriseAlarm/play") { 
        dfPlayer.wakeUp();
        dfPlayer.play(msg.toInt()); 
    } else if (topic == "sunriseAlarm/playNext") { 
        dfPlayer.playNext(); 
    } else if (topic == "sunriseAlarm/playPrevious") { 
        dfPlayer.playPrevious(); 
    } else if (topic == "sunriseAlarm/equalizer") { 
        dfPlayer.EQSelect(msg.toInt()); //range: 0 - 5 (or 1 to 6??) 
    } else if (topic == "sunriseAlarm/stopMusic") { 
        dfPlayer.stop();
        dfPlayer.sleep();
    } else if (topic == "sunriseAlarm/volumeRaiseTo") { 
        volume = 1;
        dfPlayer.volume(volume);
        mqtt.publish("sunriseAlarm/volume", (String)volume);
        //dfPlayer.volume(volume); //range: 0 - 30
        this->volumeTarget = msg.toInt();
        this->timerTrigger = millis() + 100;
    }
}




