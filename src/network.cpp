#include "network.h"
#include "credentials.h"

const char* _timeZone = "CET-1CEST,M3.5.0/2,M10.5.0/3"; // check your timezone
const char* _timeServer = "pool.ntp.org";

static int _lastSyncHour = -1;
static int _wifiStatus = WL_IDLE_STATUS;
static bool _currentTimeIsValid = false;

// Non-blocking state tracking
static unsigned long _lastWifiAttemptTime = 0;
static unsigned long _lastTimeUpdateAttemptTime = 0;
static unsigned long _timeUpdateStartTime = 0;
static bool _isTimeUpdateInProgress = false;

void initializeWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // from credentials.h
}

void connectWiFi() {
  if (_wifiStatus == WL_CONNECTED) 
    return;

  // Start connection attempt if not already connecting
  if (WiFi.status() != WL_CONNECTED) 
  {
    if (millis() - _lastWifiAttemptTime > 5000) 
    {
      WiFi.reconnect();
      _lastWifiAttemptTime = millis();
    }
  }
  
  _wifiStatus = WiFi.status();
}

bool TryUpdateTime(unsigned long timeoutMs)
{
  // If not currently trying or timeout elapsed, check current time
  if (!_isTimeUpdateInProgress) {
    struct tm timeinfo = {0};
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    
    // If time is already valid, no need to update
    if (timeinfo.tm_year > (2020 - 1900))
      return true;
    
    // Start update attempt
    _isTimeUpdateInProgress = true;
    _timeUpdateStartTime = millis();
    configTzTime(_timeZone, _timeServer, nullptr, nullptr);
    return false;
  }
  
  // Check if timeout elapsed
  if (millis() - _timeUpdateStartTime >= timeoutMs) {
    _isTimeUpdateInProgress = false;
    
    struct tm timeinfo = {0};
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_year > (2020 - 1900);
  }
  
  // Still waiting for NTP sync
  return false;
}

tm GetCurrentTime()
{
  struct tm timeinfo = {0};
  time_t now = time(nullptr);
  localtime_r(&now, &timeinfo);
  return timeinfo;
}

ConnectionState checkTimeSync()
{
  // Initial time sync on first call
  if (_lastSyncHour == -1 || !_currentTimeIsValid)
  {
    _currentTimeIsValid = TryUpdateTime(10000);
  }

  tm timeinfo = GetCurrentTime();

  // Check if it's time for periodic sync (at 00:00 and 12:00)
  bool shouldUpdateTime = (timeinfo.tm_hour == 0 || timeinfo.tm_hour == 12) && timeinfo.tm_min == 0;
  
  if (shouldUpdateTime) 
  {
    int currentSyncHour = timeinfo.tm_hour;

    if (_lastSyncHour == -1) 
    {
      // First sync at startup
      _lastSyncHour = currentSyncHour;
      _wifiStatus = WiFi.status();
      connectWiFi();
    }
    else if (currentSyncHour != _lastSyncHour) 
    {
      // Periodic sync (non-blocking)
      _wifiStatus = WiFi.status();
      connectWiFi();
      _currentTimeIsValid = TryUpdateTime(10000);
      _lastSyncHour = currentSyncHour;
    }
  }
  else
  {
    // Update WiFi status even when not syncing time
    _wifiStatus = WiFi.status();
  }

  if (_currentTimeIsValid && _wifiStatus == WL_CONNECTED)
    return TIME_SYNCED;
  else if (_wifiStatus == WL_CONNECTED)
    return WIFI_CONNECTED;
  else
    return NONE;
}