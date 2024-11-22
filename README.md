# NTP Client for ESP32 and ESP8266

The ESP32 and ESP8266 has WiFi capabilities and implementations of TCP and UDP, which allows to query NTP servers easily to get the exact time for IoT projects.

```cpp

#include <WiFi.h>
#include <ntp.h>

// Enter your WiFi settings
const char *ssid = "...";
const char *passwd = "...";

WiFiUDP udp;
NTP ntp(udp);

bool connected = false;

void WiFiEvent(system_event_id_t event){

  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      ntp.begin();
      connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      connected = false;
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, passwd);

  ntp.setTimeOffset(2 * 3600); // Set time offset to GMT+2:00
}

void loop() {

  if (connected) {
    ntp.update();

    ntp_datetime_t t = ntp.getDateTime();

    Serial.println(String(t.day) + "." +
     String(t.month) + "." +
     String(t.year) + "  " +
     String(t.hour) + " " +
     String(t.minute) + " " +
     String(t.second));
  }
  // Note:
  // You should not query the NTP server every second in production
  // Get the time once, set a RTC with it or interpolate the time with millis() or something
  delay(1000);
}
```

## Constructor

The constructor requires a UDP object to be passed, and optionally the address of the NTP server to be queried (`pool.ntp.org` is the default, which should select the best time server close to you)

```cpp
NTP(UDP& udp);
NTP(UDP& udp, const char *address);
```

## Methods

When a WiFi connection is established, `begin()` creates a UDP socket

```cpp
void begin(uint32_t port = NTP_DEFAULT_LOCAL_PORT)
```

Whenever the server should be queried to update the local timestamp, call `update()` (Note: You should not call this method all to often to not flood NTP servers! Use the current time to set a RTC real time clock for example)

```cpp
bool update()
```

To get a human readable datetime, call `getDateTime()`, which returns an object with `{day, month, year, hour, minute, second}` attributes:

```cpp
ntp_datetime_t getDateTime()
```

To get the current time as Unix Timestamp (seconds since 01.01.1970 00:00:00), call

```cpp
uint32_t getTimestamp()
```

To fix `getDateTime()` for different time-zones, set the number of seconds your region deviates from UTC (for GMT+2, pass `2 * 3600`).

```cpp
void setTimeOffset(int16_t timeOffset)
```

To close the socket, call `end()`:

```cpp
void end()
```



Copyright and licensing
===
Copyright (c) 2024, [Robert Eisele](https://raw.org/)
Licensed under the MIT license.
