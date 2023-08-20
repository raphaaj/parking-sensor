#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "LedBarStatusReporter.h"

LedBarStatusReporter::LedBarStatusReporter(
  byte pin,
  uint8_t totalNumberOfLeds,
  uint8_t brightness) {
  _ledBarHandler = new Adafruit_NeoPixel(totalNumberOfLeds, pin, NEO_GRB + NEO_KHZ800);

  for (int i = 0; i < totalNumberOfLeds; i++) {
    _ledBarHandler->setBrightness(brightness);
  }

  _totalNumberOfLeds = totalNumberOfLeds;
  _brightness = brightness;
  _color = _ledBarHandler->Color(255, 0, 0);

  _isReportingStatus = false;
  _numberOfLedsToLight = 0;

  _blinkEnabled = false;
  _blinkIntervalInMilliseconds = 0;
  _blinkLedsOn = false;
  _blinkLedsOnStartMillis = 0;
  _blinkLedsOffStartMillis = 0;
}

LedBarStatusReporter::~LedBarStatusReporter() {
  delete _ledBarHandler;
}

void LedBarStatusReporter::setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
}

void LedBarStatusReporter::begin() {
  _ledBarHandler->begin();

  _updateLedBarStatusReport();
}

bool LedBarStatusReporter::isReportingStatus() {
  return _isReportingStatus;
}

bool LedBarStatusReporter::isBlinking() {
  return _blinkEnabled;
}

bool LedBarStatusReporter::allLedsOn() {
  return _numberOfLedsToLight == _totalNumberOfLeds;
}

void LedBarStatusReporter::startReportingStatus(uint8_t numberOfLedsToLight) {
  _isReportingStatus = true;
  _numberOfLedsToLight = numberOfLedsToLight;

  _updateLedBarStatusReport();
}

void LedBarStatusReporter::setNumberOfLedsToLight(uint8_t numberOfLedsToLight) {
  _numberOfLedsToLight = numberOfLedsToLight;

  _updateLedBarStatusReport();
}

void LedBarStatusReporter::stopReportingStatus() {
  _isReportingStatus = false;
  _numberOfLedsToLight = 0;

  stopBlinking();

  _updateLedBarStatusReport();
}

void LedBarStatusReporter::startBlinking(uint16_t blinkIntervalInMilliseconds) {
  _blinkEnabled = true;
  _blinkLedsOn = true;

  _blinkIntervalInMilliseconds = blinkIntervalInMilliseconds;
}

void LedBarStatusReporter::stopBlinking() {
  _blinkEnabled = false;
  _blinkLedsOn = false;

  _blinkIntervalInMilliseconds = 0;
}

void LedBarStatusReporter::synchronize(unsigned long currentMillis) {
  if (_isReportingStatus) {
    _synchronizeBlinkState(currentMillis);
  }
}

void LedBarStatusReporter::_synchronizeBlinkState(unsigned long currentMillis) {
  if (_blinkEnabled) {
    if (_blinkLedsOn) {
      if (currentMillis - _blinkLedsOnStartMillis >= _blinkIntervalInMilliseconds) {
        _blinkLedsOn = false;
        _blinkLedsOffStartMillis = currentMillis;

        _updateLedBarStatusReport();
      }
    } else {
      if (currentMillis - _blinkLedsOffStartMillis >= _blinkIntervalInMilliseconds) {
        _blinkLedsOn = true;
        _blinkLedsOnStartMillis = currentMillis;

        _updateLedBarStatusReport();
      }
    }
  }
}

void LedBarStatusReporter::_updateLedBarStatusReport() {
  _ledBarHandler->clear();

  if (_isReportingStatus && (!_blinkEnabled || (_blinkEnabled && _blinkLedsOn))) {
    for (int i = 0; i < _totalNumberOfLeds; i++) {
      if (i < _numberOfLedsToLight) {
        _ledBarHandler->setPixelColor(i, _color);
      }
    }
  }

  _ledBarHandler->show();
}