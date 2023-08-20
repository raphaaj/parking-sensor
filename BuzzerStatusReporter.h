#ifndef BuzzerStatusReporter_h
#define BuzzerStatusReporter_h

class BuzzerStatusReporter {
public:
  BuzzerStatusReporter(byte pin);
  void reportInitialization();
  bool isReportingStatus();
  void startReportingStatus(uint16_t statusReportBeepIntervalInMilliseconds);
  void setStatusReportBeepInterval(uint16_t statusReportBeepIntervalInMilliseconds);
  void stopReportingStatus(unsigned long currentMillis);
  void synchronize(unsigned long currentMillis);
private:
  static const uint16_t _initializationReportSingleBeepDurationInMilliseconds;
  static const float _initializationReportBeepTones[];
  static const uint16_t _statusReportBeepDurationInMilliseconds;
  static const float _statusReportBeepTone;
  byte _pin;
  bool _isReportingStatus;
  uint16_t _statusReportBeepIntervalInMilliseconds;
  bool _isPlayingStatusReportBeep;
  unsigned long _statusReportBeepStartMillis;
  unsigned long _statusReportBeepEndMillis;
  void _startPlayingStatusReportBeep(unsigned long currentMillis);
  void _stopPlayingStatusReportBeep(unsigned long currentMillis);
};

#endif