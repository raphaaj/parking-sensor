// https://github.com/Martinsos/arduino-lib-hc-sr04
#include <HCSR04.h>

// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "Buzzer.h"

#define HEARTBEAT_INTERVAL 1000

#define DISTANCE_SENSOR_PIN_TRIG 2
#define DISTANCE_SENSOR_PIN_ECHO 3
#define DISTANCE_SENSOR_MAX_DISTANCE_IN_CM 200
#define DISTANCE_SENSOR_MEASUREMENT_INTERVAL_IN_MS 250

#define BUZZER_PIN 4
#define BUZZER_MAX_DISTANCE_TO_ALERT_IN_CM 100

#define LED_PIN 5
#define LED_NUM_PIXELS 8
#define LED_DELAYVAL 500


unsigned long heartbeatPreviousMillis = 0;
unsigned long distanceSensorMeasurementPreviousMillis = 0;

float currentDistanceInCm = 0;

Buzzer buzzer(BUZZER_PIN);

UltraSonicDistanceSensor distanceSensor(
  DISTANCE_SENSOR_PIN_TRIG,
  DISTANCE_SENSOR_PIN_ECHO,
  DISTANCE_SENSOR_MAX_DISTANCE_IN_CM);

Adafruit_NeoPixel pixels(LED_NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando");

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();

  buzzer.alertInitialization();

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

  // atualização da distância
  if (currentMillis - distanceSensorMeasurementPreviousMillis >= DISTANCE_SENSOR_MEASUREMENT_INTERVAL_IN_MS) {
    distanceSensorMeasurementPreviousMillis = currentMillis;

    currentDistanceInCm = distanceSensor.measureDistanceCm();

    Serial.print("Medição: ");
    Serial.print(currentDistanceInCm, 2);
    Serial.println(" cm");

    if (currentDistanceInCm >= 0 && currentDistanceInCm < BUZZER_MAX_DISTANCE_TO_ALERT_IN_CM) {
      int buzzerAlertBeatIntervalInMs = getBuzzerAlertBeatIntervalInMsFromDistanceInCm(currentDistanceInCm);

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

  buzzer.synchronize(currentMillis);
}

int getBuzzerAlertBeatIntervalInMsFromDistanceInCm(float distanceInCm) {
  if (distanceInCm > 50) {
    return 1500;
  } else if (distanceInCm > 25) {
    return 1000;
  } else if (distanceInCm > 12.5) {
    return 500;
  } else {
    return 250;
  }
}
