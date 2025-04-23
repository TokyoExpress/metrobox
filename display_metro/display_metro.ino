#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <FreeDefaultFonts.h>

#define BLACK   0x0000
#define RED     0xF800
#define ORANGE  0xFA60
#define BLUE    0x13B8
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x8410

int header_end_h;
const int line_width = 20;

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.println("STARTUP");
    // Serial.print("found ID = 0x");
    // Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    showmsgXY(20, 10, 2, NULL, "Connecting to WiFi...");
    bool connected = false;

    while (!connected) {
      String payload = receiveMessage();
      Serial.println(payload);
      if (payload == "[CONNECTED]\r") {
        connected = true;
        showmsgXY(20, 40, 2, NULL, "Connection established!");
      }
      else if (payload == "[ERROR]\r") {
        showmsgXY(20, 40, 2, NULL, "Error connecting to WiFi - please restart the box.");
      }
    }
    calculateHeaderEndHeight();
    Serial.println("[READY]");
    delay(1000);
    tft.fillScreen(BLACK);
}

const int text_size = 4;
const int slots[] = {2, 1, 9, 3};
const int x[] = {text_size*6, text_size*6*4, text_size*6*6, text_size*6*16};
const int y_margin = 20;
const int line_height = 9;
const int y_start = tft.height()-y_margin;
const int y[] = {y_start-(text_size*line_height*3), y_start-(text_size*line_height*2), y_start-(text_size*line_height)};

void loop(void) 
{
    String payload = receiveMessage();
    if (payload == "[ERROR]\r") {
      showStatus(true);
      // showTrains(",,,,,,,,,,,,");
    }
    else if payload[0] == 'S' {
      showTrains(payload.substring(2));
    }
    else if payload[0] == 'T' {
      showstatus(false);
      showHeader(payload.substring(2));
    }
}

String receiveMessage()
{
  String payload = "";
  bool rcvd = false;
  while (!rcvd) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c != '\n') {
        payload += c;
      } else {
        rcvd = true;
      }
    }
  }
  return payload;
}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(GREEN, BLACK);
    tft.setTextSize(sz);
    tft.print(msg);
}

void calculateHeaderEndHeight()
{
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold18pt7b);
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds(station_name, 0, 0, &x1, &y1, &w, &h);
  header_end_h = h+60;
}

void showHeader(String payload)
{
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold18pt7b);

  String station_name;
  int idx = 0;
  String word = "";

  Serial.print(payload);
  for (int i = 0; i < payload.length(); i++) {
    if (payload[i] == ',') {
      if idx == 0 {
        station_name = word;
      } else {
        tft.fillRect(line_width*(idx-1), 0, line_width, header_end_h, COLOR_MAP[word]);
      }
      word = "";
      idx += 1;
    } else {
      word += payload[i];
    }
  }

  tft.setCursor(line_width*idx, h+30);
  tft.print(station_name);
  // tft.drawFastHLine(0, header_end_h, tft.width(), WHITE);
  // tft.drawFastHLine(0, y_start-(text_size*line_height*4)-y_margin-5, tft.width(), WHITE);
}

void showStatus(bool error)
{
  uint16_t color;
  if (error) {
    color = RED;
  } else {
    color = GREEN;
  }
  tft.fillCircle(tft.width()-(header_end_h/2), header_end_h/2, 5, color);
}

void showTrains(String payload)
{
  tft.setFont(NULL);
  tft.setTextColor(RED);
  tft.setTextSize(text_size);
  tft.setCursor(x[0], y_start-(text_size*line_height*4));
  tft.print("LN CAR DEST    MIN");

  tft.setTextColor(YELLOW, BLACK);

  int idx = 0;
  String word = "";

  Serial.print(payload);
  for (int i = 0; i < payload.length(); i++) {
    if (payload[i] == ',') {
      String spaces = "";
      word = word.substring(0, slots[idx%4]);
      for (int j = 0; j < slots[idx%4] - word.length(); j++) {
        spaces += " ";
      }
      if (idx%4 == 3) {
        word = spaces + word;
      } else {
        word += spaces;
      }
      Serial.print(word);
      tft.setCursor(x[idx%4], y[idx/4]);
      tft.print(word);
      word = "";
      idx += 1;
    } else {
      word += payload[i];
    }
  }
}
