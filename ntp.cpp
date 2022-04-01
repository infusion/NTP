
#include "ntp.h"

NTP::NTP(UDP &sock) {
  this->_sock = &sock;
}

void NTP::begin(uint32_t port) {
  this->_sock->begin(port);
}

void NTP::end() {
  this->_sock->stop();
}

bool NTP::update() {

  while(this->_sock->parsePacket() != 0) {
    this->_sock->flush();
  }

  unsigned char *b = this->_buffer;

  memset(b, 0, NTP_PACKET_SIZE);

  b[0]  = 0xE3; // LI, Version, Mode
  b[1]  = 0x00; // Stratum
  b[2]  = 0x05; // Polling Interval
  b[3]  = 0xEC; // Peer Clock Precision
  b[12] = 0x31;
  b[13] = 0x4E;
  b[14] = 0x31;
  b[15] = 0x34;

  this->_sock->beginPacket(this->_address, 123);
  this->_sock->write(b, NTP_PACKET_SIZE);
  this->_sock->endPacket();

  for (int32_t ret = 0, count = 0; ret == 0; count++) {
    delay(10);
    ret = this->_sock->parsePacket();

    if (count > 100) { // Wait for 1000 ms
      return false;
    }
  }

  this->_sock->read(b, NTP_PACKET_SIZE);

  uint32_t ntpTimestamp = word(b[40], b[41]) << 16 | word(b[42], b[43]);

  // Read NTP time (seconds since Jan 1 1900)
  this->_timestamp = ntpTimestamp - NTP_TIMESTAMP_DIFFERENCE;

  return true;
}

uint32_t NTP::getTimestamp() {
  return this->_timestamp;
}

void NTP::setTimeOffset(int16_t timeOffset) {
  this->_timeOffset = timeOffset;
}

ntp_datetime_t NTP::getDateTime() {

  uint32_t s = this->_timestamp + this->_timeOffset;
  ntp_datetime_t r;

  // https://stackoverflow.com/questions/7136385/calculate-day-number-from-an-unix-timestamp-in-a-math-way
  uint32_t z = s / 86400 + 719468;
  uint32_t era = z / 146097;
  uint32_t doe = (z - era * 146097);
  uint32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  r.year = yoe + era * 400;

  uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  uint32_t mp = (5 * doy + 2) / 153;
  r.day = doy - (153 * mp+2) / 5 + 1;
  r.month = mp + (mp < 10 ? 3 : -9);

  r.year+= (r.month <= 2);

  r.hour = (s % 86400) / 3600;
  r.minute = (s % 3600) / 60;
  r.second = s % 60;

  return r;
}
