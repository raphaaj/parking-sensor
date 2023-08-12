// https://github.com/Martinsos/arduino-lib-hc-sr04
#include <HCSR04.h>

// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "Buzzer.h"
#include "DistanceMeasurer.h"
#include "Utils.h"

#define HEARTBEAT_INTERVAL 1000

#define DISTANCE_MEASURER_TRIGGER_PIN 2
#define DISTANCE_MEASURER_ECHO_PIN 3
#define DISTANCE_MEASURER_MAX_DISTANCE_IN_CENTIMETERS 200
#define DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_AVERAGE 5
#define DISTANCE_MEASURER_NUMBER_OF_DISTANCE_MEASUREMENTS_TO_TRACK_FOR_STABILITY 5
#define DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_AVERAGE 50
#define DISTANCE_MEASURER_DISTANCE_MEASUREMENTS_INTERVAL_IN_MS_FOR_STABILITY 250
#define DISTANCE_MEASURER_MAX_DISTANCE_MEASUREMENT_STANDARD_DEVIATION_FOR_STABILITY 1
#define DISTANCE_MEASURER_MAX_AVERAGE_SPEED_IN_CENTIMETERS_PER_SECOND_FOR_STABILITY 0.5

#define BUZZER_PIN 4

#define LED_PIN 5
#define LED_NUM_PIXELS 8
#define LED_DELAYVAL 500

#define ALERT_MAX_DISTANCE 100
#define ALERT_INTERVAL_IN_MS_TO_UPDATE_STATUS 250

unsigned long heartbeatPreviousMillis = 0;
unsigned long distanceStatusUpdatePreviousMillis = 0;

Buzzer buzzer(BUZZER_PIN);

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

Adafruit_NeoPixel pixels(LED_NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando");

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();

  // buzzer.alertInitialization();

  Serial.println("Inicialização finalizada");
}

void loop() {
  // pixels.clear();

  // for (int i = 0; i < LED_NUM_PIXELS; i++) {

  //   pixels.setPixelColor(i, pixels.Color(150, 0, 0));
  //   pixels.setBrightness(32);
  //   pixels.show();
  //   delay(LED_DELAYVAL);
  // }

  unsigned long currentMillis = millis();

  // heartbeat
  if (currentMillis - heartbeatPreviousMillis >= HEARTBEAT_INTERVAL) {
    heartbeatPreviousMillis = currentMillis;
    Serial.println("Heartbeat");
  }

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

      if (buzzer.isAlerting()) {
        buzzer.setAlertBeatsInterval(buzzerAlertBeatIntervalInMs);
      } else {
        Serial.println("Iniciando alerta sonoro");
        buzzer.startAlerting(buzzerAlertBeatIntervalInMs);
      }
    } else {
      if (buzzer.isAlerting()) {
        Serial.println("Encerrando alerta sonoro");
        buzzer.stopAlerting(currentMillis);
      }
    }
  }

  distanceMeasurer.synchronize(currentMillis);
  buzzer.synchronize(currentMillis);
}

int getBuzzerAlertBeatIntervalInMsFromDistanceInCm(float distanceInCm) {
  if (distanceInCm > 60) {
    return 2500;
  } else if (distanceInCm > 30) {
    return 1600;
  } else if (distanceInCm > 15) {
    return 800;
  } else if (distanceInCm > 7.5) {
    return 400;
  } else {
    return 200;
  }
}
