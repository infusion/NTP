
#include <Arduino.h>
#include <Udp.h>

#define NTP_DEFAULT_LOCAL_PORT 9876
#define NTP_PACKET_SIZE 48
#define NTP_TIMESTAMP_DIFFERENCE 2208988800 // Difference between 01.01.1900 and 01.01.1970

typedef struct {
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} ntp_datetime_t;

class NTP {

private:
  UDP *_sock;

  int32_t _timeOffset = 0;
  uint32_t _timestamp = 0; // Seconds since 1970-01-01 00:00:00

  const char *_address = "pool.ntp.org";
  char unsigned _buffer[NTP_PACKET_SIZE];

public:
  NTP(UDP& udp);
  NTP(UDP& udp, const char *address);

  void begin(uint32_t port = NTP_DEFAULT_LOCAL_PORT);

  bool update();
  
  void setTimeOffset(int16_t timeOffset);
  uint32_t getTimestamp();

  ntp_datetime_t getDateTime();

  void end();
};
