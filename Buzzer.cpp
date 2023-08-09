#include "Buzzer.h"

Buzzer::Buzzer(byte pin) {
  _pin = pin;
  _isAlerting = false;
  _alertBeatsIntervalInMilliseconds = 0;
  _isPlayingAlertBeat = false;
  _alertBeatStartMillis = 0;
  _alertBeatEndMillis = 0;

  pinMode(_pin, OUTPUT);
}

const unsigned long Buzzer::_initializationAlertSingleBeatDurationInMilliseconds = 100;
const float Buzzer::_initializationAlertBeatTones[] = { 440, 493.88, 523.25 };
const unsigned long Buzzer::_alertBeatDurationInMilliseconds = 100;
const float Buzzer::_alertBeatTone = 853;

void Buzzer::alertInitialization() {
  for (const float &initializationAlertBeatTone : _initializationAlertBeatTones) {
    tone(_pin, initializationAlertBeatTone, _initializationAlertSingleBeatDurationInMilliseconds);
    delay(_initializationAlertSingleBeatDurationInMilliseconds);
  }
}

void Buzzer::startAlerting(unsigned int alertBeatsIntervalInMilliseconds) {
  _isAlerting = true;
  _alertBeatsIntervalInMilliseconds = alertBeatsIntervalInMilliseconds;
}

void Buzzer::setAlertBeatsInterval(unsigned int alertBeatsIntervalInMilliseconds) {
  _alertBeatsIntervalInMilliseconds = alertBeatsIntervalInMilliseconds;
}

void Buzzer::stopAlerting() {
  stopAlerting(millis());
}

void Buzzer::stopAlerting(unsigned long currentMillis) {
  _stopPlayingAlertBeat(currentMillis);

  _isAlerting = false;
  _alertBeatsIntervalInMilliseconds = 0;
}

bool Buzzer::isAlerting() {
  return _isAlerting;
}

void Buzzer::synchronize(unsigned long currentMillis) {
  if (_isAlerting) {
    if (_isPlayingAlertBeat) {
      if (currentMillis - _alertBeatStartMillis >= _alertBeatDurationInMilliseconds) {
        _stopPlayingAlertBeat(currentMillis);
      }
    } else {
      if (currentMillis - _alertBeatEndMillis >= _alertBeatsIntervalInMilliseconds) {
        _startPlayingAlertBeat(currentMillis);
      }
    }
  }
}

void Buzzer::_startPlayingAlertBeat(unsigned long currentMillis) {
  if (!_isPlayingAlertBeat) {
    _isPlayingAlertBeat = true;
    _alertBeatStartMillis = currentMillis;
    tone(_pin, _alertBeatTone);
  }
}

void Buzzer::_stopPlayingAlertBeat(unsigned long currentMillis) {
  if (_isPlayingAlertBeat) {
    _isPlayingAlertBeat = false;
    _alertBeatEndMillis = currentMillis;
    noTone(_pin);
  }
}