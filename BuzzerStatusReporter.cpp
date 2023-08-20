#include <Arduino.h>
#include "BuzzerStatusReporter.h"

BuzzerStatusReporter::BuzzerStatusReporter(byte pin) {
  _pin = pin;

  _isReportingStatus = false;
  _statusReportBeepIntervalInMilliseconds = 0;
  _isPlayingStatusReportBeep = false;
  _statusReportBeepStartMillis = 0;
  _statusReportBeepEndMillis = 0;

  pinMode(_pin, OUTPUT);
  noTone(_pin);
}

const uint16_t BuzzerStatusReporter::_initializationReportSingleBeepDurationInMilliseconds = 100;
const float BuzzerStatusReporter::_initializationReportBeepTones[] = { 440, 493.88, 523.25 };
const uint16_t BuzzerStatusReporter::_statusReportBeepDurationInMilliseconds = 100;
const float BuzzerStatusReporter::_statusReportBeepTone = 853;

void BuzzerStatusReporter::reportInitialization() {
  for (const float &initializationAlertBeatTone : _initializationReportBeepTones) {
    tone(_pin, initializationAlertBeatTone, _initializationReportSingleBeepDurationInMilliseconds);
    delay(_initializationReportSingleBeepDurationInMilliseconds);
  }
}

bool BuzzerStatusReporter::isReportingStatus() {
  return _isReportingStatus;
}

void BuzzerStatusReporter::startReportingStatus(uint16_t statusReportBeepIntervalInMilliseconds) {
  _isReportingStatus = true;
  _statusReportBeepIntervalInMilliseconds = statusReportBeepIntervalInMilliseconds;
}

void BuzzerStatusReporter::setStatusReportBeepInterval(uint16_t statusReportBeepIntervalInMilliseconds) {
  _statusReportBeepIntervalInMilliseconds = statusReportBeepIntervalInMilliseconds;
}

void BuzzerStatusReporter::stopReportingStatus(unsigned long currentMillis) {
  _isReportingStatus = false;
  _statusReportBeepIntervalInMilliseconds = 0;

  _stopPlayingStatusReportBeep(currentMillis);
}

void BuzzerStatusReporter::synchronize(unsigned long currentMillis) {
  if (_isReportingStatus) {
    if (_isPlayingStatusReportBeep) {
      if (currentMillis - _statusReportBeepStartMillis >= _statusReportBeepDurationInMilliseconds) {
        _stopPlayingStatusReportBeep(currentMillis);
      }
    } else {
      if (currentMillis - _statusReportBeepEndMillis >= _statusReportBeepIntervalInMilliseconds) {
        _startPlayingStatusReportBeep(currentMillis);
      }
    }
  }
}

void BuzzerStatusReporter::_startPlayingStatusReportBeep(unsigned long currentMillis) {
  _isPlayingStatusReportBeep = true;
  _statusReportBeepStartMillis = currentMillis;
  tone(_pin, _statusReportBeepTone);
}

void BuzzerStatusReporter::_stopPlayingStatusReportBeep(unsigned long currentMillis) {
  _isPlayingStatusReportBeep = false;
  _statusReportBeepEndMillis = currentMillis;
  noTone(_pin);
}