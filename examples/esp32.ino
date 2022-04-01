
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

    Serial.println(String(t.day) + "." + String(t.month) + "." + String(t.year) + "  " + String(t.hour) + " " + String(t.minute) + " " + String(t.second));
  }
  // Note:
  // You should not query the NTP server every second in production
  // Get the time once, set a RTC with it or interpolate the time with millis() or something
  delay(1000);
}
