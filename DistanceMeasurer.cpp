#include <Arduino.h>
#include <HCSR04.h>
#include "Utils.h"
#include "DistanceMeasurer.h"

DistanceMeasurer::DistanceMeasurer(
  byte triggerPin,
  byte echoPin,
  unsigned short maxDistanceInCentimeters,
  unsigned short numberOfDistanceMeasurementsToTrackForAverage,
  unsigned short numberOfDistanceMeasurementsToTrackForStability,
  unsigned short distanceMeasurementsIntervalInMillisecondsForAverage,
  unsigned short distanceMeasurementsIntervalInMillisecondsForStability,
  float maxDistanceMeasurementStandardDeviationForStability,
  float maxAverageSpeedInCentimetersPerSecondForStability) {
  _distanceSensor = new UltraSonicDistanceSensor(triggerPin, echoPin, maxDistanceInCentimeters);

  _maxDistanceInCentimeters = maxDistanceInCentimeters;
  _numberOfDistanceMeasurementsToTrackForAverage = numberOfDistanceMeasurementsToTrackForAverage;
  _numberOfDistanceMeasurementsToTrackForStability = numberOfDistanceMeasurementsToTrackForStability;
  _distanceMeasurementsIntervalInMillisecondsForAverage = distanceMeasurementsIntervalInMillisecondsForAverage;
  _distanceMeasurementsIntervalInMillisecondsForStability = distanceMeasurementsIntervalInMillisecondsForStability;
  _maxDistanceMeasurementStandardDeviationForStability = maxDistanceMeasurementStandardDeviationForStability;
  _maxAverageSpeedInCentimetersPerSecondForStability = maxAverageSpeedInCentimetersPerSecondForStability;

  _distanceMeasurementsInCentimetersForAverage = new float[_numberOfDistanceMeasurementsToTrackForAverage];
  _initializeDistanceMeasurements(_distanceMeasurementsInCentimetersForAverage, _numberOfDistanceMeasurementsToTrackForAverage);

  _distanceMeasurementsInCentimetersForStability = new float[_numberOfDistanceMeasurementsToTrackForStability];
  _initializeDistanceMeasurements(_distanceMeasurementsInCentimetersForStability, _numberOfDistanceMeasurementsToTrackForStability);

  _recordNextDistanceMeasurementForAverageAtIndex = 0;
  _recordNextDistanceMeasurementForStabilityAtIndex = 0;
  _lastDistanceMeasurementForAverageMillis = 0;
  _lastDistanceMeasurementForStabilityMillis = 0;
  _isReadyToReportDistance = false;
  _isReadyToCheckDistanceStability = false;
}

DistanceMeasurer::~DistanceMeasurer() {
  delete _distanceSensor;
  delete[] _distanceMeasurementsInCentimetersForAverage;
  delete[] _distanceMeasurementsInCentimetersForStability;
}

void DistanceMeasurer::_initializeDistanceMeasurements(
  float* distanceMeasurements,
  unsigned short numberOfDistanceMeasurements) {
  for (int i = 0; i < numberOfDistanceMeasurements; i++) {
    distanceMeasurements[i] = 0;
  }
}

void DistanceMeasurer::synchronize(unsigned long currentMillis) {
  if (currentMillis - _lastDistanceMeasurementForAverageMillis > _distanceMeasurementsIntervalInMillisecondsForAverage) {
    _recordNextDistanceMeasurementForAverage();

    _lastDistanceMeasurementForAverageMillis = currentMillis;
  }

  if (_isReadyToReportDistance
      && currentMillis - _lastDistanceMeasurementForStabilityMillis > _distanceMeasurementsIntervalInMillisecondsForStability) {
    _recordNextDistanceMeasurementForStability();

    _lastDistanceMeasurementForStabilityMillis = currentMillis;
  }
}

void DistanceMeasurer::_recordNextDistanceMeasurementForAverage() {
  float currentDistanceInCentimeters = _distanceSensor->measureDistanceCm();

  if (currentDistanceInCentimeters < 0) currentDistanceInCentimeters = _maxDistanceInCentimeters;

  _distanceMeasurementsInCentimetersForAverage[_recordNextDistanceMeasurementForAverageAtIndex] = currentDistanceInCentimeters;

  if (_recordNextDistanceMeasurementForAverageAtIndex < _numberOfDistanceMeasurementsToTrackForAverage - 1) {
    _recordNextDistanceMeasurementForAverageAtIndex++;
  } else {
    _recordNextDistanceMeasurementForAverageAtIndex = 0;
    _isReadyToReportDistance = true;
  }
}


void DistanceMeasurer::_recordNextDistanceMeasurementForStability() {
  float meanDistanceMeasurement = _getAverageDistanceMeasurement();

  _distanceMeasurementsInCentimetersForStability[_recordNextDistanceMeasurementForStabilityAtIndex] = meanDistanceMeasurement;

  if (_recordNextDistanceMeasurementForStabilityAtIndex < _numberOfDistanceMeasurementsToTrackForStability - 1) {
    _recordNextDistanceMeasurementForStabilityAtIndex++;
  } else {
    _recordNextDistanceMeasurementForStabilityAtIndex = 0;
    _isReadyToCheckDistanceStability = true;
  }
}

float DistanceMeasurer::_getAverageDistanceMeasurement() {
  if (_numberOfDistanceMeasurementsToTrackForAverage > 2) {
    float minDistanceMeasurement = _distanceMeasurementsInCentimetersForAverage[0];
    float maxDistanceMeasurement = _distanceMeasurementsInCentimetersForAverage[0];
    float distanceMeasurementsSum = _distanceMeasurementsInCentimetersForAverage[0];

    for (int i = 1; i < _numberOfDistanceMeasurementsToTrackForAverage; i++) {
      float currentDistanceMeasurement = _distanceMeasurementsInCentimetersForAverage[i];

      if (currentDistanceMeasurement < minDistanceMeasurement) minDistanceMeasurement = currentDistanceMeasurement;
      if (currentDistanceMeasurement > maxDistanceMeasurement) maxDistanceMeasurement = currentDistanceMeasurement;

      distanceMeasurementsSum += currentDistanceMeasurement;
    }

    distanceMeasurementsSum = distanceMeasurementsSum - minDistanceMeasurement - maxDistanceMeasurement;
    float averageDistanceMeasurement = distanceMeasurementsSum / (_numberOfDistanceMeasurementsToTrackForAverage - 2);

    return averageDistanceMeasurement;
  } else {
    return Utils::average(_distanceMeasurementsInCentimetersForAverage, _numberOfDistanceMeasurementsToTrackForAverage);
  }
}

bool DistanceMeasurer::isReady() {
  return _isReadyToReportDistance && _isReadyToCheckDistanceStability;
}

float DistanceMeasurer::getDistanceInCentimeters() {
  if (isReady()) return _getAverageDistanceMeasurement();
  else return -1.0;
}

float DistanceMeasurer::getAverageSpeedInCentimetersPerSecond() {
  if (isReady()) return _getAverageSpeedInCentimetersPerSecondInStabilityTrackWindow();
  else return 0.0;
}

bool DistanceMeasurer::isDistanceSteady() {
  if (!_isReadyToCheckDistanceStability) return false;

  float distanceMeasurementsStandardDeviation = Utils::standardDeviation(_distanceMeasurementsInCentimetersForStability,
                                                                         _numberOfDistanceMeasurementsToTrackForStability);

  float averageSpeedInCentimetersPerSecondInStabilityTrackWindow =
    _getAverageSpeedInCentimetersPerSecondInStabilityTrackWindow();

  return distanceMeasurementsStandardDeviation < _maxDistanceMeasurementStandardDeviationForStability
         && abs(averageSpeedInCentimetersPerSecondInStabilityTrackWindow) < _maxAverageSpeedInCentimetersPerSecondForStability;
}

float DistanceMeasurer::_getAverageSpeedInCentimetersPerSecondInStabilityTrackWindow() {
  if (_numberOfDistanceMeasurementsToTrackForStability <= 1) return 0.0;

  int indexOfOldestDistanceMeasurementForStability = _recordNextDistanceMeasurementForStabilityAtIndex;

  float oldestDistanceMeasurementInCentimetersForStability =
    _distanceMeasurementsInCentimetersForStability[indexOfOldestDistanceMeasurementForStability];

  int indexOfMostRecentDistanceMeasurementForStability = _recordNextDistanceMeasurementForStabilityAtIndex - 1;
  if (indexOfMostRecentDistanceMeasurementForStability < 0)
    indexOfMostRecentDistanceMeasurementForStability = _numberOfDistanceMeasurementsToTrackForStability - 1;

  float mostRecentDistanceMeasurementInCentimetersForStability =
    _distanceMeasurementsInCentimetersForStability[indexOfMostRecentDistanceMeasurementForStability];

  float distanceVariationInCentimetersInStabilityTrackWindow =
    mostRecentDistanceMeasurementInCentimetersForStability - oldestDistanceMeasurementInCentimetersForStability;
  float timeElapsedInSecondsInStabilityTrackWindow =
    (_distanceMeasurementsIntervalInMillisecondsForStability * (_numberOfDistanceMeasurementsToTrackForStability - 1)) / 1000;

  float averageSpeedInCentimetersPerSecondInStabilityTrackWindow =
    distanceVariationInCentimetersInStabilityTrackWindow / timeElapsedInSecondsInStabilityTrackWindow;

  return averageSpeedInCentimetersPerSecondInStabilityTrackWindow;
}
