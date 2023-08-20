#ifndef DistanceMeasurer_h
#define DistanceMeasurer_h

// https://github.com/Martinsos/arduino-lib-hc-sr04
#include <HCSR04.h>

class DistanceMeasurer {
public:
  DistanceMeasurer(
    byte triggerPin,
    byte echoPin,
    uint16_t maxDistanceInCentimeters,
    uint8_t numberOfDistanceMeasurementsToTrackForAverage,
    uint16_t distanceMeasurementsIntervalInMillisecondsForAverage);
  ~DistanceMeasurer();
  bool isReady();
  float getDistanceInCentimeters();
  void synchronize(unsigned long currentMillis);
private:
  UltraSonicDistanceSensor* _distanceSensor;
  uint16_t _maxDistanceInCentimeters;
  uint8_t _numberOfDistanceMeasurementsToTrackForAverage;
  uint16_t _distanceMeasurementsIntervalInMillisecondsForAverage;
  float* _distanceMeasurementsInCentimetersForAverage;
  uint8_t _recordNextDistanceMeasurementForAverageAtIndex;
  unsigned long _lastDistanceMeasurementForAverageMillis;
  bool _isReadyToReportDistance;
  void _initializeDistanceMeasurements(
    float* distanceMeasurements,
    uint8_t numberOfDistanceMeasurements);
  void _recordNextDistanceMeasurementForAverage();
  float _getAverageDistanceMeasurement();
};

#endif