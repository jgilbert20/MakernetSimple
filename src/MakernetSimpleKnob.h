/********************************************************
 **
 **  MakernetSimpleKnob.h
 **
 **  Part of the Makernet framework by Jeremy Gilbert
 **
 **  License: GPL 3
 **  See footer for copyright and license details.
 **
 ********************************************************/

#if !defined(MAKERNETSIMPLEKNOB_H)
#define MAKERNETSIMPLEKNOB_H

#include <Arduino.h>

#define MAKERNET_I2CCMD_ENCODER_SET_I2C_ADDRESS 0xF8


// Must be a NINE byte message exactly, with the 9th byte the new I2C address
// the first 8 bytes are "MAKERNET"
// No response is given, but the light is blinked in a particular pattern


#define MAKERNET_I2CCMD_ENCODER_GET_STATE 0x20
#define MAKERNET_I2CCMD_ENCODER_GET_SETTINGS 0x21
#define MAKERNET_I2CCMD_ENCODER_STORE_SETTINGS 0x22

#define MAKERNET_I2CADDR_ENCODER 0x30



struct MakernetEncoderState {
	long curPosition;
	boolean button;
};

struct MakernetEncoderSettings {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t statusLed;
};


class MakernetSimpleKnob {
public:
	void begin( int addr );
	void update();

	inline long position() { return _lastEncoderState.curPosition; };
	inline boolean button() { return _lastEncoderState.button; };
	inline boolean pressed() {
		return ( _hasFirstUpdate and
		         _oldButton == false and
		         _lastEncoderState.button == true);
	};
	inline boolean released() {
		return ( _hasFirstUpdate and
		         _oldButton == true and
		         _lastEncoderState.button == false);
	};

	inline signed long positionDelta() {
		return ( _lastEncoderState.curPosition - _oldPosition );
	};


private:
	uint8_t _addr;
	MakernetEncoderState _lastEncoderState;
	boolean _hasFirstUpdate = false;
	long _oldPosition;
	boolean _oldButton;
};







#endif
