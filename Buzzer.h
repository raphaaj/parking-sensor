#ifndef Buzzer_h
#define Buzzer_h

class Buzzer {
public:
  Buzzer(byte pin);
  void alertInitialization();
  void startAlerting(unsigned int alertBeatsIntervalInMilliseconds);
  void setAlertBeatsInterval(unsigned int alertBeatsIntervalInMilliseconds);
  void stopAlerting();
  void stopAlerting(unsigned long currentMillis);
  bool isAlerting();
  void synchronize(unsigned long currentMillis);
private:
  byte _pin;
  bool _isAlerting;
  int _alertBeatsIntervalInMilliseconds;
  bool _isPlayingAlertBeat;
  unsigned long _alertBeatStartMillis;
  unsigned long _alertBeatEndMillis;
  static const unsigned long _initializationAlertSingleBeatDurationInMilliseconds;
  static const float _initializationAlertBeatTones[];
  static const unsigned long _alertBeatDurationInMilliseconds;
  static const float _alertBeatTone;
  void _stopPlayingAlertBeat(unsigned long currentMillis);
  void _startPlayingAlertBeat(unsigned long currentMillis);
};

#endif