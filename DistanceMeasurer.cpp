#include <Arduino.h>
#include <HCSR04.h>
#include "Utils.h"
#include "DistanceMeasurer.h"

DistanceMeasurer::DistanceMeasurer(
  byte triggerPin,
  byte echoPin,
  uint16_t maxDistanceInCentimeters,
  uint8_t numberOfDistanceMeasurementsToTrackForAverage,
  uint16_t distanceMeasurementsIntervalInMillisecondsForAverage) {
  _distanceSensor = new UltraSonicDistanceSensor(triggerPin, echoPin, maxDistanceInCentimeters);

  _maxDistanceInCentimeters = maxDistanceInCentimeters;
  _numberOfDistanceMeasurementsToTrackForAverage = numberOfDistanceMeasurementsToTrackForAverage;
  _distanceMeasurementsIntervalInMillisecondsForAverage = distanceMeasurementsIntervalInMillisecondsForAverage;

  _distanceMeasurementsInCentimetersForAverage = new float[_numberOfDistanceMeasurementsToTrackForAverage];
  _initializeDistanceMeasurements(_distanceMeasurementsInCentimetersForAverage, _numberOfDistanceMeasurementsToTrackForAverage);

  _recordNextDistanceMeasurementForAverageAtIndex = 0;
  _lastDistanceMeasurementForAverageMillis = 0;
  _isReadyToReportDistance = false;
}

DistanceMeasurer::~DistanceMeasurer() {
  delete _distanceSensor;
  delete[] _distanceMeasurementsInCentimetersForAverage;
}

void DistanceMeasurer::_initializeDistanceMeasurements(
  float* distanceMeasurements,
  uint8_t numberOfDistanceMeasurements) {
  for (int i = 0; i < numberOfDistanceMeasurements; i++) {
    distanceMeasurements[i] = 0;
  }
}

bool DistanceMeasurer::isReady() {
  return _isReadyToReportDistance;
}

float DistanceMeasurer::getDistanceInCentimeters() {
  if (isReady()) return _getAverageDistanceMeasurement();
  else return -1.0;
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

void DistanceMeasurer::synchronize(unsigned long currentMillis) {
  if (currentMillis - _lastDistanceMeasurementForAverageMillis > _distanceMeasurementsIntervalInMillisecondsForAverage) {
    _recordNextDistanceMeasurementForAverage();

    _lastDistanceMeasurementForAverageMillis = currentMillis;
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
