#include <Arduino.h>
#include "Utils.h"

float Utils::average(float* values, unsigned short numberOfValues) {
  float sum = 0.0;
  for (int i = 0; i < numberOfValues; i++) {
    sum += values[i];
  }

  return sum / numberOfValues;
}

float Utils::standardDeviation(float *values, unsigned short numberOfValues) {
  float valuesAverage = Utils::average(values, numberOfValues);

  float squaredDiffSum = 0.0;
  for (int i = 0; i < numberOfValues; i++) {
    float diff = values[i] - valuesAverage;
    squaredDiffSum += pow(diff, 2);
  }

  float variance = squaredDiffSum / numberOfValues;
  float standardDeviation = sqrt(variance);

  return standardDeviation;
}