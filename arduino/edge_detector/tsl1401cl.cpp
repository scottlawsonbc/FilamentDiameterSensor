#include "tsl1401cl.h"
#include "digitalWriteFast.h"
#include "configuration.h"

// 'No operation' assembly instruction
#define NOP __asm__ __volatile__ ("nop\n\t")

#if CFG_EXTERNAL_ADC
#define EXTERNAL_ADC_CLOCK() digitalWriteFast(CFG_EXTERNAL_ADC_WRITE_PIN, LOW); \
                             for (uint8_t i = CFG_EXTERNAL_ADC_NOPS; i > 0; --i) NOP; \
                             digitalWriteFast(CFG_EXTERNAL_ADC_WRITE_PIN, HIGH); \
                             for (uint8_t i = CFG_EXTERNAL_ADC_NOPS; i > 0; --i) NOP; \

#endif

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

    #if CFG_EXTERNAL_ADC
    /* Configure the DMA register as an input */
    pinMode(CFG_EXTERNAL_ADC_WRITE_PIN, OUTPUT);
    digitalWriteFast(CFG_EXTERNAL_ADC_WRITE_PIN, LOW);
    CFG_EXTERNAL_ADC_DDR = 0x00;
    CFG_EXTERNAL_ADC_DR  = 0x00;
    #else
    /* ADC prescalers: PS_16, PS_32, PS_64 or PS_128: */
    ADCSRA &= ~CFG_ADC_PS_128;   /* remove bits set by arduino */
    ADCSRA |= CFG_ADC_PRESCALER; /* PS_32 takes ~30 us instead of ~100 us */
    analogReference(DEFAULT);
    #endif

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

    /* Clear indeterminant data from the external ADC */
    #if CFG_EXTERNAL_ADC
    EXTERNAL_ADC_CLOCK();
    EXTERNAL_ADC_CLOCK();
    #endif

    /* Read each pixel in the array */
    for (uint16_t i = 0; i < CFG_PIXEL_COUNT; i++)
    {
        // If no delay is used, don't create parasitic delay by making a function call
        #if CFG_STABILIZATION_TIME_US > 0
        delayMicroseconds(CFG_STABILIZATION_TIME_US); /* delay to stabilize the pixel data */
        #endif

        #if !CFG_EXTERNAL_ADC
        /* Read the pixel data from the internal ADC */
        x[i] = analogRead(CFG_ANALOG_INPUT_PIN);
        #else
        /* Start a conversion on the ADC and read it on the DMA register */
        EXTERNAL_ADC_CLOCK();
        /* Read the pixel data from the DMA register */
        x[i] = CFG_EXTERNAL_ADC_IDR;
        #endif
        Clock();
    }
    digitalWriteFast(CFG_SERIAL_PIN, HIGH);
    Clock();
    digitalWriteFast(CFG_SERIAL_PIN, LOW);
}

