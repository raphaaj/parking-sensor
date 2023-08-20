#include "DistanceMeasurer.h"
#include "Buzzer.h"
#include "LedBarStatusReporter.h"

#define DISTANCE_MEASURER_TRIGGER_PIN 2
#define DISTANCE_MEASURER_ECHO_PIN 3
#define DISTANCE_MEASURER_MAX_DISTANCE_IN_CENTIMETERS 150
#define DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_AVERAGE 5
#define DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_STABILITY 5
#define DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_AVERAGE 100
#define DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_STABILITY 250
#define DISTANCE_MEASURER_MAX_DISTANCE_MEASUREMENT_STANDARD_DEVIATION_FOR_STABILITY 1
#define DISTANCE_MEASURER_MAX_AVERAGE_SPEED_IN_CENTIMETERS_PER_SECOND_FOR_STABILITY 0.5

#define LED_BAR_STATUS_REPORTER_PIN 5
#define LED_BAR_STATUS_REPORTER_TOTAL_NUMBER_OF_LEDS 8
#define LED_BAR_STATUS_REPORTER_BRIGHTNESS 32
#define LED_BAR_STATUS_REPORTER_BLINK_INTERVAL_IN_MILLISECONDS 350

#define BUZZER_PIN 4

#define HEARTBEAT_INTERVAL_IN_MILLISECONDS 1000

#define MIN_DISTANCE_TO_REPORT_STATUS_IN_CENTIMETERS 100
#define STATUS_REPORT_UPDATE_INTERVAL_IN_MILLISECONDS 250

#define MIN_DISTANCE_IN_CENTIMETERS_TO_START_PREPARING_TO_DISABLE_STATUS_REPORT 5
#define MIN_DISTANCE_IN_CENTIMETERS_TO_ENABLE_STATUS_REPORT_AFTER_BEING_DISABLED 100
#define TIME_FOR_PREPARATION_TO_DISABLE_STATUS_REPORT_IN_MILLISECONDS 10000

bool isStatusReportEnabled = true;
bool isPreparingToDisableStatusReport = false;

unsigned long heartbeatPreviousMillis = 0;
unsigned long statusReportUpdatePreviousMillis = 0;
unsigned long preparationToDisableStatusReportStartMillis = 0;

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

LedBarStatusReporter ledBarStatusReporter(
  LED_BAR_STATUS_REPORTER_PIN,
  LED_BAR_STATUS_REPORTER_TOTAL_NUMBER_OF_LEDS,
  LED_BAR_STATUS_REPORTER_BRIGHTNESS);

Buzzer buzzer(BUZZER_PIN);

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
  if (currentMillis - heartbeatPreviousMillis >= HEARTBEAT_INTERVAL_IN_MILLISECONDS) {
    Serial.println("Heartbeat");

    heartbeatPreviousMillis = currentMillis;
  }

  // desabilitar status report após preparação
  if (isPreparingToDisableStatusReport
      && currentMillis - preparationToDisableStatusReportStartMillis > TIME_FOR_PREPARATION_TO_DISABLE_STATUS_REPORT_IN_MILLISECONDS) {
    Serial.println("Desativando alertas");

    isStatusReportEnabled = false;
    isPreparingToDisableStatusReport = false;

    if (ledBarStatusReporter.isReportingStatus()) {
      Serial.println("Finalizando alerta visual");
      ledBarStatusReporter.stopReportingStatus();
    }

    if (buzzer.isAlerting()) {
      Serial.println("Finalizando alerta sonoro");
      buzzer.stopAlerting(currentMillis);
    }
  }

  if (distanceMeasurer.isReady()) {
    if (currentMillis - statusReportUpdatePreviousMillis > STATUS_REPORT_UPDATE_INTERVAL_IN_MILLISECONDS) {
      statusReportUpdatePreviousMillis = currentMillis;

      float currentDistanceInCentimeters = distanceMeasurer.getDistanceInCentimeters();

      Serial.print("Distância: ");
      Serial.print(currentDistanceInCentimeters, 2);
      Serial.println(" cm");

      // status report
      if (isStatusReportEnabled) {

        // verificação para início da preparação para desativação do status report
        if (currentDistanceInCentimeters < MIN_DISTANCE_IN_CENTIMETERS_TO_START_PREPARING_TO_DISABLE_STATUS_REPORT
            && !isPreparingToDisableStatusReport) {
          Serial.println("Iniciando preparação para desativação dos alertas");

          isPreparingToDisableStatusReport = true;
          preparationToDisableStatusReportStartMillis = currentMillis;

          ledBarStatusReporter.startBlinking(LED_BAR_STATUS_REPORTER_BLINK_INTERVAL_IN_MILLISECONDS);
        }

        // atualização do status
        if (currentDistanceInCentimeters < MIN_DISTANCE_TO_REPORT_STATUS_IN_CENTIMETERS) {
          uint16_t buzzerAlertBeatIntervalInMs = getBuzzerAlertBeatIntervalInMsFromDistanceInCm(currentDistanceInCentimeters);
          uint8_t numberOfLedsToLight = getNumberOfLedsToLightFromDistanceInCm(currentDistanceInCentimeters);

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
        } else {
          if (ledBarStatusReporter.isReportingStatus()) {
            Serial.println("Finalizando alerta visual");
            ledBarStatusReporter.stopReportingStatus();
          }

          if (buzzer.isAlerting()) {
            Serial.println("Finalizando alerta sonoro");
            buzzer.stopAlerting(currentMillis);
          }
        }
      } else {
        // reabilitação do status report após ter sido desabilitado
        if (currentDistanceInCentimeters > MIN_DISTANCE_IN_CENTIMETERS_TO_ENABLE_STATUS_REPORT_AFTER_BEING_DISABLED) {
          Serial.println("Reativando alertas");
          isStatusReportEnabled = true;
        }
      }
    }
  }

  distanceMeasurer.synchronize(currentMillis);
  buzzer.synchronize(currentMillis);
  ledBarStatusReporter.synchronize(currentMillis);
}

uint16_t getBuzzerAlertBeatIntervalInMsFromDistanceInCm(float distanceInCm) {
  if (distanceInCm > 100) return 2000;
  else if (distanceInCm > 50) return 1000;
  else if (distanceInCm > 25) return 500;
  else if (distanceInCm > 5) return 250;
  else return 125;
}

uint8_t getNumberOfLedsToLightFromDistanceInCm(float distanceInCm) {
  if (distanceInCm > 100) return 0;
  else if (distanceInCm > 75) return 1;
  else if (distanceInCm > 50) return 2;
  else if (distanceInCm > 25) return 3;
  else if (distanceInCm > 20) return 4;
  else if (distanceInCm > 15) return 5;
  else if (distanceInCm > 10) return 6;
  else if (distanceInCm > 5) return 7;
  else return 8;
}
