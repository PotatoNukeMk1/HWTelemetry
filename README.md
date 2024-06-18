Its a working demo for samd21 but any other microcontroller with uart should work. Baudrate is 115200. A telemetry packet is sent every 100ms and is 32 bytes long.<br />

Every packet starts with a 0xFE and ends with 16 bit checksum (CRC-16/MODBUS). I don't know what the first 7 bytes are for. This bytes change if you use the app (with OTA Programmer) to load and save the config but if byte 1-4 are 0x01 0x00 0x03 and 0x30 its a telemetry packet.<br />

Byte 9 is the raw throttle input value from receiver.<br />
Byte 10 is the throttle output value from ESC. Its different from the raw throttle input because of the ESC settings.<br />
Byte 11 specifies the direction value. Is 0x00 is neutral, 0x01 if forward or reverse and 0x02 if brake.<br />
Byte 13 and 14 stores the RPM value. Needs to be multiplicated with 10 and the motor poles if your motor has more then 2 poles.<br />
Byte 15 is the battery voltage. Must be divided by 10.<br />
Byte 19 is ESC temperature.<br />
Byte 21 is Motor temperature.<br />

Because my ESC don't shows this value i cant determine which byte(s) are for current.<br />

Full package example<br /> 
`
// 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
// FE 01 00 03 30 5C 17 06 00 2F 30 01 00 F5 04 4B 00 FF FF 25 FF 22 FF FF FF FF FF FF FF FF 83 95
`
