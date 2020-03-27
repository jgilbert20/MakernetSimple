#include "SAMD11_EncoderReader.h"

#if defined(ARDUINO)

//#pragma GCC optimize ("O3")
#pragma GCC optimize ("Ofast")

int16_t MakernetEncoderReader::getPosition()
{
  noInterrupts();
  int16_t r = _rotPosition;
  interrupts();
  return r;
}

boolean MakernetEncoderReader::hasChanged()
{
  if (  _lastReportedPosition == _rotPosition )
    return false;

  _lastReportedPosition = _rotPosition;
  return true;
}

// TODO: Implement
int16_t MakernetEncoderReader::getDetentPosition()
{

  return 0;
}

// Algorithim below based on a pattern by Paul Stoffregen. For some reason
// Paul's Encoder code doesn't work with ATSAMD11. Have to roll my own. Can't
// figure out why that library doesn't work :(

//inline __attribute__((always_inline))
void MakernetEncoderReader::update(boolean pin1, boolean pin2) {
  uint8_t s = _rotState & 3;
  if (pin1) s |= 4;
  if (pin2) s |= 8;
  _rotState = (s >> 2);
  switch (s) {
  case 1: case 7: case 8: case 14:
    _rotPosition++; break;
  case 2: case 4: case 11: case 13:
    _rotPosition--; break;
  case 3: case 12:
    _rotPosition += 2; break;
  case 6: case 9:
    _rotPosition -= 2; break;
  case 0: case 5: case 10: case 15:
    break;
  }
}

void MakernetEncoderReader::update(void) {
  update( digitalReadFast(_pin1), digitalReadFast(_pin2) );
}



MakernetEncoderReader::MakernetEncoderReader( int pin1, int pin2 )
{
  _pin1 = pin1;
  _pin2 = pin2;
}

void MakernetEncoderReader::begin()
{
  pinMode(_pin1, INPUT_PULLUP);
  pinMode(_pin2, INPUT_PULLUP);

  // Note: Population of the initial state is essential for the smoothness
  // of the decode! Do not omit this step!

  delayMicroseconds(2000);
  uint8_t s = 0;
  if ( digitalRead(_pin1) ) s |= 1;
  if ( digitalRead(_pin2) ) s |= 2;
  _rotState = s;
}


/* Encoder Library, for measuring quadrature encoded signals
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 * Copyright (c) 2011,2013 PJRC.COM, LLC - Paul Stoffregen <paul@pjrc.com>
 *
 * Version 1.2 - fix -2 bug in C-only code
 * Version 1.1 - expand to support boards with up to 60 interrupts
 * Version 1.0 - initial release
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#endif
