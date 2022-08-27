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
  // from 8 hours until wakeHour to wakeHour, turn off the clock
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
