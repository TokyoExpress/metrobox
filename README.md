# Metrobox

Source code and build for a miniature WMATA Metrorail train arrivals board.

## Configuring a Ready-to-Use Metrobox

#### Obtain a WMATA API key

Sign up for an account at https://developer.wmata.com/signup/ and verify your email.
Once you have an account, go to Products -> Default Tier and subscribe a new product.
![subscribe](https://github.com/user-attachments/assets/db1c2b66-29bb-4d52-bea0-09c1ff52db15)
You should be taken to the Profile page, where you can access your new API key.
![api](https://github.com/user-attachments/assets/c0c7e95c-8c09-40ed-845e-4bf895470f79)
Keep this page open.

#### Find Your Station Code

Find your station name's corresponding 3-digit code in [wmata_station_codes.csv](wmata_station_codes.csv). Some stations have multiple platforms, so be sure you choose the correct platform with the lines you typically take. Keep this page open as well.

#### Configuring the Metrobox

Power on the box. You should see a message saying `Connect to "Metrobox" to set up`. If you don't see this and instead it shows `Connection established` and begins to show times for an unwanted station, follow the instructions in [Resetting](Resetting), then come back to this section.

#### Resetting

If your box is already auto-connecting to your WiFi, you will have to reset the box to change stations or API keys after configuration. Upon connection, you should see a message saying `Visit http://<ip_address>/reset to reset`. Do this on any device that is connected to the same WiFi network and you should receive a message saying `Resetting configuration`. The box should no longer auto-connect and you can follow the instructions in [Configuring the Metrobox](Configuring the Metrobox) to set it up again.

## Parts and Build

### Software
