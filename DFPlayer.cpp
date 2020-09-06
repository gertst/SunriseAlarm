#include "DFPlayer.h"
#include "Arduino.h"


//https://github.com/PowerBroker2/DFPlayerMini_Fast
//https://forum.arduino.cc/index.php?topic=513128.15
#include <DFPlayerMini_Fast.h>

// DFPlayer(uint8_t rxPin_, uint8_t txPin_) {
//     this->rxPin = rxPin_;
//     this->txPin = txPin_;
// }

void DFPlayer::setup() {
    softwareSerial.begin(9600);
    dfPlayer.begin(softwareSerial);
}

void DFPlayer::loop() {
    // if (!this->isStarted) {
    //     this->isStarted = true;
    //     dfPlayer.playNext(); 
    //     dfPlayer.volume(1); 
    // }
}

void DFPlayer::command(String topic, String msg) {
    if (topic == "sunriseAlarm/volume") { 
        dfPlayer.volume(msg.toInt()); //range: 0 - 30
    } else if (topic == "sunriseAlarm/playNext") { 
        dfPlayer.playNext(); 
    } else if (topic == "sunriseAlarm/playPrevious") { 
        dfPlayer.playPrevious(); 
    } else if (topic == "sunriseAlarm/equalizer") { 
        dfPlayer.EQSelect(msg.toInt()); //range: 0 - 5 (or 1 to 6??) 
    } else if (topic == "sunriseAlarm/stopMusic") { 
        dfPlayer.stop();
    }
}


