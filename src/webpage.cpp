#include "webpage.h"
#include "secrets.h"

AsyncWebServer server(80);

// config vars
short wakeHour = 11;
// todo: make this a char or short so 0 is still nothing, but any other value is a different type of event so it can be represented with a color
bool schedule[64];

// webpage vars
const char *PARAM_INPUT_1 = "output";
const char *PARAM_INPUT_2 = "state";

String toString(int i)
{
  return String(i);
}

String scheduleState(int output)
{
  if (schedule[output] == 0)
    return "";
  else
    return "checked";
}

// Replaces placeholder with button section
String processor(const String &var)
{

  if (var == "HOURPLACEHOLDER")
  {
    return "<input id=\"wakeHour\" class=\"\" onchange=\"toggleCheckbox(this)\" type=\"number\" max=\"24\" min=\"1\" value=\"" + toString(wakeHour) + "\">";
  }
  // Serial.println(var);
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    for (int i = 0; i < 16; i++)
      buttons += "<div class=\"time\">" + toString((i + wakeHour) % 12) + ":00</div>";
    buttons += "<div></div>";
    for (int i = 0; i < 64; i++)
      buttons += "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + toString(i) + "\" " + scheduleState(i) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

void startServer()
{
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
}
