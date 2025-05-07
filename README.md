# Metrobox

Source code and build for a miniature WMATA Metrorail train arrivals board.

![494856235_698844399746148_1401936910194309229_n 1](https://github.com/user-attachments/assets/a64b9ca4-966c-4bbc-8b6c-76393950ed5d)

---

## Configuring a Ready-to-Use Metrobox

#### Obtain a WMATA API key

1. Sign up for an account at https://developer.wmata.com/signup/ and verify your email.
Once you have an account, go to Products -> Default Tier and subscribe a new product in the section shown below (the name does not matter).

<img src="https://github.com/user-attachments/assets/0f91c9cd-96ff-4a55-b4f0-fac1dbf8b521" width="1000">

2. You should be taken to the Profile page, where you can access your new API key. Keep this page open.

<img src="https://github.com/user-attachments/assets/53431f79-78f3-45a3-9787-44904617d616" width="800">

#### Find Your Station Code

Find your station name's corresponding 3-digit code in [wmata_station_codes.csv](wmata_station_codes.csv). Some stations have multiple platforms, so be sure you choose the correct platform with the lines you typically take. Keep this page open as well.

#### Configuring the Metrobox

1. Plug in the box. You should see a message saying `Connect to "Metrobox" to set up`. If you don't see this and instead it shows `Connection established` and begins to show times for an unwanted station, follow the instructions in [Resetting](#Resetting), then come back to this section.

You can do the following on either a phone or a computer, but it will be easier on the same computer that you performed the above steps on since you will be able to copy-paste the API key.

2. Go to where you would normally connect to a WiFi network and connect to the Access Point "Metrobox" with the password "metrobox". You will lose Internet access on this device until setup is complete, so be sure to keep the API key and station code on hand.

3. A captive portal should open. Click on "Configure WiFi" and fill in your WiFi settings, the primary API key, and the station code (notional example below).

<img src="https://github.com/user-attachments/assets/1a730281-59ac-4a62-aeeb-b104c2cb84c9" width="400">

Once you hit "Save", the box should display `Connection established` and begin to display arrival times. You can then reconnect back to the Internet on your device.

The box is now successfully configured and will automatically function with the same settings if unplugged and restarted.

#### Resetting

If your box is already auto-connecting to your WiFi, you will have to reset the box in order to change stations or API keys after configuration. Upon connection, you should see a message saying `Visit http://<ip_address>/reset to reset`. Do this on any device that is connected to the same WiFi network and you should receive a message saying `Resetting configuration`.

The box should no longer auto-connect and you can follow the instructions in [Configuring the Metrobox](#Configuring-the-Metrobox) to set it up again.

---

## Parts and Build

To make this yourself, you will need the following (links may need updating):
- UNO R3 + WIFI ESP8266, [example "UNO R3 Black"](https://www.aliexpress.us/item/3256806816026732.html)
- 3.5" TFT LCD Shield for Arduino Uno (no touchscreen), [example "Without Touch"](https://www.aliexpress.us/item/3256806911484514.html)
- Micro-USB cable
- 3D printer or access to 3D printing services

![components](https://github.com/user-attachments/assets/339163df-8bea-435f-b5e7-586ea0b838e8)

### Uploading Software

1. Install the [Arduino IDE](https://www.arduino.cc/en/software/).
2. [Install the ESP8266 platform into Arduino IDE Boards Manager](https://github.com/esp8266/Arduino?tab=readme-ov-file#installing-with-boards-manager)
3. [Add libraries](https://support.arduino.cc/hc/en-us/articles/5145457742236-Add-libraries-to-Arduino-IDE) WiFiManager, ArduinoJson, MCUFRIEND_kbv, and Adafruit GFX Library.
4. You are now ready to begin [uploading sketches](https://support.arduino.cc/hc/en-us/articles/4733418441116-Upload-a-sketch-in-Arduino-IDE) to the board. To start, ensure that the UNO R3 is recognized by Arduino IDE when plugged into the computer. If not, you may need to install the [CH340G drivers](https://www.wch.cn/downloads/CH341SER_ZIP.html).

The board controls communications using a DIP switch as pictured below. You will need to use a small needle or pin to set the switches appropriately for each of the next steps.

![dip switch](https://github.com/user-attachments/assets/9ed1c4f8-131f-44bb-a8a0-c41e6dd24821)
![setups](https://github.com/user-attachments/assets/d50c6346-fb41-4efa-9b78-ec938e37b345)

a. With only switches 5, 6, and 7 in the "on" position, upload `get_trains.ino` with board type set to "Generic ESP8266 Module".

b. With only switches 3 and 4 in the "on" position, upload `display_metro.ino` with board type set to "Arduino UNO".

c. Set only switches 1 and 2 to the "on" position.

If all uploads succeeded, you are ready to move on to the assembly portion.

### Printing and Assembly

3D print files (TBD)

1. Place the UNO R3 board inside the case so that the ports line up with the holes.

![494358693_949836297227734_8370262248152707982_n 1](https://github.com/user-attachments/assets/b35d52f9-d78a-41cd-b75d-a1778da796ea)

2. Line up the TFT shield pins with the UNO R3 and connect the two components.

![494812978_1202707301205929_8375195684690110429_n 1](https://github.com/user-attachments/assets/fc94184a-7a6b-45d9-ac89-981b0ad9e400)

3. Place and snap the cover on.

![494860804_1945757139497187_8018358129188421132_n 1](https://github.com/user-attachments/assets/ea6cfae6-b9d7-4bf4-8b8e-7586797b5957)

The Metrobox is now complete and ready for [Configuring a Ready-to-Use Metrobox](#Configuring-a-Ready-to-Use-Metrobox).
