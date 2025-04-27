# Metrobox

Source code and build for a miniature WMATA Metrorail train arrivals board.

## Configuring a Ready-to-Use Metrobox

#### Obtain a WMATA API key

1. Sign up for an account at https://developer.wmata.com/signup/ and verify your email.
Once you have an account, go to Products -> Default Tier and subscribe a new product.

![subscribe](https://github.com/user-attachments/assets/db1c2b66-29bb-4d52-bea0-09c1ff52db15)

2. You should be taken to the Profile page, where you can access your new API key. Keep this page open.

![api](https://github.com/user-attachments/assets/c0c7e95c-8c09-40ed-845e-4bf895470f79)

#### Find Your Station Code

Find your station name's corresponding 3-digit code in [wmata_station_codes.csv](wmata_station_codes.csv). Some stations have multiple platforms, so be sure you choose the correct platform with the lines you typically take. Keep this page open as well.

#### Configuring the Metrobox

1. Power on the box. You should see a message saying `Connect to "Metrobox" to set up`. If you don't see this and instead it shows `Connection established` and begins to show times for an unwanted station, follow the instructions in [Resetting](Resetting), then come back to this section.

You can do the following on either a phone or a computer, but it will be easier on the same computer that you performed the above steps on since you will be able to copy-paste the API key.

2. Go to where you would normally connect to a WiFi network and connect to the Access Point "Metrobox" with the password "metrobox". You will lose Internet access on this device until setup is complete, so be sure to keep the API key and station code on hand.

3. A captive portal should open. Click on "Configure WiFi" and fill in your WiFi settings, the API key, and the station code (notional example below).

![metrobox](https://github.com/user-attachments/assets/1a730281-59ac-4a62-aeeb-b104c2cb84c9)

Once you hit "Save", the box should display `Connection established` and begin to display arrival times. You can then reconnect back to the Internet on your device.

The box is now successfully configured and will automatically function with the same settings if unplugged and restarted.

#### Resetting

If your box is already auto-connecting to your WiFi, you will have to reset the box in order to change stations or API keys after configuration. Upon connection, you should see a message saying `Visit http://<ip_address>/reset to reset`. Do this on any device that is connected to the same WiFi network and you should receive a message saying `Resetting configuration`. The box should no longer auto-connect and you can follow the instructions in [Configuring the Metrobox](Configuring-the-Metrobox) to set it up again.

## Parts and Build (WIP)

To make this yourself, you will need:
- UNO R3 + WIFI ESP8266
- 3.5" TFT LCD Shield for Arduino Uno
- Micro-USB cable
- 3D printer or access to 3D printing services

### Software (WIP)
