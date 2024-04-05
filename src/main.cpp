#include <Arduino.h>
#include <HWTelemetry.h>

void hwtCallback() {
  Serial.printf("Throttle in: %d%%\n", HWTelemetry.getThrottle(true));
  Serial.printf("Throttle out: %d%%\n", HWTelemetry.getThrottle());
  Serial.printf("Direction: ");
  if(HWTelemetry.getDirection() == HWT_DIRECTION_NEUTRAL) {
    Serial.printf("Neutral\n");
  } else if(HWTelemetry.getDirection() == HWT_DIRECTION_THROTTLE) {
    Serial.printf("Forward/Reverse\n");
  } else if(HWTelemetry.getDirection() == HWT_DIRECTION_BRAKE) {
    Serial.printf("Brake\n");
  }
  Serial.printf("RPM: %drpm\n", HWTelemetry.getRPM());
  Serial.printf("Speed: %.2fkm/h\n", HWTelemetry.getSpeed());
  Serial.printf("Voltage: %.2fV\n", HWTelemetry.getVoltage());
  Serial.printf("ESC Temp: %d°C\n", HWTelemetry.getESCTemperature());
  Serial.printf("Motor Temp: %d°C\n\n", HWTelemetry.getMotorTemperature());
}


void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial1.begin(115200);
  Serial1.setTimeout(50);
  Serial1.flush();
  HWTelemetry.begin(Serial1);
  HWTelemetry.attach(hwtCallback);

  HWTelemetry.setMotorPoles(2);
  HWTelemetry.setGearRatio(6.9);
  HWTelemetry.setWheelSize(85);
}

void loop() {
  HWTelemetry.processInput();
}
