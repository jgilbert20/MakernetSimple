// This is a simple example of how to use a MakernetSimple Knob.
// It should run on nearly any Arduino compatable board but has been
// explicitly tested on the Feather M0, Teensy and Uno.
// 
// This example will print messages to the serial port when the Knob
// is rotated and pressed. It will also change the colors of the Knob's
// RGB backlight when you press the button.
//
// Wiring connections:
//
// Knob VCC - Arduino Board Logic VCC (usually 3.3V or 5V)
// Knob SDA - Arduino Board SDA (I2C bus) (sometimes A4)
// Knob SCL - Arduino Board SDA (I2C bus) (sometimes A5)
// Knob GND - Project GND
//
// Note: The Knob board has a pull up already for SDA & SCL so no
// other hardware is needed for this example to work
//
// This sketch is licensed under GPL3. Additional licensing options
// are available by contacting Jeremy Gilbert.

#include <MakernetSimpleKnob.h>

// Declare a knob object to represent the physical Knob
MakernetSimpleKnob knob;

void setup() {

  Serial.begin( 115200 );
  Serial.print( "Example Makernet Knob tutorial\n");

  delay(500);
  
  // Initiate connection to Knob on its default I2C address
  int status = knob.begin( 0x30 );

  if ( status != 1 ) {
    Serial.print( "No device found - check your I2C address!" );
    while (1);
  }

  // Make the knob glow red
  knob.setRGB( 255, 0, 0 );
}

void loop() {
  // Refresh Knob state over the I2C bus
  knob.update();

  // If the button was released...
  if ( knob.released() ) {
    Serial.println( "Button released" );
    // Make the knob glow red
    knob.setRGB( 255, 0, 0 );
    // Turn off the blue status LED on the front of the Knob
    knob.setStatusLed( 0 );
  }

  // If the button was pressed...
  if ( knob.pressed() ) {
    Serial.println( "Button pressed" );
    // Make the knob glow WHITE
    knob.setRGB( 255, 255, 255 );
    // Turn on the blue status LED on the front of the Knob
    knob.setStatusLed( 1 );
  }

  if ( knob.positionDelta() != 0 ) {
    Serial.print( "Button moved delta:" );
    Serial.print(  knob.positionDelta() );
    Serial.print( "  absolute position=" );
    Serial.print( knob.position() );
    Serial.println();
  }
}
