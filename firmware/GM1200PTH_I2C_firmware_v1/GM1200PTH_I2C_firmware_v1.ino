
#include <Wire.h>
#include <MakernetSimpleKnob.h>
#include "SAMD11_EncoderReader.h"
#include <Bounce2.h>

// Stuff here used for encoder hardware version 1.x

#define MAKERNET_PINOUT_ENCODER_A 10
#define MAKERNET_PINOUT_ENCODER_B 15
#define MAKERNET_PINOUT_ENCODER_SW 3

#define MAKERNET_PINOUT_ENCODER_RED 4
#define MAKERNET_PINOUT_ENCODER_GREEN 5
#define MAKERNET_PINOUT_ENCODER_BLUE 17

#define MAKERNET_PINOUT_ENCODER_STATUS_LED 16

#include <FlashStorage.h>

// Reserve a portion of flash memory to store an "int" variable
// and call it "my_flash_store".
FlashStorage(my_flash_store, int);



MakernetEncoderReader encoder( MAKERNET_PINOUT_ENCODER_A, MAKERNET_PINOUT_ENCODER_B );
Bounce rotaryButton;


struct MakernetEncoderSettings curSettings = { 0, 0, 0, 1 };

#if defined(__SAMD11D14AM__)
#define DEBUGSERIAL Serial1
#else
#define DEBUGSERIAL Serial
#endif


void configureFromSettings()
{

  if ( curSettings.red > 0 )
    analogWrite( MAKERNET_PINOUT_ENCODER_RED, 255 - curSettings.red );
  else
    pinMode( MAKERNET_PINOUT_ENCODER_RED, INPUT );

  if ( curSettings.green > 0 )
    analogWrite( MAKERNET_PINOUT_ENCODER_GREEN, 255 - curSettings.green );
  else
    pinMode( MAKERNET_PINOUT_ENCODER_GREEN, INPUT );

  if ( curSettings.blue > 0 )
    analogWrite( MAKERNET_PINOUT_ENCODER_BLUE, 255 - curSettings.blue );
  else
    pinMode( MAKERNET_PINOUT_ENCODER_BLUE, INPUT );

  digitalWrite ( MAKERNET_PINOUT_ENCODER_STATUS_LED, curSettings.statusLed );
}


void isr0(void)
{
  noInterrupts();

  // A bit of a hack that takes advantage of the fact that on the SAMD11 all pins are
  // on the same port. This ensures that both are read together.
  long portVal = PORT->Group[g_APinDescription[MAKERNET_PINOUT_ENCODER_A].ulPort].IN.reg;
  boolean pin1 = portVal & (1ul << g_APinDescription[MAKERNET_PINOUT_ENCODER_A].ulPin);
  boolean pin2 = portVal & (1ul << g_APinDescription[MAKERNET_PINOUT_ENCODER_B].ulPin);

  encoder.update(pin1, pin2);

  interrupts();
}

int i2c_address;

void setup() {

  // Read the content of "my_flash_store" 
  i2c_address = my_flash_store.read();

  if ( i2c_address < 10 or i2c_address > 120 )
    i2c_address = MAKERNET_I2CADDR_ENCODER;

  Wire.begin(i2c_address);                // join i2c bus
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event

  encoder.begin();
  pinMode( MAKERNET_PINOUT_ENCODER_SW, INPUT_PULLDOWN );
  rotaryButton.attach(MAKERNET_PINOUT_ENCODER_SW);

  pinMode( MAKERNET_PINOUT_ENCODER_STATUS_LED, OUTPUT );
  digitalWrite( MAKERNET_PINOUT_ENCODER_STATUS_LED, HIGH );

  pinMode( MAKERNET_PINOUT_ENCODER_RED, INPUT );
  pinMode( MAKERNET_PINOUT_ENCODER_GREEN, INPUT );
  pinMode( MAKERNET_PINOUT_ENCODER_BLUE, INPUT );

  attachInterrupt(encoder._pin1, isr0, CHANGE);
  attachInterrupt(encoder._pin2, isr0, CHANGE);

  Serial.begin(115200);
  Serial.print( "GM1200PTH - Starting on I2C Address [0x" );
  Serial.print( i2c_address, HEX );
  Serial.println( "]" );
}

void loop() {
  // Occasionally, the signal is too noisy and by the time the read occurs,
  // the signal has bounced more and it gets the old value.
  // Empirical testing has shown that a redundant check every 500 microseconds seems to
  // fix this issue and does not cause any harm.
  isr0();
  delayMicroseconds(500);

  if ( encoder.hasChanged() )
  {
    int16_t newPos = encoder.getPosition();
    DEBUGSERIAL.print( "@@@@ New encoder position: " );
    DEBUGSERIAL.println( newPos );
  }

  rotaryButton.update();

  if ( rotaryButton.rose() )
  {
    DEBUGSERIAL.println( "@@@@ Encoder pressed" );
  }

  if ( rotaryButton.fell() )
  {
    DEBUGSERIAL.println( "@@@@ Encoder released" );
  }
}

uint8_t i2c_buffer[200];
int replyDataSize = 0;

void receiveEvent(int howMany) {
  int pos = 0;
  replyDataSize = 0;
  while ( Wire.available())
    i2c_buffer[pos++] = Wire.read();

  if ( pos < 1 )
    return;

  uint8_t cmd = i2c_buffer[0];

  if( cmd == MAKERNET_I2CCMD_ENCODER_RESET )
  {
      i2c_buffer[0] = MAKERNET_RESET_CONFIRM;
      replyDataSize = 1;      
  }

  if ( cmd == MAKERNET_I2CCMD_ENCODER_GET_STATE )
  {
    struct MakernetEncoderState *reply = (MakernetEncoderState *)(i2c_buffer);
    // rotaryButton.update();
    reply->curPosition = encoder.getPosition();
    reply->button = rotaryButton.read();
    replyDataSize = sizeof( struct MakernetEncoderState );
    return;
  }

  if ( cmd == MAKERNET_I2CCMD_ENCODER_STORE_SETTINGS )
  {
    if ( pos < sizeof( MakernetEncoderSettings ) + 1 )
      return;

    memcpy( &curSettings, i2c_buffer + 1, sizeof( MakernetEncoderSettings ) );

    configureFromSettings();

    return;
  }

  if ( cmd == MAKERNET_I2CCMD_ENCODER_SET_I2C_ADDRESS )
  {
    DEBUGSERIAL.println( "@@@ Got possible request to assign new I2C addresss" );

    // Make sure the length was EXACTLY 10 bytes (1 cmd, 8 guard code, 1 new addr)
    if ( pos != 10 ) {
      DEBUGSERIAL.println( "@@@ Reprogram Addr: Message length wrong" );
      return;
    }

    // A guard code to ensure that this wasn't just a spurious bit of traffic
    uint8_t knockCode[] = { 'M', 'A', 'K', 'E', 'R', 'N', 'E', 'T' };

    if ( memcmp( knockCode, i2c_buffer + 1, 8 ) != 0 ) {
      DEBUGSERIAL.println( "@@@ Reprogram Addr: Knock wrong" );
      return;
    }
    int newAddr = i2c_buffer[9];

    if (  newAddr < 10 or newAddr > 120 )
      return;

    DEBUGSERIAL.print( "@@@ Accepting new I2C address: 0x" );
    DEBUGSERIAL.println( newAddr, HEX );

    my_flash_store.write(newAddr);

    delay(500);

    while (1)
    {
      digitalWrite( MAKERNET_PINOUT_ENCODER_STATUS_LED, HIGH );
      delay(100);
      digitalWrite( MAKERNET_PINOUT_ENCODER_STATUS_LED, LOW );
      delay(100);
    }
    
  }
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  if ( replyDataSize > 0 )
    Wire.write(i2c_buffer, replyDataSize);
  replyDataSize = 0;
}



