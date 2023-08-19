#include "DistanceMeasurer.h"
#include "Buzzer.h"
#include "LedBarStatusReporter.h"

#define HEARTBEAT_INTERVAL 1000

#define DISTANCE_MEASURER_TRIGGER_PIN 2
#define DISTANCE_MEASURER_ECHO_PIN 3
#define DISTANCE_MEASURER_MAX_DISTANCE_IN_CENTIMETERS 200
#define DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_AVERAGE 5
#define DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_STABILITY 5
#define DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_AVERAGE 100
#define DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_STABILITY 250
#define DISTANCE_MEASURER_MAX_DISTANCE_MEASUREMENT_STANDARD_DEVIATION_FOR_STABILITY 1
#define DISTANCE_MEASURER_MAX_AVERAGE_SPEED_IN_CENTIMETERS_PER_SECOND_FOR_STABILITY 0.5

#define BUZZER_PIN 4

#define LED_BAR_STATUS_REPORTER_PIN 5
#define LED_BAR_STATUS_REPORTER_TOTAL_NUMBER_OF_LEDS 8
#define LED_BAR_STATUS_REPORTER_BRIGHTNESS 8
#define LED_BAR_STATUS_REPORTER_BLINK_INTERVAL_IN_MILLISECONDS 350

#define ALERT_MAX_DISTANCE 100
#define ALERT_INTERVAL_IN_MS_TO_UPDATE_STATUS 250

unsigned long heartbeatPreviousMillis = 0;
unsigned long distanceStatusUpdatePreviousMillis = 0;

DistanceMeasurer distanceMeasurer(
  DISTANCE_MEASURER_TRIGGER_PIN,
  DISTANCE_MEASURER_ECHO_PIN,
  DISTANCE_MEASURER_MAX_DISTANCE_IN_CENTIMETERS,
  DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_AVERAGE,
  DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_STABILITY,
  DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_AVERAGE,
  DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_STABILITY,
  DISTANCE_MEASURER_MAX_DISTANCE_MEASUREMENT_STANDARD_DEVIATION_FOR_STABILITY,
  DISTANCE_MEASURER_MAX_AVERAGE_SPEED_IN_CENTIMETERS_PER_SECOND_FOR_STABILITY);

Buzzer buzzer(BUZZER_PIN);

LedBarStatusReporter ledBarStatusReporter(
  LED_BAR_STATUS_REPORTER_PIN,
  LED_BAR_STATUS_REPORTER_TOTAL_NUMBER_OF_LEDS,
  LED_BAR_STATUS_REPORTER_BRIGHTNESS);

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando");

  LedBarStatusReporter::setup();

  ledBarStatusReporter.begin();

  // buzzer.alertInitialization();

  Serial.println("Inicialização finalizada");
}

void loop() {
  unsigned long currentMillis = millis();

  // heartbeat
  if (currentMillis - heartbeatPreviousMillis >= HEARTBEAT_INTERVAL) {
    heartbeatPreviousMillis = currentMillis;
    Serial.println("Heartbeat");
  }

  // status
  if (distanceMeasurer.isReady() && currentMillis - distanceStatusUpdatePreviousMillis > ALERT_INTERVAL_IN_MS_TO_UPDATE_STATUS) {
    distanceStatusUpdatePreviousMillis = currentMillis;

    float currentDistanceInCentimeters = distanceMeasurer.getDistanceInCentimeters();

    Serial.print("Distância: ");
    Serial.print(currentDistanceInCentimeters, 2);
    Serial.print(" cm | ");

    float averageSpeedInCentimetersPerSeconds = distanceMeasurer.getAverageSpeedInCentimetersPerSecond();

    Serial.print("Velocidade: ");
    Serial.print(averageSpeedInCentimetersPerSeconds, 2);
    Serial.print(" cm/s | ");

    Serial.print("Estável: ");
    if (distanceMeasurer.isDistanceSteady()) {
      Serial.println("sim");
    } else {
      Serial.println("não");
    }

    if (currentDistanceInCentimeters >= 0 && currentDistanceInCentimeters < ALERT_MAX_DISTANCE) {
      int buzzerAlertBeatIntervalInMs = getBuzzerAlertBeatIntervalInMsFromDistanceInCm(currentDistanceInCentimeters);
      unsigned short numberOfLedsToLight = getNumberOfLedsToLightFromDistanceInCm(currentDistanceInCentimeters);

      if (buzzer.isAlerting()) {
        buzzer.setAlertBeatsInterval(buzzerAlertBeatIntervalInMs);
      } else {
        // Serial.println("Iniciando alerta sonoro");
        // buzzer.startAlerting(buzzerAlertBeatIntervalInMs);
      }

      if (ledBarStatusReporter.isReportingStatus()) {
        ledBarStatusReporter.setNumberOfLedsToLight(numberOfLedsToLight);
      } else {
        Serial.println("Iniciando alerta visual");
        ledBarStatusReporter.startReportingStatus(numberOfLedsToLight);
      }

      if (ledBarStatusReporter.allLedsOn()) {
        if (!ledBarStatusReporter.isBlinking()) {
          Serial.println("Iniciando blink no alerta visual");
          ledBarStatusReporter.startBlinking(LED_BAR_STATUS_REPORTER_BLINK_INTERVAL_IN_MILLISECONDS);
        }
      } else {
        if (ledBarStatusReporter.isBlinking()) {
          Serial.println("Encerrando blink no alerta visual");
          ledBarStatusReporter.stopBlinking();
        }
      }
    } else {
      if (buzzer.isAlerting()) {
        Serial.println("Encerrando alerta sonoro");
        buzzer.stopAlerting(currentMillis);
      }

      if (ledBarStatusReporter.isReportingStatus()) {
        Serial.println("Encerrando alerta visual");
        ledBarStatusReporter.stopReportingStatus();
      }
    }
  }

  distanceMeasurer.synchronize(currentMillis);
  buzzer.synchronize(currentMillis);
  ledBarStatusReporter.synchronize(currentMillis);
}

int getBuzzerAlertBeatIntervalInMsFromDistanceInCm(float distanceInCm) {
  if (distanceInCm > 60) return 2000;
  else if (distanceInCm > 30) return 1000;
  else if (distanceInCm > 15) return 500;
  else if (distanceInCm > 6) return 250;
  else return 125;
}

unsigned short getNumberOfLedsToLightFromDistanceInCm(float distanceInCm) {
  if (distanceInCm > 100) return 0;
  else if (distanceInCm > 75) return 1;
  else if (distanceInCm > 50) return 2;
  else if (distanceInCm > 25) return 3;
  else if (distanceInCm > 20) return 4;
  else if (distanceInCm > 15) return 5;
  else if (distanceInCm > 10) return 6;
  else if (distanceInCm > 6) return 7;
  else return 8;
}
