#ifndef DistanceMeasurer_h
#define DistanceMeasurer_h

// https://github.com/Martinsos/arduino-lib-hc-sr04
#include <HCSR04.h>

class DistanceMeasurer {
public:
  DistanceMeasurer(
    byte triggerPin,
    byte echoPin,
    unsigned short maxDistanceInCentimeters,
    unsigned short numberOfDistanceMeasurementsToTrackForAverage,
    unsigned short numberOfDistanceMeasurementsToTrackForStability,
    unsigned short distanceMeasurementsIntervalInMillisecondsForAverage,
    unsigned short distanceMeasurementsIntervalInMillisecondsForStability,
    float maxDistanceMeasurementStandardDeviationForStability,
    float maxAverageSpeedInCentimetersPerSecondForStability);
  ~DistanceMeasurer();
  void synchronize(unsigned long currentMillis);
  bool isReady();
  float getDistanceInCentimeters();
  float getAverageSpeedInCentimetersPerSecond();
  bool isDistanceSteady();
private:
  UltraSonicDistanceSensor* _distanceSensor;
  unsigned short _maxDistanceInCentimeters;
  unsigned short _numberOfDistanceMeasurementsToTrackForAverage;
  unsigned short _numberOfDistanceMeasurementsToTrackForStability;
  unsigned short _distanceMeasurementsIntervalInMillisecondsForAverage;
  unsigned short _distanceMeasurementsIntervalInMillisecondsForStability;
  float _maxDistanceMeasurementStandardDeviationForStability;
  float _maxAverageSpeedInCentimetersPerSecondForStability;
  float* _distanceMeasurementsInCentimetersForAverage;
  float* _distanceMeasurementsInCentimetersForStability;
  unsigned short _recordNextDistanceMeasurementForAverageAtIndex;
  unsigned short _recordNextDistanceMeasurementForStabilityAtIndex;
  unsigned long _lastDistanceMeasurementForAverageMillis;
  unsigned long _lastDistanceMeasurementForStabilityMillis;
  bool _isReadyToReportDistance;
  bool _isReadyToCheckDistanceStability;
  void _initializeDistanceMeasurements(
    float* distanceMeasurements,
    unsigned short numberOfDistanceMeasurements);
  void _recordNextDistanceMeasurementForAverage();
  void _recordNextDistanceMeasurementForStability();
  float _getAverageDistanceMeasurement();
  float _getAverageSpeedInCentimetersPerSecondInStabilityTrackWindow();
};

#endif