// This sketch can be used to reprogram a MakernetSimple Knob's I2C
// address. This allows you to have multiple Knobs on the same I2C bus
// and create more complex control services. You can run this sketch on
// nearly any Arduino IDE compatable hardware such as a Arduino Uno,
// Teensy, Feather etc.
//
// To start, locate the I2C address that the board is currently using
// using the i2c_scanner sketch available in the library's example folder.
// You will probably want to disconnect any other devices you have connected
// before doing this procedure to avoid getting confused.
//
// Once you have located your current I2C address, put it in the information
// below along with the new address you'd like to use. Note that acceptable
// I2C addresses must be between 10-120.
//
// Now program and load this sketch. When you run the sketch, you should
// see the Knob rapidly blink to indicate a new address has been set
// in flash memory. Now you must "reboot" the Knob by removing power
// and then restoring it, which will cause it to appear on the bus under the
// new address.

#define CURRENT_ADDRESS 0x30
#define NEW_ADDRESS 0x34

// Connections:
//
// Knob VCC - Arduino Board VCC (usually 3.3V or 5V)
// Knob SDA - Arduino Board SDA (I2C bus) (sometimes A4)
// Knob SCL - Arduino Board SDA (I2C bus) (sometimes A5)
// Knob GND - Project GND
//
// Note: The Knob board has a pull up already for SDA & SCL. No
// other hardware or resisters are needed for this example to work
//
// This sketch is licensed under GPL3. Additional licensing options
// are available by contacting Jeremy Gilbert.

#include <Wire.h>
#include <MakernetSimpleKnob.h>

void setup() {

  Serial.begin( 115200 );

  delay(3000);

  Serial.println( "Attempting to reprogram device to new I2C address" );

  uint8_t reprogramCode[] = {
    'M', 'A', 'K', 'E', 'R', 'N', 'E', 'T'  };

  Wire.begin();
  Wire.beginTransmission(CURRENT_ADDRESS);
  Wire.write( (uint8_t) MAKERNET_I2CCMD_ENCODER_SET_I2C_ADDRESS );
  Wire.write( reprogramCode, 8 );
  Wire.write( (uint8_t) NEW_ADDRESS );
  Wire.endTransmission();    // stop transmitting

  Serial.println( "If successful, your Makernet device should be blinking rapidly" );
}

void loop() {
  // put your main code here, to run repeatedly:

}
