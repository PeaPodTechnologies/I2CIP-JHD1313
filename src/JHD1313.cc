#include <PCA9632.h>

#ifndef PCA9632_ENABLE_CURSOR
#define PCA9632_ENABLE_CURSOR false
#endif

#ifndef PCA9632_ENABLE_BLINK
#define PCA9632_ENABLE_BLINK false
#endif

#define PCA9632_FAILSAFE_ARGS { .r = 0x3F, .g = 0xFF, .b = 0x3F } // White

I2CIP_DEVICE_INIT_STATIC_ID(PCA9632);
I2CIP_OUTPUT_INIT_FAILSAFE(PCA9632, String, String("[I2CIP] HELLO!"), i2cip_pca9632_args_t, PCA9632_FAILSAFE_ARGS);

// bool PCA9632::_failsafe_set = false;
// char PCA9632::_failsafe[PCA9632_TXMAX] = {"I2CIP\nBLANK"};
// uint16_t PCA9632::_failsafe_b = PCA9632_FAILSAFE_ARGS;

PCA9632::PCA9632(i2cip_fqa_t fqa, const i2cip_id_t& id) : I2CIP::Device(fqa, id), I2CIP::OutputInterface<String, i2cip_pca9632_args_t>((I2CIP::Device*)this), Print(), fqa_rgb(I2CIP::createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), PCA9632_ADDRESS_RGBLED)) { }
// PCA9632::PCA9632(i2cip_fqa_t fqa, const i2cip_id_t& id) : I2CIP::Device(fqa, id), I2CIP::OutputInterface<const char*, i2cip_pca9632_args_t>((I2CIP::Device*)this), fqa_rgb(I2CIP::createFQA(I2CIP_FQA_SEG_I2CBUS(fqa), I2CIP_FQA_SEG_MODULE(fqa), I2CIP_FQA_SEG_MUXBUS(fqa), PCA9632_ADDRESS_RGBLED)) { }

using namespace I2CIP;

i2cip_errorlevel_t PCA9632::begin(uint8_t cols, uint8_t rows, bool setbus) {
  if(cols > PCA9632_COLS || rows > PCA9632_ROWS) {
    return I2CIP_ERR_SOFT;
  }

  i2cip_errorlevel_t errlev; // uint8_t c = 0;
  // do{
  //   c++;
    errlev = writeRegister(PCA9632_REG_MODE, PCA9632_FUNCTIONSET | (PCA9632_ROWS == 2 ? PCA9632_2LINE : PCA9632_1LINE), setbus); // & c == 0); // Only the first time
    // delayMicroseconds(c == 0 ? 4500 : 150);
  // } while(c < 3); // Don't know why exactly but we ought to do this 3 times
  I2CIP_ERR_BREAK(errlev);
    delayMicroseconds(4500);

  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_FUNCTIONSET | (PCA9632_ROWS == 2 ? PCA9632_2LINE : PCA9632_1LINE), false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(150);

  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_FUNCTIONSET | (PCA9632_ROWS == 2 ? PCA9632_2LINE : PCA9632_1LINE), false);
  I2CIP_ERR_BREAK(errlev);

  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_FUNCTIONSET | (PCA9632_ROWS == 2 ? PCA9632_2LINE : PCA9632_1LINE), false);
  I2CIP_ERR_BREAK(errlev);

  // DISPLAY
  // errlev = writeRegister(PCA9632_REG_MODE, PCA9632_DISPLAYCONTROL | PCA9632_DISPLAYON | (PCA9632_ENABLE_CURSOR ? PCA9632_CURSORON : PCA9632_CURSOROFF) | (PCA9632_ENABLE_BLINK ? PCA9632_BLINKON : PCA9632_BLINKOFF), false);
  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_DISPLAYCONTROL | PCA9632_DISPLAYON | PCA9632_CURSORON, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(PCA9632_DELAY_MICROS);

  // // CLEAR
  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_CLEARDISPLAY, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(PCA9632_DELAY_MICROS);

  // ENTRY MODE L TO R
  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_ENTRYMODESET | PCA9632_ENTRYLEFT | PCA9632_ENTRYSHIFTDECREMENT, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(PCA9632_DELAY_MICROS);

  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_RETURNHOME, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(PCA9632_DELAY_MICROS);

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

i2cip_errorlevel_t PCA9632::setCursor(uint8_t col, uint8_t row, bool setbus) {
  if(!initialized) return I2CIP_ERR_SOFT;
  if(col > PCA9632_COLS || row > PCA9632_ROWS) {
    return I2CIP_ERR_SOFT;
  }
  uint8_t b = (row == 0 ? (col | PCA9632_ROW1) : (col | (PCA9632_ROW2)));
  return writeRegister(PCA9632_REG_MODE, b, setbus);
}

i2cip_errorlevel_t PCA9632::setRGB(uint8_t r, uint8_t g, uint8_t b, bool setbus) {
  if(!initialized) return I2CIP_ERR_SOFT;
  i2cip_errorlevel_t errlev = Device::writeRegister(fqa_rgb, PCA9632_REG_RED, r, setbus);
  I2CIP_ERR_BREAK(errlev);
  errlev = Device::writeRegister(fqa_rgb, PCA9632_REG_GREEN, g, false);
  I2CIP_ERR_BREAK(errlev);
  return Device::writeRegister(fqa_rgb, PCA9632_REG_BLUE, b, false);
}

i2cip_errorlevel_t PCA9632::set(const String& buf_args, const i2cip_pca9632_args_t& args) {
  i2cip_errorlevel_t errlev = I2CIP_ERR_NONE;
  if(!initialized) {
    delayMicroseconds(50000); // Wakeup delay
    errlev = begin(PCA9632_COLS, PCA9632_ROWS, false);
    I2CIP_ERR_BREAK(errlev);

    #ifdef I2CIP_DEBUG_SERIAL
      DEBUG_DELAY();
      I2CIP_DEBUG_SERIAL.println(F("[PCA9632 | SET] INIT PASS"));
      DEBUG_DELAY();
    #endif
    initialized = true;
    delayMicroseconds(PCA9632_DELAY_MICROS);
  }
  // Clear
  errlev = writeRegister(PCA9632_REG_MODE, PCA9632_CLEARDISPLAY, false);
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(PCA9632_DELAY_MICROS);

  errlev = setRGB(args.r, args.g, args.b, false);
  I2CIP_ERR_BREAK(errlev);

  errlev = setCursor(0, 0, false);
  I2CIP_ERR_BREAK(errlev);
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.println(F("[PCA9632 | SET] SCREEN CLEAR; RGB SET; CURSOR RESET; PASS"));
    DEBUG_DELAY();
  #endif
  delayMicroseconds(PCA9632_DELAY_MICROS);

  String buf = String("[I2CIP] HELLO!\n") + fqaToString(this->fqa); // hardcode failsafe
  // String buf = String(buf_args.c_str()); // Copy
  // if(buf_stream.write('\n') != 1) { return I2CIP_ERR_SOFT; }
  size_t len = buf.length(); // strlen(buf);
  if(len <= 0) return I2CIP_ERR_SOFT;
  if(len > PCA9632_TXMAX) {
    len = PCA9632_TXMAX;
  }

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("[PCA9632 | SET] PRINT ARGS '"));
    I2CIP_DEBUG_SERIAL.print(buf);
    I2CIP_DEBUG_SERIAL.println(F("'"));
    DEBUG_DELAY();
  #endif
  
  return print(buf) > 0 ? I2CIP_ERR_NONE : I2CIP_ERR_SOFT;

  // uint8_t s = 0; // row shift
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.print(F("[PCA9632 | SET] BUFFER '"));
  //   I2CIP_DEBUG_SERIAL.print(buf);
  //   I2CIP_DEBUG_SERIAL.print(F("' -> '"));
  // #endif
  // for(size_t i = 0; i < len; i++) {
  //   if(buf.charAt(i) == '\n') { // [i] == '\n') {
  //     #ifdef I2CIP_DEBUG_SERIAL
  //       I2CIP_DEBUG_SERIAL.print("\\n");
  //     #endif
  //     s++; if(s >= PCA9632_ROWS) { // increment row shifter
  //       return errlev;
  //     }

  //     // Write and clear line
  //     // errlev = writeRegister(PCA9632_REG_DATA, (uint8_t*)(&_buf[last]), (size_t)PCA9632_COLS, false);
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
  // // errlev = writeRegister(PCA9632_REG_DATA, (uint8_t*)(&_buf[last]), PCA9632_TXMAX - last, false); // Write all bytes (clears display)
  // // I2CIP_ERR_BREAK(errlev);
  // #ifdef I2CIP_DEBUG_SERIAL
  //   I2CIP_DEBUG_SERIAL.println(F("' PASS"));
  //   DEBUG_DELAY();
  // #endif

  // errlev = setRGB(args.r, args.b, args.g);
  // I2CIP_ERR_BREAK(errlev);
  // #ifdef I2CIP_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   I2CIP_DEBUG_SERIAL.println(F("[PCA9632 | SET] RGB PASS"));
  //   DEBUG_DELAY();
  // #endif

  // // Enable display
  // return writeRegister(PCA9632_REG_MODE, PCA9632_DISPLAYCONTROL | PCA9632_DISPLAYON, false);
  // I2CIP_ERR_BREAK(errlev);
}

size_t PCA9632::write(uint8_t b) {
  size_t r = 0;
  switch(b) {
    case '\0':
      return 1; // NOP here is OK
    case '\n':
      r = (setCursor(0, 1, false) == I2CIP_ERR_NONE ? 1 : 0); // Hard-coded Line 2 Set
      break;
    default:
      r = (writeRegister(PCA9632_REG_DATA, b, false) == I2CIP_ERR_NONE ? 1 : 0);
      break;
  }
  delayMicroseconds(PCA9632_DELAY_MICROS);
  return r;
}