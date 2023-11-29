#ifndef LedBarStatusReporter_h
#define LedBarStatusReporter_h

// Adafruit_NeoPixel
// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

class LedBarStatusReporter {
public:
  LedBarStatusReporter(
    byte pin,
    uint8_t totalNumberOfLeds,
    uint8_t brightness);
  ~LedBarStatusReporter();
  static void setup();
  void begin();
  bool isReportingStatus();
  bool isBlinking();
  bool allLedsOn();
  void startReportingStatus(uint8_t numberOfLedsToLight);
  void setNumberOfLedsToLight(uint8_t numberOfLedsToLight);
  void stopReportingStatus();
  void startBlinking(uint16_t blinkIntervalInMilliseconds);
  void stopBlinking();
  void synchronize(unsigned long currentMillis);
private:
  Adafruit_NeoPixel* _ledBarHandler;
  uint8_t _totalNumberOfLeds;
  uint8_t _brightness;
  uint32_t _color;
  bool _isReportingStatus;
  uint8_t _numberOfLedsToLight;
  bool _blinkEnabled;
  uint16_t _blinkIntervalInMilliseconds;
  bool _blinkLedsOn;
  unsigned long _blinkLedsOnStartMillis;
  unsigned long _blinkLedsOffStartMillis;
  void _updateLedBarStatusReport();
  void _synchronizeBlinkState(unsigned long currentMillis);
};

#endif