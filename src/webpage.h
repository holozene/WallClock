#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

// config vars
extern short wakeHour;
// todo: make this a char or short so 0 is still nothing, but any other value is a different type of event so it can be represented with a color
extern bool schedule[64];

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Wall Clock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    input {width: 50px}
    p {font-size: 3.0rem;}
    body {max-width: 1280px; margin: 0px auto; padding-bottom: 25px;}
    .time {position: relative; display: inline-block; width: 79px; height: 20px; text-align: left; text-indent: 3px; border-right: solid black 1px;}
    .switch {position: relative; display: inline-block; width: 20px; height: 20px} 
    .switch input {display: none}
    .slider {background-color: #ccc; position: absolute; top: 0; left: 0; right: 0; bottom: 0; border-radius: 6px}
    input:checked+.slider {background-color: #b30000}
  </style>
</head>
<body>
  <div style="height: 20px;"></div>
  <h>Wake Hour: </h>
  %HOURPLACEHOLDER%
  <h1>Schedule For Today:</h1>
  %BUTTONPLACEHOLDER%
<script>
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/change?output="+element.id+"&state="+(+element.checked), true);
  xhr.send();
}
function wakeHour(element) {
  window.location.reload(true);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/change?output=wakeHour&state="+element.value, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

String toString(int i);

String scheduleState(int output);

String processor(const String &var);

void startServer();

#endif /* WEBPAGE_H */