#include "JHD1313.h"

#ifndef I2CIP_H_
#error "I2CIP must be in I2CIP-JHD1313/libs, or, adjacent to I2CIP-JHD1313 together in $PWD/libs"
#else

// #include <debug.h>

#ifndef JHD1313_ENABLE_CURSOR
#define JHD1313_ENABLE_CURSOR false
#endif

#ifndef JHD1313_ENABLE_BLINK
#define JHD1313_ENABLE_BLINK false
#endif

#define JHD1313_FAILSAFE_ARGS { .r = 0xFF, .g = 0xFF, .b = 0xFF } // White
#define JHD1313_WRITE_RETRIES 3 // Should be enough

I2CIP_DEVICE_INIT_STATIC_ID(JHD1313);
I2CIP_OUTPUT_INIT_FAILSAFE(JHD1313, String, "", i2cip_jhd1313_args_t, JHD1313_FAILSAFE_ARGS); // Default to just updating RGB

// bool JHD1313::_failsafe_set = false;
// char JHD1313::_failsafe[JHD1313_TXMAX] = {"I2CIP\nBLANK"};
// uint16_t JHD1313::_failsafe_b = JHD1313_FAILSAFE_ARGS;

using namespace I2CIP;

void JHD1313::parseJSONArgs(I2CIP::i2cip_args_io_t& argsDest, JsonVariant argsA, JsonVariant argsS, JsonVariant argsB) {
  // Case: String (char[4] cast to uint32_t)
  if(argsB.is<int>()) {
    int rgb = argsB.as<int>();
    if(rgb >= 0 && rgb <= 0xFFFFFF) {
      argsDest.b = new i2cip_jhd1313_args_t{ .r = (uint8_t)((rgb >> 16) & 0xFF), .g = (uint8_t)((rgb >> 8) & 0xFF), .b = (uint8_t)(rgb & 0xFF) };
    }
  }
  if(!argsS.isNull()) {
    // This works for any argsS, even null
    argsDest.s = new String(argsS.as<String>());
  }
}

void JHD1313::deleteArgs(I2CIP::i2cip_args_io_t& args) {
  delete((String*)args.s);
  delete((i2cip_jhd1313_args_t*)args.b);
}

JHD1313::JHD1313(i2cip_fqa_t fqa, const i2cip_id_t& id) : Device(fqa, id), OutputInterface<String, i2cip_jhd1313_args_t>((Device*)this), Print() { }
// JHD1313::JHD1313(i2cip_fqa_t fqa, const i2cip_id_t& id) : Device(fqa, id), OutputInterface<const char*, i2cip_jhd1313_args_t>((Device*)this), fqa_rgb(createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), JHD1313_ADDRESS_RGBLED)) { }

using namespace I2CIP;

i2cip_errorlevel_t JHD1313::begin(bool setbus) { return JHD1313::_begin(this->fqa, setbus); }
i2cip_errorlevel_t JHD1313::_begin(const i2cip_fqa_t& fqa, bool setbus) {
  // Serial.println("LCD BEGIN");
  delayMicroseconds(50000); // Wakeup delay

  i2cip_errorlevel_t errlev; // uint8_t c = 0;
  // do{
  //   c++;
    errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE)), setbus); // & c == 0); // Only the first time
    // delayMicroseconds(c == 0 ? 4500 : 150);
  // } while(c < 3); // Don't know why exactly but we ought to do this 3 times
  I2CIP_ERR_BREAK(errlev);
    delayMicroseconds(4500);

  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE)), false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(150);

  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE)), false);
  I2CIP_ERR_BREAK(errlev);

  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE)), false);
  I2CIP_ERR_BREAK(errlev);

  // DISPLAY
  // errlev = writeRegister(JHD1313_REG_MODE, JHD1313_DISPLAYCONTROL | JHD1313_DISPLAYON | (JHD1313_ENABLE_CURSOR ? JHD1313_CURSORON : JHD1313_CURSOROFF) | (JHD1313_ENABLE_BLINK ? JHD1313_BLINKON : JHD1313_BLINKOFF), false);
  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_DISPLAYCONTROL | JHD1313_DISPLAYON | JHD1313_CURSORON), false);
  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_DISPLAYCONTROL | JHD1313_DISPLAYON | JHD1313_CURSOROFF), false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // // CLEAR
  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)JHD1313_CLEARDISPLAY, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // ENTRY MODE L TO R
  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)(JHD1313_ENTRYMODESET | JHD1313_ENTRYLEFT | JHD1313_ENTRYSHIFTDECREMENT), false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  errlev = Device::writeRegister(fqa, (uint8_t)JHD1313_REG_MODE, (uint8_t)JHD1313_RETURNHOME, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // RGB
  i2cip_fqa_t fqa_rgb = createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), JHD1313_ADDRESS_RGBLED);
  errlev = Device::writeRegister(fqa_rgb, (uint8_t)0x0, (uint8_t)0x0, false); // Backlight
  I2CIP_ERR_BREAK(errlev);
  // set LEDs controllable by both PWM and GRPPWM registers
  errlev = Device::writeRegister(fqa_rgb, (uint8_t)0x8, (uint8_t)0xFF, false);
  I2CIP_ERR_BREAK(errlev);
  // set MODE2 values
  // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
  return Device::writeRegister(fqa_rgb, (uint8_t)0x1, (uint8_t)0x20, false);
  // return errlev;
}

i2cip_errorlevel_t JHD1313::setCursor(const uint8_t& col, const uint8_t& row, bool setbus) { return this->_setCursor(this->fqa, col, row, setbus); }
i2cip_errorlevel_t JHD1313::_setCursor(const i2cip_fqa_t& fqa, const uint8_t& col, const uint8_t& row, bool setbus) {
  // if(!this->initialized) return I2CIP_ERR_SOFT;
  if(col > JHD1313_COLS || row > JHD1313_ROWS) {
    return I2CIP_ERR_SOFT;
  }
  uint8_t b = (row == 0 ? (col | JHD1313_ROW1) : (col | (JHD1313_ROW2)));
  return Device::writeRegister(fqa, JHD1313_REG_MODE, b, setbus);
}

i2cip_errorlevel_t JHD1313::setRGB(const uint8_t& r, const uint8_t& g, const uint8_t& b, bool setbus) { return this->_setRGB(this->fqa, r, g, b, setbus); }
i2cip_errorlevel_t JHD1313::_setRGB(const i2cip_fqa_t& fqa, const uint8_t& r, const uint8_t& g, const uint8_t& b, bool setbus) {
  // if(!this->initialized) return I2CIP_ERR_SOFT;
  i2cip_fqa_t fqa_rgb = createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), JHD1313_ADDRESS_RGBLED);
  i2cip_errorlevel_t errlev = Device::writeRegister(fqa_rgb, JHD1313_REG_RED, r, setbus);
  I2CIP_ERR_BREAK(errlev);
  errlev = Device::writeRegister(fqa_rgb, JHD1313_REG_GREEN, g, false);
  I2CIP_ERR_BREAK(errlev);
  return Device::writeRegister(fqa_rgb, JHD1313_REG_BLUE, b, false);
}

i2cip_errorlevel_t JHD1313::set(const String& _buf, const i2cip_jhd1313_args_t& args) {
  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;
  // if(!this->ready) {
  //   errlev = this->begin();
  //   I2CIP_ERR_BREAK(errlev);
  // }
  if(!this->ready) {
    return I2CIP_ERR_SOFT;
  }

  // SET RGB
  errlev = setRGB(args.r, args.g, args.b, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.println(F("[JHD1313 | SET] SCREEN CLEAR; RGB SET"));
    DEBUG_DELAY();
  #endif


  // if(buf_stream.write('\n') != 1) { return I2CIP_ERR_SOFT; }
  size_t len = _buf.length(); // strlen(buf);
  if(len == 0 || _buf.equals(this->getValue())) { return I2CIP_ERR_NONE; } // RGB only; either it's already been written or there's nothing to write

  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_CLEARDISPLAY, true);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // Write buffer
  errlev = setCursor(0, 0, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);
  
  // Copy buffer
  String buf = String();
  for(size_t i = 0; i < len; i++) {
    buf += _buf.charAt(i);
  }

  // Pad newline with spaces to force clear
  uint8_t nl = buf.indexOf('\n');
  if(nl != -1 && nl < JHD1313_COLS) {
    uint8_t pad = JHD1313_COLS - nl;
    String temp = buf.substring(nl);
    buf = buf.substring(0, nl);
    for(uint8_t i = 0; i < pad; i++) {
      buf += ' ';
    }
    buf += temp;
  }
  if(len > JHD1313_TXMAX) {
    len = JHD1313_TXMAX;
  } else if(len < JHD1313_TXMAX) {
    while(buf.length() < JHD1313_TXMAX) {
      buf += ' '; // This will clear the rest of the screen instead of calling clearDisplay
    }
  }

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("[JHD1313 | SET] PRINT ARGS["));
    I2CIP_DEBUG_SERIAL.print(len);
    I2CIP_DEBUG_SERIAL.print(F("] '"));
    I2CIP_DEBUG_SERIAL.print(buf);
    I2CIP_DEBUG_SERIAL.println(F("'"));
    DEBUG_DELAY();
  #endif
  
  // return print(buf) > 0 ? I2CIP_ERR_NONE : (MUX::pingMUX(fqa) ? I2CIP_ERR_SOFT : I2CIP_ERR_HARD);
  return print(buf) == buf.length() ? I2CIP_ERR_NONE : (MUX::pingMUX(fqa) ? I2CIP_ERR_SOFT : I2CIP_ERR_HARD);

  // uint8_t s = 0; // row shift
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.print(F("[JHD1313 | SET] BUFFER '"));
  //   I2CIP_DEBUG_SERIAL.print(buf);
  //   I2CIP_DEBUG_SERIAL.print(F("' -> '"));
  // #endif
  // for(size_t i = 0; i < len; i++) {
  //   if(buf.charAt(i) == '\n') { // [i] == '\n') {
  //     #ifdef I2CIP_DEBUG_SERIAL
  //       I2CIP_DEBUG_SERIAL.print("\\n");
  //     #endif
  //     s++; if(s >= JHD1313_ROWS) { // increment row shifter
  //       return errlev;
  //     }

  //     // Write and clear line
  //     // errlev = writeRegister(JHD1313_REG_DATA, (uint8_t*)(&_buf[last]), (size_t)JHD1313_COLS, false);
  //     // I2CIP_ERR_BREAK(errlev);

  //     // Set cursor
  //     errlev = setCursor(0, s, false);
  //     I2CIP_ERR_BREAK(errlev);
      
  //     // last = i + 1; // Set last newline index +1 (next print start index)
  //     continue;
  //   } else {
  //     #ifdef I2CIP_DEBUG_SERIAL
  //       I2CIP_DEBUG_SERIAL.print(buf.charAt(i)); // [i]);
  //     #endif
  //     errlev = (write((uint8_t)buf.charAt(i)) == 1) ? I2CIP_ERR_NONE : I2CIP_ERR_SOFT;
  //     if(errlev != I2CIP_ERR_NONE) {
  //       #ifdef I2CIP_DEBUG_SERIAL
  //         I2CIP_DEBUG_SERIAL.println(F("' FAIL"));
  //         DEBUG_DELAY();
  //       #endif
  //       return errlev;
  //     }
  //     delayMicroseconds(200);
  //   }
  //   // _buf[i-s] = buf.charAt(i); //[i];
  // }
  // // errlev = writeRegister(JHD1313_REG_DATA, (uint8_t*)(&_buf[last]), JHD1313_TXMAX - last, false); // Write all bytes (clears display)
  // // I2CIP_ERR_BREAK(errlev);
  // #ifdef I2CIP_DEBUG_SERIAL
  //   I2CIP_DEBUG_SERIAL.println(F("' PASS"));
  //   DEBUG_DELAY();
  // #endif

  // errlev = setRGB(args.r, args.b, args.g);
  // I2CIP_ERR_BREAK(errlev);
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.println(F("[JHD1313 | SET] RGB PASS"));
  //   DEBUG_DELAY();
  // #endif

  // // Enable display
  // return writeRegister(JHD1313_REG_MODE, JHD1313_DISPLAYCONTROL | JHD1313_DISPLAYON, false);
  // I2CIP_ERR_BREAK(errlev);
}

size_t JHD1313::write(uint8_t b) {
  size_t r = 0; uint8_t c = 0;
  switch(b) {
    case '\0':
    case '\r':
    case '\t':
      return 1; // NOP here is OK
    case '\n':
      while(r == 0 && c < 10) { // Many retries here is OK
        c++;
        r = (setCursor(0, 1, false) == I2CIP_ERR_NONE ? 1 : 0);
        delayMicroseconds(JHD1313_DELAY_MICROS);
      }
      break;
    default:
      while(r == 0 && c < JHD1313_WRITE_RETRIES) { // Fewer retries; multiples
        c++;
        r = (writeRegister(JHD1313_REG_DATA, b, false) == I2CIP_ERR_NONE ? 1 : 0);
        delayMicroseconds(JHD1313_DELAY_MICROS);
      }
      break;
  }
  return r;
}

i2cip_jhd1313_args_t JHD1313::randomRGBLCD(void) {
  uint32_t lcdrgb = random(0, 0xFFFFFF); // Random color
  // Serial.print("Random RGB: 0x"); Serial.println(lcdrgb, HEX);
  uint8_t red = ((lcdrgb >> 16) & 0xFF); uint8_t green = ((lcdrgb >> 8) & 0xFF); uint8_t blue = (lcdrgb & 0xFF); // Extract RGB

  // float brightness = (float)lcdrgb / 0xFFFFFF; // Normal Brightness
  float brightness = (red / (float)0xFF) * 0.299f + (green / (float)0xFF) * 0.587f + (blue / (float)0xFF) * 0.114f; // Relative Spectral Luminance
  float _scale = I2CIP_JHD1313_BRIGHTNESS / brightness; // Raw scalings

  float _red = _scale * red / 255.f; float _green = _scale * green / 255.f; float _blue = _scale * blue / 255.f; // Normalize
  float scale = 1.f / max(1.f, max(_red, max(_green, _blue))); // Adjusted scaling

  red = (uint8_t)min(255.f, _red * scale * 255);
  green = (uint8_t)min(255.f, _green * scale * 255);
  blue = (uint8_t)min(255.f, _blue * scale * 255);
  // Serial.print("LCD RGB: 0x");
  // if(red < 0x10) Serial.print('0');
  // Serial.print(red, HEX);
  // if(green < 0x10) Serial.print('0');
  // Serial.print(green, HEX);
  // if(blue < 0x10) Serial.print('0');
  // Serial.println(blue, HEX);
  i2cip_jhd1313_args_t lcdargs = { .r = red, .g = green, .b = blue };
  return lcdargs;
}

#endif