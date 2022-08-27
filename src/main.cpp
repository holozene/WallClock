#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <ezTime.h>

#include <Adafruit_NeoPixel.h>

AsyncWebServer server(80);

Timezone Pacific;

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
Adafruit_NeoPixel strip(512, 13, NEO_GRB + NEO_KHZ800);

// clock vars
short wakeHour = 11;
// todo: make this a char or short so 0 is still nothing, but any other value is a different type of event so it can be represented with a color
bool schedule[64];

// wifi vars
const char *ssid = "Dibuni";
const char *password = "Rainhawk";

// webpage vars
const char *PARAM_INPUT_1 = "output";
const char *PARAM_INPUT_2 = "state";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Wall Clock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 1280px; margin:0px auto; padding-bottom: 25px;}
    .time {position: relative; display: inline-block; width: 79px; height: 20px; text-align: left; text-indent: 3px; border-right: solid black 1px;}
    .switch {position: relative; display: inline-block; width: 20px; height: 20px} 
    .switch input {display: none}
    .slider {background-color: #ccc; position: absolute; top: 0; left: 0; right: 0; bottom: 0; border-radius: 6px}
    input:checked+.slider {background-color: #b30000}
  </style>
</head>
<body>
  <h2>Enter Schedule For Today:</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

String scheduleState(int output)
{
  if (schedule[output] == 0)
    return "";
  else
    return "checked";
}

String toString(int i)
{
  return String(i);
}

// Replaces placeholder with button section
String processor(const String &var)
{
  // Serial.println(var);
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    for (int i = 0; i < 16; i++)
    {
      buttons += "<div class=\"time\">" + toString((i + wakeHour) % 12) + ":00</div>";
    }
    buttons += "<div></div>";
    for (int i = 0; i < 64; i++)
    {
      buttons += "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + toString(i) + "\" " + scheduleState(i) + "><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  return String();
}

// pixel vars
const bool nums[10][15] = {
    {1, 1, 1, // 0
     1, 0, 1,
     1, 0, 1,
     1, 0, 1,
     1, 1, 1},
    {0, 1, 0, // 1
     1, 1, 0,
     0, 1, 0,
     0, 1, 0,
     1, 1, 1},
    {1, 1, 1, // 2
     0, 0, 1,
     1, 1, 1,
     1, 0, 0,
     1, 1, 1},
    {1, 1, 1, // 3
     0, 0, 1,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1},
    {1, 0, 1, // 4
     1, 0, 1,
     1, 1, 1,
     0, 0, 1,
     0, 0, 1},
    {1, 1, 1, // 5
     1, 0, 0,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1},
    {1, 1, 1, // 6
     1, 0, 0,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1},
    {1, 1, 1, // 7
     0, 0, 1,
     0, 0, 1,
     0, 0, 1,
     0, 0, 1},
    {1, 1, 1, // 8
     1, 0, 1,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1},
    {1, 1, 1, // 9
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

// color vars
const uint32_t neutral = strip.Color(6, 6, 6);

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
        strip.setPixelColor(xyToIndex(startX + x, startY + y), 0);
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
        strip.setPixelColor(xyToIndex(startX + x, startY + y), 0);
    }
}

void showSchedule()
{
  for (int i = 0; i < 64; i++)
  {
    switch (schedule[i])
    {
    case 1:
      strip.setPixelColor(xyToIndex(i, 6), neutral);
      break;
    default:
      strip.setPixelColor(xyToIndex(i, 6), 0);
      break;
    }
  }
}

IPAddress store;
// use only after WiFi has been setup
void IP(int startX)
{
  if (WiFi.localIP() == store)
    return;

  store = WiFi.localIP();
  int x = startX;
  for (char c : WiFi.localIP().toString())
  {
    if (c == '.')
    {
      strip.setPixelColor(xyToIndex(x, 5), minuteColors[14]);
      x += 2;
    }
    else
    {
      showDigit(c - '0', x, 1, minuteColors[14]);
      x += 4;
    }
  }
}

void barClock()
{
  showSchedule();
  int endX = (Pacific.hour() + 24 - wakeHour) % 24 * 4 + Pacific.minute() / 15;
  for (int x = 0; x < endX - 1; x++)
    strip.setPixelColor(xyToIndex(x, 7), neutral);
  strip.setPixelColor(xyToIndex(endX, 7), minuteColors[Pacific.minute() % 15]);
}

void pixelClock()
{
  showSchedule();
  int x = (Pacific.hour() + 24 - wakeHour) % 24 * 4 + Pacific.minute() / 15;
  if (x > 0) strip.setPixelColor(xyToIndex(x - 1, 7), 0);
  strip.setPixelColor(xyToIndex(x, 7), minuteColors[Pacific.minute() % 15]);
}

void digitClock()
{
  if (Pacific.hour() >= 10)
    showDigit(Pacific.hour() / 10, 0, 0, neutral);
  else
    for (int y = 0; y < 5; y++)
      for (int x = 0; x < 3; x++)
        strip.setPixelColor(xyToIndex(x, y), 0);
  showDigit(Pacific.hour() % 10, 4, 0, neutral);
  strip.setPixelColor(65, neutral);
  strip.setPixelColor(67, neutral);
  showDigit(Pacific.minute() / 10, 10, 0, neutral);
  showDigit(Pacific.minute() % 10, 14, 0, neutral);
  showDay(20, 0, neutral);
}

void setup(void)
{
  // Start Strip
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(50);

  // Start Serial
  Serial.begin(115200);

  // Start WiFi
  Serial.println("Starting WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start AsyncElegantOTA
  AsyncElegantOTA.begin(&server);

  // Setup Server
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      schedule[inputMessage1.toInt()] = inputMessage2.toInt() == 0 ? false : true;
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("id: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK"); });

  // Start Server
  server.begin();
  Serial.println("HTTP server started");

  // Setup ezTime
  waitForSync();
  Pacific.setLocation("America/Los_Angeles");
  Pacific.setDefault();
  Serial.print("Time and Date: ");
  Serial.println(Pacific.dateTime("l, d-M-y H:i:s.v T"));
}

void loop(void)
{
  // from 9 hours until wakeHour to wakeHour, turn off the clock
  if (Pacific.hour() < wakeHour && Pacific.hour() > (wakeHour - 9) % 24)
  {
    strip.clear();
    strip.show();
    delay(10000);
  } 
  // while awake, update the clock display every 1/3 of a second
  else
  {
    digitClock();
    pixelClock();
    strip.show();
    delay(333);
  }
}
