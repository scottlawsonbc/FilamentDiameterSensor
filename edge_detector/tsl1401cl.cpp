#include "tsl1401cl.h"
#include "digitalWriteFast.h"
#include "configuration.h"

// 'No operation' assembly instruction
#define NOP __asm__ __volatile__ ("nop\n\t")

// Configure the ADC prescaler for faster measurements:
const unsigned char PS_32  = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// Send a single clock pulse to the linear sensor array
inline void Clock()
{
    digitalWriteFast(CFG_CLOCK_PIN, HIGH);
    digitalWriteFast(CFG_CLOCK_PIN, LOW);
}

/* Initializes the TSL1401CL sensor and ADC
 * 
 * Clears indeterminant data from the sensor and prepares it for measurements.
 * Initializes the ADC for faster measurements while slightly reducing resolution.
 * 
 */
void TSL_Init()
{
    pinMode(CFG_CLOCK_PIN, OUTPUT);
    pinMode(CFG_SERIAL_PIN, OUTPUT);

    /* ADC prescalers: PS_16, PS_32, PS_64 or PS_128: */
    ADCSRA &= ~PS_128; /* remove bits set by arduino */
    ADCSRA |= PS_32;   /* PS_32 takes ~30 us instead of ~100 us */
    analogReference(DEFAULT);

    /* Set all IO pins low */
    digitalWriteFast(CFG_CLOCK_PIN, LOW);
    digitalWriteFast(CFG_SERIAL_PIN, LOW);

    /* Clock out indeterminant data on power up */
    for (uint16_t i = 0; i < CFG_PIXEL_COUNT+CFG_LOGIC_SETUP_CLOCKS; i++)
    {
        Clock();
    }

    /* Initialize an SI pulse and discard the data */
    digitalWriteFast(CFG_SERIAL_PIN, HIGH);
    Clock();
    digitalWriteFast(CFG_SERIAL_PIN, LOW);
    for (uint16_t i = 0; i < CFG_PIXEL_COUNT+CFG_LOGIC_SETUP_CLOCKS; i++)
    {
        Clock();
    }
}

/* Returns the raw pixel data from the TSL1401CL sensor
 *
 * Starts a new integration cycle and reads the sequential pixel data into the array.
 * Integration time can be increased by adding delays between clock cycles.
 * Measurement time can be reduced by reducing integration time and reducing stabilization time. 
 */
void TSL_MeasurePixels(int16_t x[])
{
    /* New measuring cycle is started once the logic setup pulses have been sent */
    for (uint16_t i = 0; i < CFG_LOGIC_SETUP_CLOCKS + CFG_PIXEL_COUNT; i++)
    {
        Clock();
    }

    /* Stop the current photodiode integration */
    digitalWriteFast(CFG_SERIAL_PIN, HIGH);
    Clock();
    digitalWriteFast(CFG_SERIAL_PIN, LOW);

    /* Read each pixel in the array */
    for (uint16_t i = 0; i < CFG_PIXEL_COUNT; i++)
    {
        delayMicroseconds(CFG_STABILIZATION_TIME_US); /* delay to stabilize the pixel data */
        x[i] = analogRead(CFG_ANALOG_INPUT_PIN);
        Clock();
    }

    digitalWriteFast(CFG_SERIAL_PIN, HIGH);
    Clock();
    digitalWriteFast(CFG_SERIAL_PIN, LOW);
}

