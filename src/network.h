#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <time.h>

void connectWiFiWithTimeout(unsigned long timeoutMs = 15000);
void checkTimeSync();

#endif