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

#include <MakernetSimpleKnob.h>
#include <Wire.h>

int MakernetSimpleKnob::begin( int addr )
{
	_addr = addr;

	_settings.red = 0;
	_settings.green = 0;
	_settings.blue = 0;
	_settings.statusLed = 1;

	Wire.begin();

	Wire.beginTransmission(_addr); 
	Wire.write(MAKERNET_I2CCMD_ENCODER_RESET);              // sends one byte
	Wire.endTransmission(true);    // stop transmitting

	uint8_t recvSize = Wire.requestFrom(_addr, 1 ); 
	uint8_t statusCode = Wire.read();

	if( statusCode != MAKERNET_RESET_CONFIRM )
		return 0;

	return 1;
}

#define BUFF_SIZE 20

void MakernetSimpleKnob::update()
{
	uint8_t i2c_buffer[BUFF_SIZE];

	_oldPosition = _lastEncoderState.curPosition;
	_oldButton = _lastEncoderState.button;

	Wire.beginTransmission(_addr); 
	Wire.write(MAKERNET_I2CCMD_ENCODER_GET_STATE);              // sends one byte
	Wire.endTransmission(true);    // stop transmitting

	uint8_t recvSize = Wire.requestFrom(_addr, sizeof( MakernetEncoderState ) );  

	int pos = 0;
	while (Wire.available()) {
		if ( pos < BUFF_SIZE )
			i2c_buffer[pos++] = Wire.read();
	}

	struct MakernetEncoderState *reply = (MakernetEncoderState *)(i2c_buffer);

	// Check if the packet is good..?

	memcpy( &_lastEncoderState, reply, sizeof(MakernetEncoderState));

	if ( not _hasFirstUpdate )
	{
		_oldPosition = _lastEncoderState.curPosition;
		_oldButton = _lastEncoderState.button;
		_hasFirstUpdate = true;
	}
}

void MakernetSimpleKnob::pushSettings()
{
	Wire.beginTransmission(_addr); // transmit to device #8
    Wire.write(MAKERNET_I2CCMD_ENCODER_STORE_SETTINGS);
    Wire.write( (uint8_t *)&_settings, sizeof( MakernetEncoderSettings ) );
    Wire.endTransmission(true);    // stop transmitting	
}

void MakernetSimpleKnob::setRGB( uint8_t red, uint8_t green,  uint8_t blue )
{
	_settings.red = red;
	_settings.blue = blue;
	_settings.green = green;

	pushSettings();
}

void MakernetSimpleKnob::setStatusLed( boolean l )
{
	_settings.statusLed = l ? 1 : 0;
	pushSettings();
}
