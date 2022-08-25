#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <ezTime.h>

#include <Adafruit_NeoPixel.h>

// x must be less than 64
// y must be less than 8

// c 0 1  2  3  4  5  6

//   0 15 16 31 32 47 48
//   1 14 17 30 33 46 49
//   2 13 18 29 34 45 50
//   3 12 19 28 35 44 51
//   4 11 20 27 36 43 52
//   5 10 21 26 37 42 53
//   6 9  22 25 38 41 54
//   7 8  23 24 39 40 55

AsyncWebServer server(80);

Timezone Pacific;

Adafruit_NeoPixel strip(512, 13, NEO_GRB + NEO_KHZ800);

// webpage vars
String text = "I'm an esp32dev running PlatformIO, AsyncTCP, ESPAsyncWebServer, and AsyncElegantOTA on port 80, firmware raws can be uploaded to me over the air\n"
              "I have an Adafruit NeoPixel strip (GRB 800KHZ) connected to pin 22 which is 8 pixels tall and 64 pixels across, 512 pixels total, the strip snakes from top to bottom on even columns then bottom to top on odd columns";

// wifi vars
const char *ssid = "Dibuni";
const char *password = "Rainhawk";

// clock vars
short wakeHour = 11;
short stripClockOffset = 24 - wakeHour;

const uint32_t w = strip.Color(0, 1, 0);

// pixel vars
const bool nums[10][15] = {
    {1, 1, 1,
     1, 0, 1,
     1, 0, 1,
     1, 0, 1,
     1, 1, 1},
    {0, 1, 0,
     1, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0},
    {1, 1, 1,
     0, 0, 1,
     1, 1, 1,
     1, 0, 0,
     1, 1, 1},
    {1, 1, 1,
     0, 0, 1,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1},
    {1, 0, 1,
     1, 0, 1,
     1, 1, 1,
     0, 0, 1,
     0, 0, 1},
    {1, 1, 1,
     1, 0, 0,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1},
    {1, 1, 1,
     1, 0, 0,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1},
    {1, 1, 1,
     0, 0, 1,
     0, 0, 1,
     0, 0, 1,
     0, 0, 1},
    {1, 1, 1,
     1, 0, 1,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1},
    {1, 1, 1,
     1, 0, 1,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1}};

const bool days[8][30] = {
    {1, 1, 1, 0, 0, 0, // Er (error)
     1, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1,
     1, 0, 0, 1, 0, 0,
     1, 1, 1, 1, 0, 0},
    {1, 1, 1, 0, 0, 0, // Su
     1, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 0, 1,
     0, 0, 1, 1, 0, 1,
     1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, // M
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0},
    {1, 1, 1, 0, 0, 0, // Tu
     0, 1, 0, 0, 0, 0,
     0, 1, 0, 1, 0, 1,
     0, 1, 0, 1, 0, 1,
     0, 1, 0, 1, 1, 1},
    {1, 0, 1, 0, 1, 0, // W
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 0, 0, // Th
     0, 1, 0, 1, 0, 0,
     0, 1, 0, 1, 1, 1,
     0, 1, 0, 1, 0, 1,
     0, 1, 0, 1, 0, 1},
    {1, 1, 1, 0, 0, 0, // Fr
     1, 0, 0, 0, 0, 0,
     1, 1, 0, 1, 1, 1,
     1, 0, 0, 1, 0, 0,
     1, 0, 0, 1, 0, 0},
    {1, 1, 1, 0, 0, 0, // Sa
     1, 0, 0, 0, 0, 1,
     1, 1, 1, 1, 1, 1,
     0, 0, 1, 1, 0, 1,
     1, 1, 1, 1, 1, 1}};

const bool pwnme[161] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
    1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
    1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const bool pwn[91] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint32_t minuteColors[15] = {
    strip.Color(255, 12, 0),   // 0 red
    strip.Color(237, 36, 0),   // 1
    strip.Color(219, 73, 0),   // 2
    strip.Color(200, 109, 0),  // 3
    strip.Color(164, 146, 0),  // 4
    strip.Color(109, 182, 0),  // 5
    strip.Color(55, 200, 0),   // 6 green
    strip.Color(36, 219, 0),   // 7
    strip.Color(18, 255, 36),  // 8
    strip.Color(36, 219, 73),  // 9
    strip.Color(55, 164, 128), // 10
    strip.Color(73, 109, 200), // 11
    strip.Color(91, 73, 255),  // 12
    strip.Color(109, 36, 191), // 13
    strip.Color(128, 12, 128)  // 14
};

int xyToIndex(short x, short y)
{
  return x * 8 + ((x % 2 == 1) ? 7 - y : y);
}

void showDigit(short digit, int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 5; y++)
    for (int x = 0; x < 3; x++)
    {
      if (nums[digit][x + y * 3])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), strip.Color(0, 0, 0));
    }
  strip.show();
}

void showDay(int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 5; y++)
    for (int x = 0; x < 6; x++)
    {
      if (days[Pacific.weekday()][x + y * 6])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), strip.Color(0, 0, 0));
    }
  strip.show();
}

void showPwnMe(int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 7; y++)
    for (int x = 0; x < 23; x++)
    {
      if (pwnme[x + y * 23])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), strip.Color(0, 0, 0));
    }
  strip.show();
}

void showPwn(int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 7; y++)
    for (int x = 0; x < 13; x++)
    {
      if (pwn[x + y * 13])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), strip.Color(0, 0, 0));
    }
  strip.show();
}

void leftClock(uint32_t clockColor)
{
  if (Pacific.hour() >= 10)
    showDigit(Pacific.hour() / 10, 0, 0, clockColor);
  else
    for (int y = 0; y < 5; y++)
      for (int x = 0; x < 3; x++)
        strip.setPixelColor(xyToIndex(x, y), strip.Color(0, 0, 0));
  showDigit(Pacific.hour() % 10, 4, 0, clockColor);
  strip.setPixelColor(65, clockColor);
  strip.setPixelColor(67, clockColor);
  showDigit(Pacific.minute() / 10, 10, 0, clockColor);
  showDigit(Pacific.minute() % 10, 14, 0, clockColor);
}

// void bottomBar(int startX, int endX, uint32_t barColor, uint32_t endColor)
// {
//   for (int x = startX; x < endX - 1; x++)
//   {
//     strip.setPixelColor(xyToIndex(x, 7), barColor);
//   }
//   strip.setPixelColor(xyToIndex(endX, 7), endColor);
// }

// int timeToX()
// {
//   return (Pacific.hour() + stripClockOffset) % 24 * 4 + Pacific.minute() / 15;
// }

// void chaserClock()
// {
//   strip.setPixelColor(xyToIndex(max(timeToX() - 1, 0), 7), strip.Color(0, 0, 0));
//   strip.setPixelColor(xyToIndex(timeToX(), 7), minuteColors[Pacific.minute() % 15]);
// }

void setup(void)
{
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(10);

  Serial.begin(115200);
  // WiFi.mode(WIFI_MODE_STA);
  // WiFi.setAutoConnect(true);
  // WiFi.scanNetworks();
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", text); });

  AsyncElegantOTA.begin(&server); // Start AsyncElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  waitForSync();
  Pacific.setLocation("America/Los_Angeles");
  Pacific.setDefault();
  Serial.print("Time and Date: ");
  Serial.println(Pacific.dateTime("l, d-M-y H:i:s.v T"));

  strip.setPixelColor(0, w);
  strip.show();
}

IPAddress store;
// use after setup only
void showIP(int startX)
{
  if (WiFi.localIP() == store)
    return;

  store = WiFi.localIP();
  int x = startX;
  for (char c : WiFi.localIP().toString())
  {
    if (c == '.')
    {
      strip.setPixelColor(xyToIndex(x + 2, 5), w);
      x += 3;
    }
    else
    {
      showDigit(c - '0', x, 0, w);
      x += 5;
    }
  }
  strip.show();
}

void loop(void)
{
  strip.setPixelColor(0, w);
  strip.show();

  // showPwn(0, 1, w);

  showIP(0);

  delay(1000);
}