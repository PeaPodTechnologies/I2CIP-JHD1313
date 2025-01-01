#include "JHD1313.h"

#include <debug.h>

#ifndef JHD1313_ENABLE_CURSOR
#define JHD1313_ENABLE_CURSOR false
#endif

#ifndef JHD1313_ENABLE_BLINK
#define JHD1313_ENABLE_BLINK false
#endif

#define JHD1313_FAILSAFE_ARGS { .r = 0x3F, .g = 0xFF, .b = 0x3F } // White

I2CIP_DEVICE_INIT_STATIC_ID(JHD1313);
I2CIP_OUTPUT_INIT_FAILSAFE(JHD1313, String, String("[I2CIP] HELLO!"), i2cip_jhd1313_args_t, JHD1313_FAILSAFE_ARGS);

// bool JHD1313::_failsafe_set = false;
// char JHD1313::_failsafe[JHD1313_TXMAX] = {"I2CIP\nBLANK"};
// uint16_t JHD1313::_failsafe_b = JHD1313_FAILSAFE_ARGS;

using namespace I2CIP;

JHD1313::JHD1313(i2cip_fqa_t fqa, const i2cip_id_t& id) : Device(fqa, id), OutputInterface<String, i2cip_jhd1313_args_t>((Device*)this), Print(), fqa_rgb(createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), JHD1313_ADDRESS_RGBLED)) { }
// JHD1313::JHD1313(i2cip_fqa_t fqa, const i2cip_id_t& id) : Device(fqa, id), OutputInterface<const char*, i2cip_jhd1313_args_t>((Device*)this), fqa_rgb(createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), JHD1313_ADDRESS_RGBLED)) { }

using namespace I2CIP;

i2cip_errorlevel_t JHD1313::begin(uint8_t cols, uint8_t rows, bool setbus) {
  if(cols > JHD1313_COLS || rows > JHD1313_ROWS) {
    return I2CIP_ERR_SOFT;
  }

  i2cip_errorlevel_t errlev; // uint8_t c = 0;
  // do{
  //   c++;
    errlev = writeRegister(JHD1313_REG_MODE, JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE), setbus); // & c == 0); // Only the first time
    // delayMicroseconds(c == 0 ? 4500 : 150);
  // } while(c < 3); // Don't know why exactly but we ought to do this 3 times
  I2CIP_ERR_BREAK(errlev);
    delayMicroseconds(4500);

  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE), false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(150);

  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE), false);
  I2CIP_ERR_BREAK(errlev);

  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_FUNCTIONSET | (JHD1313_ROWS == 2 ? JHD1313_2LINE : JHD1313_1LINE), false);
  I2CIP_ERR_BREAK(errlev);

  // DISPLAY
  // errlev = writeRegister(JHD1313_REG_MODE, JHD1313_DISPLAYCONTROL | JHD1313_DISPLAYON | (JHD1313_ENABLE_CURSOR ? JHD1313_CURSORON : JHD1313_CURSOROFF) | (JHD1313_ENABLE_BLINK ? JHD1313_BLINKON : JHD1313_BLINKOFF), false);
  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_DISPLAYCONTROL | JHD1313_DISPLAYON | JHD1313_CURSORON, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // // CLEAR
  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_CLEARDISPLAY, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // ENTRY MODE L TO R
  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_ENTRYMODESET | JHD1313_ENTRYLEFT | JHD1313_ENTRYSHIFTDECREMENT, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_RETURNHOME, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(JHD1313_DELAY_MICROS);

  // RGB
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

i2cip_errorlevel_t JHD1313::setCursor(uint8_t col, uint8_t row, bool setbus) {
  // if(!this->initialized) return I2CIP_ERR_SOFT;
  if(col > JHD1313_COLS || row > JHD1313_ROWS) {
    return I2CIP_ERR_SOFT;
  }
  uint8_t b = (row == 0 ? (col | JHD1313_ROW1) : (col | (JHD1313_ROW2)));
  return writeRegister(JHD1313_REG_MODE, b, setbus);
}

i2cip_errorlevel_t JHD1313::setRGB(uint8_t r, uint8_t g, uint8_t b, bool setbus) {
  // if(!this->initialized) return I2CIP_ERR_SOFT;
  i2cip_errorlevel_t errlev = Device::writeRegister(fqa_rgb, JHD1313_REG_RED, r, setbus);
  I2CIP_ERR_BREAK(errlev);
  errlev = Device::writeRegister(fqa_rgb, JHD1313_REG_GREEN, g, false);
  I2CIP_ERR_BREAK(errlev);
  return Device::writeRegister(fqa_rgb, JHD1313_REG_BLUE, b, false);
}

i2cip_errorlevel_t JHD1313::set(const String& buf_args, const i2cip_jhd1313_args_t& args) {
  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;
  if(!this->initialized) {
    delayMicroseconds(50000); // Wakeup delay
    errlev = begin(JHD1313_COLS, JHD1313_ROWS, false);
    I2CIP_ERR_BREAK(errlev);

    #ifdef I2CIP_DEBUG_SERIAL
      DEBUG_DELAY();
      I2CIP_DEBUG_SERIAL.println(F("[JHD1313 | SET] INIT PASS"));
      DEBUG_DELAY();
    #endif
    this->initialized = true; // Comment out to always begin
    delayMicroseconds(JHD1313_DELAY_MICROS);
  }
  // Clear
  errlev = writeRegister(JHD1313_REG_MODE, JHD1313_CLEARDISPLAY, false);
  if(errlev != I2CIP_ERR_NONE) { this->initialized = false; return errlev; }
  delayMicroseconds(JHD1313_DELAY_MICROS);

  errlev = setRGB(args.r, args.g, args.b, false);
  if(errlev != I2CIP_ERR_NONE) { this->initialized = false; return errlev; }

  errlev = setCursor(0, 0, false);
  if(errlev != I2CIP_ERR_NONE) { this->initialized = false; return errlev; }
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.println(F("[JHD1313 | SET] SCREEN CLEAR; RGB SET; CURSOR RESET; PASS"));
    DEBUG_DELAY();
  #endif
  delayMicroseconds(JHD1313_DELAY_MICROS);

  String buf = String("[I2CIP] HELLO :D\n") + fqaToString(this->fqa); // hardcode failsafe
  // String buf = String(buf_args.c_str()); // Copy
  // if(buf_stream.write('\n') != 1) { return I2CIP_ERR_SOFT; }
  size_t len = buf_args.length(); // strlen(buf);
  if(len > 0) buf = buf_args;
  if(len > JHD1313_TXMAX) {
    len = JHD1313_TXMAX;
  }

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("[JHD1313 | SET] PRINT ARGS '"));
    I2CIP_DEBUG_SERIAL.print(buf);
    I2CIP_DEBUG_SERIAL.println(F("'"));
    DEBUG_DELAY();
  #endif
  
  return print(buf) > 0 ? I2CIP_ERR_NONE : I2CIP_ERR_SOFT;

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
  size_t r = 0;
  switch(b) {
    case '\0':
      return 1; // NOP here is OK
    case '\n':
      r = (setCursor(0, 1, false) == I2CIP_ERR_NONE ? 1 : 0); // Hard-coded Line 2 Set
      break;
    default:
      r = (writeRegister(JHD1313_REG_DATA, b, false) == I2CIP_ERR_NONE ? 1 : 0);
      break;
  }
  delayMicroseconds(JHD1313_DELAY_MICROS);
  return r;
}