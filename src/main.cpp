#include "secrets.h"
#include "webpage.h"
#include "pixels.h"
#include "clock.h"
#include <AsyncElegantOTA.h>

void setup(void)
{
  // Start Strip
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(50);
  strip.setPixelColor(8, minuteColors[0]);

  // Start Serial
  Serial.begin(115200);

  // Connect AsyncElegantOTA to Server
  AsyncElegantOTA.begin(&server);
  // Start Server
  startServer();

  // Setup ezTime
  waitForSync();
  Pacific.setLocation("America/Los_Angeles");
  Pacific.setDefault();
  Serial.print("Time and Date: ");
  Serial.println(Pacific.dateTime("l, d-M-y H:i:s.v T"));
}

void loop(void)
{
  // while awake, update the clock display every 1/3 of a second
  // if (Pacific.hour() > wakeHour && Pacific.hour() < (wakeHour - 9) % 24)
  {
    digitClock();
    pixelClock();
    strip.show();
    delay(333);
  }
  // from 8 hours until wakeHour to wakeHour, turn off the clock
  // else
  // {
  //   strip.clear();
  //   strip.show();
  //   delay(10000);
  // }
}
