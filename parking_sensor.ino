// https://github.com/Martinsos/arduino-lib-hc-sr04
#include <HCSR04.h>

#define HEARTBEAT_INTERVAL 1000

#define DISTANCE_SENSOR_PIN_TRIG 2
#define DISTANCE_SENSOR_PIN_ECHO 3
#define DISTANCE_SENSOR_MAX_DISTANCE_IN_CM 200
#define DISTANCE_SENSOR_MEASUREMENT_INTERVAL_IN_MS 250

#define BUZZER_PIN 4
#define BUZZER_MAX_DISTANCE_TO_ALERT_IN_CM 100
#define BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS 100
#define BUZZER_ALERT_BEAT_SOUND_FREQUENCY 853
#define BUZZER_ALERT_BEAT_DURATION_IN_MS 100


unsigned long heartbeatPreviousMillis = 0;
unsigned long distanceSensorMeasurementPreviousMillis = 0;
unsigned long buzzerAlertBeatOnMillis = 0;
unsigned long buzzerAlertBeatOffMillis = 0;

float currentDistanceInCm = 0;
int buzzerAlertBeatIntervalInMs = 0;
bool buzzerAlertBeatOn = 0;

UltraSonicDistanceSensor distanceSensor(
  DISTANCE_SENSOR_PIN_TRIG,
  DISTANCE_SENSOR_PIN_ECHO,
  DISTANCE_SENSOR_MAX_DISTANCE_IN_CM);

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando");

  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 440, BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS);
  delay(BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS);
  tone(BUZZER_PIN, 493.88, BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS);
  delay(BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS);
  tone(BUZZER_PIN, 523.25, BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS);
  delay(BUZZER_INITIALIZATION_BEAT_DURATION_IN_MS);

  Serial.println("Inicialização finalizada");
}

void loop() {
  unsigned long currentMillis = millis();

  // heartbeat
  if (currentMillis - heartbeatPreviousMillis >= HEARTBEAT_INTERVAL) {
    heartbeatPreviousMillis = currentMillis;
    Serial.println("Heartbeat");
  }

  // atualização da distância
  if (currentMillis - distanceSensorMeasurementPreviousMillis >= 
    DISTANCE_SENSOR_MEASUREMENT_INTERVAL_IN_MS) {
    distanceSensorMeasurementPreviousMillis = currentMillis;

    currentDistanceInCm = distanceSensor.measureDistanceCm();

    Serial.print("Medição: ");
    Serial.print(currentDistanceInCm, 2);
    Serial.println(" cm");

    // alerta sonoro
    if (currentDistanceInCm >= 0 && currentDistanceInCm < BUZZER_MAX_DISTANCE_TO_ALERT_IN_CM) {
      buzzerAlertBeatIntervalInMs = getBuzzerAlertBeatIntervalInMsFromDistanceInCm(currentDistanceInCm);

      if (buzzerAlertBeatOn) {
        if (currentMillis - buzzerAlertBeatOnMillis >= BUZZER_ALERT_BEAT_DURATION_IN_MS) {
          buzzerAlertBeatOn = false;
          buzzerAlertBeatOffMillis = currentMillis;
          noTone(BUZZER_PIN);
        }
      } else {
        if (currentMillis - buzzerAlertBeatOffMillis >= buzzerAlertBeatIntervalInMs) {
          buzzerAlertBeatOn = true;
          buzzerAlertBeatOnMillis = currentMillis;
          tone(BUZZER_PIN, BUZZER_ALERT_BEAT_SOUND_FREQUENCY);
        }
      }
    }
    else {
      buzzerAlertBeatOn = false;
      buzzerAlertBeatOffMillis = currentMillis;
      noTone(BUZZER_PIN);
    }
  }
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


