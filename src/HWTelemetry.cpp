#include "HWTelemetry.h"

// 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31

// FE 01 00 03 30 5C 17 06 00 2F 30 01 00 F5 04 4B 00 FF FF 25 FF 22 FF FF FF FF FF FF FF FF 83 95
// FE 01 00 03 30 5C 17 06 00 64 00 02 00 00 00 ... Brake 100%, 0rpm
// FE 01 00 03 30 5C 17 06 00 1E 20 01 00 20 03 ... Forward, Throttle 30%, 8000rpm
// FE 01 00 03 30 5C 17 06 00 12 0F 01 00 DF 01 ... Reverse, Throttle -18%, 4800rpm

// Byte 0: Startbyte is always 0xFE
// Byte 01-07: First 4 bytes are maybe control bytes. A 0x01000330 marks a telemetry data package.
// Byte 9:  Throttle input value from receiver.
// Byte 10: Throttle output value from ESC. Maybe different to throttle input because of ESC settings. For example: max reverse force.
// Byte 11: Direction value. Is 0x00 if neutral, 0x01 if forward or reverse and 0x02 if brake.
// Byte 13 and 14: RPM value. Needs to be multiplicated with 10. Maybe needs another multiplicator if motor has more then 2 poles.
// Byte 15: Battery voltage. Needs to be divided with 10.
// Byte 19: ESC Temperature.
// Byte 21: Motor Temperature.
// Byte 30 and 31: Checksum.

HWTelemetryClass HWTelemetry;

HWTelemetryClass::HWTelemetryClass(void)
{

}

HWTelemetryClass::~HWTelemetryClass(void)
{

}

void HWTelemetryClass::begin(void)
{
    Serial.begin(115200);
    begin(Serial);
}

void HWTelemetryClass::begin(Stream &s)
{
    HWTelemetryStream = &s;
    while(HWTelemetryStream->available())
        HWTelemetryStream->read();
}

void HWTelemetryClass::processInput(void)
{
    // Check for available data
    uint16_t bytesAvailable = HWTelemetryStream->available();
    if(bytesAvailable > 0) {
        // Check for Hobbywing telemetry header
        uint8_t startByte = HWTelemetryStream->peek();
        if(startByte == HWT_PACKAGE_HEADER) {
            uint8_t buf[HWT_PACKAGE_SIZE];
            uint16_t len = HWTelemetryStream->readBytes(buf, HWT_PACKAGE_SIZE);
            // Check for Hobbywing telemetry data frame length
            if(len == HWT_PACKAGE_SIZE) {
                // Control bytes maybe are byte 1-4
                uint32_t ctrlBytes = 0;
                ctrlBytes |= buf[1];
                ctrlBytes |= buf[2] << 8;
                ctrlBytes |= buf[3] << 16;
                ctrlBytes |= buf[4] << 24;
                // Checksum bytes maybe are byte 30 and 31
                uint16_t checksum = 0;
                checksum |= buf[30];
                checksum |= buf[31] << 8;
                // Check for control bytes and checksum (CRC-16/MODBUS)
                if(ctrlBytes == 0x30030001 && checksum == calcCRC16(buf, HWT_PACKAGE_SIZE - 2, CRC16_MODBUS_POLYNOME, CRC16_MODBUS_INITIAL, CRC16_MODBUS_XOR_OUT, CRC16_MODBUS_REV_IN, CRC16_MODBUS_REV_OUT, CRC_YIELD_DISABLED)) {
                    throttleIn = buf[9];
                    throttleOut = buf[10];
                    direction = buf[11];
                    rpm = 0;
                    rpm |= buf[13];
                    rpm |= buf[14] << 8;
                    voltage = buf[15];
                    escTemp = buf[19];
                    motorTemp = buf[21];
                    if(currentCallback) currentCallback();
                } else {
                    throttleIn = 0;
                    throttleOut = 0;
                    direction = 0;
                    rpm = 0;
                }
            }
        } else {
            Serial.read();
        }
    }
}

uint8_t HWTelemetryClass::getThrottle(bool input)
{
    uint8_t throttle = throttleIn;
    if(input)
        throttle = throttleOut;

    return throttle;
}

uint8_t HWTelemetryClass::getDirection(void)
{
    return direction;
}

int16_t HWTelemetryClass::getRPM(void)
{
    return rpm * 10 / (motorPoles / 2);
}

float HWTelemetryClass::getVoltage(void)
{
    return voltage / 10.0;
}

uint8_t HWTelemetryClass::getESCTemperature(void)
{
    return escTemp;
}

uint8_t HWTelemetryClass::getMotorTemperature(void)
{
    return motorTemp;
}

float HWTelemetryClass::getSpeed(void)
{
    float speed = ((rpm / gearRatio) * wheelSize * PI * 60) / 100000.0;
    return speed / (motorPoles / 2);
}

void HWTelemetryClass::setMotorPoles(uint8_t poles)
{
    if(poles < 2 || poles > 100) {
        return;
    }
    if((poles % 2) != 0) {
        return;
    }
    motorPoles = poles;
}

void HWTelemetryClass::setGearRatio(float ratio)
{
    if(ratio < 0.01 || ratio > 100.0) {
        return;
    }
    gearRatio = ratio;
}

void HWTelemetryClass::setWheelSize(uint16_t size)
{
    if(size < 10 || size > 1000) {
        return;
    }
    wheelSize = size;
}