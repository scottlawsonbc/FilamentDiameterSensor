#include <Arduino.h>
#include <Wire.h>

/* Connect SDA to PA7 of the STM32F3 (you MUST use a 5V-3.3V level converter!) */
/* Connect SCK to PA6 of the STM32F3 (you MUST use a 5V-3.3V level converter!) */
#define I2C_SLAVE_ADDRESS               (0x11)
#define COM_ENABLE_SENSOR               (0x01U)
#define COM_DISABLE_SENSOR              (0x02U)
#define COM_REQUEST_MEASUREMENT         (0x03U)

void setup()
{
    /* Initialize I2C communication as a Master */
    Wire.begin();
    Serial.begin(9600);
    /* Instruct the sensor to turn on */
    EnableSensor();
}

void loop()
{
    uint16_t diameter_UM = RequestFilamentDiameter_UM();
    Serial.print("Diameter: "); Serial.println(diameter_UM);
    delay(1000);
}

/* Tells the STM32F3 to enable the lasers and take measurements */
void EnableSensor()
{
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(COM_ENABLE_SENSOR);
    Wire.endTransmission();
}

/* Tells the STM32F3 to disable the lasers and stop taking measurements */
void DisableSensor()
{
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(COM_DISABLE_SENSOR);
    Wire.endTransmission();
}

/* Requests a measurement from the STM32F3 and returns the filament diameter in microns */
uint16_t RequestFilamentDiameter_UM()
{
    /* Request a filament diameter measurement from the STM32F3 */
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);   /* Transmit start condition */
    Wire.write(COM_REQUEST_MEASUREMENT); /* Transmit command byte    */
    Wire.endTransmission();                      /* Transmit stop condition  */

    /* Request two bytes of data from STM32F3 */
    Wire.requestFrom(I2C_SLAVE_ADDRESS, 2);
    uint16_t diameter_UM = 0;
    diameter_UM |= ((uint16_t)Wire.read()) << 8; /* Upper bits are transmitted first    */
    diameter_UM |= ((uint16_t)Wire.read()) << 0; /* Lower bits are transmitted second   */
    return diameter_UM;                          /* Return filament diameter in microns */
}