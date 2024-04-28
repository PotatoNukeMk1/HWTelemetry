#include <Arduino.h>
#include <wiring_private.h>
#include <HWTelemetry.h>

Uart Serial2(&sercom1, 5, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM1_Handler() {
  Serial2.IrqHandler();
}

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
  Serial.printf("Current: %.2fA\n", HWTelemetry.getCurrent());
  Serial.printf("ESC Temp: %d°C\n", HWTelemetry.getESCTemperature());
  Serial.printf("Motor Temp: %d°C\n\n", HWTelemetry.getMotorTemperature());
}


void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial2.begin(115200);
  Serial2.setTimeout(50);
  pinPeripheral(5, PIO_SERCOM);
  pinPeripheral(4, PIO_SERCOM);
  HWTelemetry.begin(Serial2);
  HWTelemetry.attach(hwtCallback);

  HWTelemetry.setMotorPoles(2);
  HWTelemetry.setGearRatio(6.9);
  HWTelemetry.setWheelSize(85);
}

void loop() {
  HWTelemetry.processInput();
}
