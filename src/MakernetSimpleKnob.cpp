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

void MakernetSimpleKnob::begin( int addr )
{
	_addr = addr;
	Wire.begin();
}

#define BUFF_SIZE 20

void MakernetSimpleKnob::update()
{
	uint8_t i2c_buffer[BUFF_SIZE];

	_oldPosition = _lastEncoderState.curPosition;
	_oldButton = _lastEncoderState.button;

	Wire.beginTransmission(9); // transmit to device #8
	Wire.write(MAKERNET_I2CCMD_ENCODER_GET_STATE);              // sends one byte
	Wire.endTransmission(true);    // stop transmitting

	uint8_t recvSize = Wire.requestFrom(9, sizeof( MakernetEncoderState ) );    // request 6 bytes from slave device #8

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

