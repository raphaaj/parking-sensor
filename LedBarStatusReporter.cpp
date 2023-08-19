#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "LedBarStatusReporter.h"

LedBarStatusReporter::LedBarStatusReporter(
  byte pin,
  unsigned short numberOfLeds,
  uint8_t brightness) {
  _ledBarHandler = new Adafruit_NeoPixel(numberOfLeds, pin, NEO_GRB + NEO_KHZ800);

  _numberOfLeds = numberOfLeds;
  _brightness = brightness;
  _color = _ledBarHandler->Color(255, 0, 0);

  _isReportingStatus = false;
  _numberOfLedsToLight = 0;
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
}

void LedBarStatusReporter::startReportingStatus(unsigned short numberOfLedsToLight) {
  _isReportingStatus = true;
  _numberOfLedsToLight = numberOfLedsToLight;

  _updateLedBarStatusReport();
}

void LedBarStatusReporter::setNumberOfLedsToLight(unsigned short numberOfLedsToLight) {
  _numberOfLedsToLight = numberOfLedsToLight;

  _updateLedBarStatusReport();
}

void LedBarStatusReporter::stopReportingStatus() {
  _isReportingStatus = false;
  _numberOfLedsToLight = 0;

  _updateLedBarStatusReport();
}

bool LedBarStatusReporter::isReportingStatus() {
  return _isReportingStatus;
}

void LedBarStatusReporter::_updateLedBarStatusReport() {
  _ledBarHandler->clear();

  if (_isReportingStatus) {
    for (int i = 0; i < _numberOfLeds; i++) {
      if (i < _numberOfLedsToLight) {
        _ledBarHandler->setPixelColor(i, _color);
        _ledBarHandler->setBrightness(_brightness);
      }
    }
  }

  _ledBarHandler->show();
}