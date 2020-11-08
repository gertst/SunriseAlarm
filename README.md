# SunriseAlarm

## A digital clock with auditive and visual alarm.

When an alarms goes off, a ledstrip above the bed slowly simulates a sunrise and music starts to play, increasing very slowly.
Different LED light scenes can be set and the wake-up time can be set and changed, both using a rotary knob and remotly. 
The SunriseAlarm can send and receive commmands using MQTT, so remote control is possible with a wide range of devices, like a smartphone or a full blown home automation system.

Here are some photos and a video of the effects: https://photos.app.goo.gl/3kZMjbhgKaqZbkF48

### Component list and why they are used:

1.  ESP8266 NodeMCU LUA CP2102 ESP-12E Internet WiFi Development Board, 5.8$
    The nodeMCU ESP8266 is the brain of my Sunrise alarm clock. 
    It's connected to the internet and gets its time from an Mqtt request that returns the exact local time.

2.  DAOKI MAX7219 Dot led Matrix MCU 8x32 Control LED Display Module Drive, 12.4$
    A Led dotmatix is used to show the digital clock and to show the menu items for the settings menu.

3.  RGB+White Individually Addressable LED Strip from Visdoll, 32$
    5 meters (16.4Ft), 150 Pixels 5050 RGB + White Individually Addressable LEDs, 30Leds/M, DC 5V, Non-Waterproof
    This LED strip is mounted on a wooded frame above the bed. It also holds the mosquito net ;-)

4.  5V Power Supply,PHEVOS, 5v 5A Dc, Universal Switching Power Supply, 12$
    Powers the LED strip and NodeMCU.

5.  Rotary encode module with push botton, <3$
    This rotary knob is reachable from inside the bed and can be used to set the alarm time, turn it on and off, and activate some premade light scenes. 
    These actions (and more) can also be controlled on a smartphone with any MQTT controller app.

6.  LDR, <1$
    This light dependant resistor makes sure that the clock display intensity adapts to the light in the room.
    It took quite some experiments to have a good readable clock in any circomstances. 
    You want it very dimmed at night but the clock should still be readable when natural light or light from the LED strip shines on it. So I programmed it to adapt to the LDR, and also increase intensity when the rotay button is used, and go in idle mode after 30 seconds.
    I can also tweak the intensity factor using my phone.

7.  DFPlayer Mini, a small and low price MP3 module with an simplified output directly to the speaker, 11€ 
    This board makes playing MP3s easy. All controlls (play, pause, volume, ...) can be steered from code. 
    MP3s play directly from a TF memory card.

### The Sunrise
To have the best possible sunrise accuracy, I creates an image in Photoshop. The pixel width of the image is the number of leds of te strip (150 pixels). The height of the image is the time: each row of pixels is a point in time of the sunrise animation. 
So pixel 1,1 shwos the starting color of led 1, and then fades to pixel 1,2 over 20 seconds. The next 20 seconds pixel 1 fades to tehe color of pixel 1,3 and so on.

For now I converted the png image to an array of RGB values and I stored that array directly in my code. But I plan to read this image from internet in order the have an easier update process.

I also discovered that the raw RGB values should be gamma-corrected to show an accurate color, but luckily the NeoPixel library I used had a handy function in place to do that for me.

### The menu
To have a good UI and UX on the clock setting, I did some experiments to find the best menu system using a single rotary knob. This is still work in progress, but at least I already have a system in place to read a json file as a tree structure for the hierachical menu. That makes modifications quite straightforward.

### OOP(s)
Because a lot of different sensors, events and modes are used, I wanted to organise my C++ code as object oriented as possible. But OOP in the Arduino world is hard to find. Quite some beginner coders are available on the Arduino platform, and code examples are poorly written. So I spend a lot of time iterating and refactoring the code to have it more or less as I want. All components are nicely black-boxed in their own cpp file and communicate to each other in the main app file. But since this is my first real c++ project, I'm afraid I still miss a lot of important c++ typical syntax to write elegant c++ code...  The use of pointers is still pointless to me (no pun intended), and I know I have to get rid of Strings and use char arrays, but that's something for the next refactoring ;-)

### OTA
What's cool about the NodeMCU is the over the air (OTA) updates system: in stead of connecting the clock directly to my PC using a usb cable, I could install the clock where it belongs, above the bed, and send all code updates over WIFI directly to the clock. How cool is that?! 
