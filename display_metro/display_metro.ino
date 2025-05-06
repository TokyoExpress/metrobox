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
#define BLUE    0x018F
#define GREEN   0x0280
#define YELLOW  0xCE40
#define WHITE   0xFFFF
#define GREY    0x4A69

#define TRAIN   0xFD20

int header_end_h;
const int line_width = 20;
const int text_size = 4;
const int slots[] = {2, 1, 9, 3};
const int x[] = {text_size*6, text_size*6*4, text_size*6*6, text_size*6*16};
const int y_margin = 25;
const int line_height = 9;
const int y_start = tft.height()-y_margin;
const int y[] = {y_start-(text_size*line_height*3), y_start-(text_size*line_height*2), y_start-(text_size*line_height)};
const int header_start = y_start-(text_size*line_height*4);

void awaitConnection() {
  showHeader("Metrobox,WH,WH,WH,");
  bool connected = false;

  while (!connected) {
    String payload = receiveMessage();
    Serial.println(payload);
    if (payload.substring(0, 11) == "[CONNECTED]") {
      String ip = payload.substring(12);
      String resetMessageStr = "Reset: http://" + ip + "/reset";
      connected = true;
      showmsgXY(20, header_start, 2, NULL, "Connection established!           ");
      showmsgXY(20, header_start + 30, 2, NULL, "Starting in 10 seconds.           ");
      showmsgXY(20, header_start + 60, 2, NULL, resetMessageStr.c_str());
    }
    else if (payload == "[AP]\r") {
      showmsgXY(20, header_start + 30, 2, NULL, "Connect to \"Metrobox\" to set up.");
    }
    else if (payload == "[ERROR]\r") {
      showmsgXY(20, header_start + 30, 2, NULL, "Error connecting to WiFi.         ");
    }
  }

  delay(10000);
  tft.fillScreen(BLACK);
  Serial.println("[READY]");
}

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
  tft.setTextWrap(false);
  calculateHeaderEndHeight();

  showmsgXY(20, header_start, 2, NULL, "Connecting to WiFi...");
  awaitConnection();
}

void loop(void) 
{
    String payload = receiveMessage();
    if (payload == "[ERROR]\r") {
      showStatus(true);
      // showTrains(",,,,,,,,,,,,");
    }
    else if (payload[0] == 'S') {
      showHeader(payload.substring(2));
    }
    else if (payload[0] == 'T') {
      showStatus(false);
      showTrains(payload.substring(2));
    }
    else if (payload == "[RESET]\r") {
      tft.fillScreen(BLACK);
      showmsgXY(20, header_start, 2, NULL, "(Reset) Connecting to WiFi...");
      awaitConnection();
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
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(sz);
    tft.print(msg);
}

uint16_t getColorFromLine(String line)
{
  if (line == "WH") return WHITE;
  if (line == "RD") return RED;
  if (line == "OR") return ORANGE;
  if (line == "BL") return BLUE;
  if (line == "GR") return GREEN;
  if (line == "YL") return YELLOW;
  if (line == "SV") return GREY;
}

String getFirstTerm(String s)
{
  int slashIndex = s.indexOf('/');
  int dashIndex = s.indexOf('-');
  if (slashIndex == -1) slashIndex = s.length();
  if (dashIndex == -1) dashIndex = s.length();

  int splitIndex = min(slashIndex, dashIndex);
  if (splitIndex != s.length()) {
    return s.substring(0, splitIndex);
  }
  return s;
}

String fitStationName(String station_name, int16_t start_x)
{
  int16_t  x1, y1;
  uint16_t w, h;
  int rightEdge = tft.width()-header_end_h;
  tft.getTextBounds(station_name, start_x, header_end_h-30, &x1, &y1, &w, &h);
  if (x1 + w <= rightEdge) return station_name;
  station_name = getFirstTerm(station_name);
  tft.getTextBounds(station_name, start_x, header_end_h-30, &x1, &y1, &w, &h);
  if (x1 + w <= rightEdge) return station_name;
  String truncatedName = "";
  String word = " ";
  int index = 0;
  while (index < station_name.length()) {
    char c = station_name[index];
    if (c == ' ' || index == station_name.length() - 1) {
      tft.getTextBounds(truncatedName + word, start_x, header_end_h-30, &x1, &y1, &w, &h);
      if (x1 + w > rightEdge) {
        break;
      }
      truncatedName += word;
      word = " ";
    }
    else {
      word += c;
    }
    index++;
  }
  return truncatedName.substring(1);
}

void calculateHeaderEndHeight()
{
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold18pt7b);
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds("a", 0, 0, &x1, &y1, &w, &h);
  header_end_h = h+70;
}

void showHeader(String payload)
{
  tft.fillRect(0, 0, tft.width(), header_end_h, BLACK);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold18pt7b);

  String station_name;
  int idx = 0;
  String word = "";

  Serial.print(payload);
  for (int i = 0; i < payload.length(); i++) {
    if (payload[i] == ',') {
      if (idx == 0) {
        station_name = word;
      } else {
        tft.fillRect(line_width*(idx-1), 0, line_width, header_end_h+20, getColorFromLine(word));
      }
      word = "";
      idx += 1;
    } else {
      word += payload[i];
    }
  }

  tft.setCursor(line_width*idx, header_end_h-35);
  station_name = fitStationName(station_name, line_width*idx);
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
  tft.setCursor(x[0], header_start);
  tft.print("LN CAR DEST    MIN");

  tft.setTextColor(TRAIN, BLACK);

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
