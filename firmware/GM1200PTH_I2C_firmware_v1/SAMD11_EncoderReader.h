#ifndef MAKERNETENCODERREADER_H
#define MAKERNETENCODERREADER_H

#if defined(ARDUINO)

// Note this whole library is a hack. Preferably use PJRC / Encoder.h
// for reading encoders since its much better written

// Needed for type definitions
#include <Arduino.h>

#if defined(__SAMD11D14AM__) or defined(__SAMD21G18A__)

// int digitalReadFast( uint32_t ulPin );

static inline __attribute__((always_inline))
int digitalReadFast( uint32_t ulPin )
{
    // Handle the case the pin isn't usable as PIO
    if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
    {
        return LOW ;
    }

    if ( (PORT->Group[g_APinDescription[ulPin].ulPort].IN.reg & (1ul << g_APinDescription[ulPin].ulPin)) != 0 )
    {
        return HIGH ;
    }

    return LOW ;
}

#else

#warning "MAKERNETENCODERREADER_H: Not implemented for non SAMD chips"
#define digitalReadFast(x) digitalRead(x)

#endif

// For some reason Paul's Encoder code doesn't work with ATSAMD11. Have to roll my own.
// Can't figure out why that library doesn't work :(

class MakernetEncoderReader {
public:
    void update();
    void update(boolean pin1, boolean pin2);
    void begin();
    int16_t getPosition();
    boolean hasChanged();
    int _pin1;
    int _pin2;
    int16_t getDetentPosition();
    MakernetEncoderReader( int pin1, int pin2 );
private:
    int16_t _lastReportedPosition = 0;
    volatile uint8_t _rotState = 0;
    volatile int32_t _rotPosition = 0;
};

#endif // SAMD

#endif // ARDUINO 
