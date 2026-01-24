#include "network.h"

const char* _timeZone = "CET-1CEST,M3.5.0/2,M10.5.0/3"; // check your timezone
const char* _timeServer = "pool.ntp.org";

static int _lastSyncHour = -1;

// Replace with your WiFi credentials
const char* _ssid = "<yourSSID>";
const char* _password = "<yourPassword>";

void connectWiFiWithTimeout(unsigned long timeoutMs) {
    WiFi.begin(_ssid, _password);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeoutMs) {
        delay(500);
    }
}

void checkTimeSync()
{
  if (_lastSyncHour == -1)
    configTzTime(_timeZone, _timeServer);

  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  if ((timeinfo.tm_hour == 0 || timeinfo.tm_hour == 12) && timeinfo.tm_min == 0) {
      int currentSyncHour = timeinfo.tm_hour;

      if (_lastSyncHour == -1) // initial
        _lastSyncHour = currentSyncHour;
      else if (currentSyncHour != _lastSyncHour) 
      {
        configTzTime(_timeZone, _timeServer);
        _lastSyncHour = currentSyncHour;
      }
  }
}