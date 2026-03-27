#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <time.h>

#include "config.h"

void initializeWifi();
void connectWiFi();
ConnectionState checkTimeSync();

#endif