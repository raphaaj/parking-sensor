#ifndef LedBarStatusReporter_h
#define LedBarStatusReporter_h

//https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

class LedBarStatusReporter {
public:
  LedBarStatusReporter(
    byte pin,
    unsigned short numberOfLeds,
    uint8_t brightness);
  ~LedBarStatusReporter();
  static void setup();
  void begin();
  void startReportingStatus(unsigned short numberOfLedsToLight);
  void setNumberOfLedsToLight(unsigned short numberOfLedsToLight);
  void stopReportingStatus();
  bool isReportingStatus();
private:
  Adafruit_NeoPixel* _ledBarHandler;
  unsigned short _numberOfLeds;
  uint8_t _brightness;
  uint32_t _color;
  bool _isReportingStatus;
  unsigned short _numberOfLedsToLight;
  void _updateLedBarStatusReport();
};

#endif