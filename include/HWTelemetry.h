#ifndef HWTELEMETRY_h
#define HWTELEMETRY_h

#include <Arduino.h>
#include <CRC.h>

#define HWT_PACKAGE_HEADER 0xFE
#define HWT_PACKAGE_SIZE   32

#define HWT_DIRECTION_NEUTRAL  0x00
#define HWT_DIRECTION_THROTTLE 0x01
#define HWT_DIRECTION_BRAKE    0x02

class HWTelemetryClass
{
public:
    HWTelemetryClass(void);
    ~HWTelemetryClass(void);
    void begin(void);
    void begin(Stream &s);
    void processInput(void);
    uint8_t getThrottle(bool input = false);
    uint8_t getDirection(void);
    int16_t getRPM(void);
    float getVoltage(void);
    float getCurrent(void);
    uint8_t getESCTemperature(void);
    uint8_t getMotorTemperature(void);
    float getSpeed(void);
    void setMotorPoles(uint8_t poles);
    void setGearRatio(float ratio);
    void setWheelSize(uint16_t size);
    void attach(void (*userDefinedCallback)(void)) {
        currentCallback = userDefinedCallback;
    }
    void detach(void) {
        attach(NULL);
    }
private:
    Stream *HWTelemetryStream;
    uint8_t throttleIn = 0;
    uint8_t throttleOut = 0;
    uint8_t direction = 0;
    int16_t rpm = 0;
    uint8_t voltage = 0;
    int16_t current = 0;
    uint8_t escTemp = 0;
    uint8_t motorTemp = 0;
    uint8_t speed = 0;
    uint8_t motorPoles = 2;
    float gearRatio = 0.01;
    uint8_t wheelSize = 10;
    void (*currentCallback)(void);
};

extern HWTelemetryClass HWTelemetry;

#endif