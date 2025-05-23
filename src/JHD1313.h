#ifndef I2CIP_JHD1313_H_
#define I2CIP_JHD1313_H_

#include <Arduino.h>
#include <I2CIP.h>

// // color define
// #define WHITE           0
// #define RED             1
// #define GREEN           2
// #define BLUE            3

// commands
#define JHD1313_CLEARDISPLAY (uint8_t)0x01
#define JHD1313_RETURNHOME (uint8_t)0x02
#define JHD1313_ENTRYMODESET (uint8_t)0x04
#define JHD1313_DISPLAYCONTROL (uint8_t)0x08
#define JHD1313_CURSORSHIFT (uint8_t)0x10
#define JHD1313_FUNCTIONSET (uint8_t)0x20
#define JHD1313_SETCGRAMADDR (uint8_t)0x40
#define JHD1313_SETDDRAMADDR (uint8_t)0x80

// flags for display entry mode
#define JHD1313_ENTRYRIGHT (uint8_t)0x00
#define JHD1313_ENTRYLEFT (uint8_t)0x02
#define JHD1313_ENTRYSHIFTINCREMENT (uint8_t)0x01
#define JHD1313_ENTRYSHIFTDECREMENT (uint8_t)0x00

// flags for display on/off control
#define JHD1313_DISPLAYON (uint8_t)0x04
#define JHD1313_DISPLAYOFF (uint8_t)0x00
#define JHD1313_CURSORON (uint8_t)0x02
#define JHD1313_CURSOROFF (uint8_t)0x00
#define JHD1313_BLINKON (uint8_t)0x01
#define JHD1313_BLINKOFF (uint8_t)0x00

// flags for display/cursor shift
#define JHD1313_DISPLAYMOVE (uint8_t)0x08
#define JHD1313_CURSORMOVE (uint8_t)0x00
#define JHD1313_MOVERIGHT (uint8_t)0x04
#define JHD1313_MOVELEFT (uint8_t)0x00

// flags for function set
#define JHD1313_8BITMODE (uint8_t)0x10
#define JHD1313_4BITMODE (uint8_t)0x00
#define JHD1313_2LINE (uint8_t)0x08
#define JHD1313_1LINE (uint8_t)0x00
#define JHD1313_5x10DOTS (uint8_t)0x04
#define JHD1313_5x8DOTS (uint8_t)0x00

// Registers
#define JHD1313_REG_MODE (uint8_t)0x80
#define JHD1313_REG_DATA (uint8_t)0x40
#define JHD1313_REG_RED (uint8_t)0x04
#define JHD1313_REG_GREEN (uint8_t)0x03
#define JHD1313_REG_BLUE (uint8_t)0x02
#define JHD1313_ROW1 (uint8_t)0x80
#define JHD1313_ROW2 (uint8_t)0xc0

// Device I2C Arress
 // 0x3E
#define I2CIP_JHD1313_ADDRESS     62
 // 0x62
#define JHD1313_ADDRESS_RGBLED     98
// #define RGB_ADDRESS_V5  (0x30)
#define JHD1313_ROWS 2
#define JHD1313_COLS 16
#define JHD1313_TXMAX JHD1313_ROWS * JHD1313_COLS

#define JHD1313_ENABLE_CURSOR true // Uncomment to enable cursor
#define JHD1313_ENABLE_BLINK true // Uncomment to enable cursor blinking

#define JHD1313_DELAY_MICROS 2000
#define I2CIP_JHD1313_BRIGHTNESS 0.65f // Keep it low for good contrast and readability

// const char i2cip_jhd1313_id_progmem[] PROGMEM = {"JHD1313"};

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} i2cip_jhd1313_args_t;

class JHD1313 : public I2CIP::Device, public I2CIP::OutputInterface<String, i2cip_jhd1313_args_t>, private Print {
  I2CIP_DEVICE_CLASS_BUNDLE(JHD1313);
  I2CIP_OUTPUT_USE_FAILSAFE(String, i2cip_jhd1313_args_t);
  I2CIP_OUTPUT_USE_TOSTRING(String, "%s", value.c_str());
  private:
    // JHD1313(i2cip_fqa_t fqa) : I2CIP::Device(fqa, i2cip_jhd1313_id_progmem, _id), I2CIP::OutputInterface<uint8_t*, size_t>((I2CIP::Device*)this) { }

    i2cip_errorlevel_t begin(bool setbus = true) override; // virtual Device::begin
    i2cip_errorlevel_t setCursor(const uint8_t& col, const uint8_t& row, bool setbus = false);
    i2cip_errorlevel_t setRGB(const uint8_t& r, const uint8_t& g, const uint8_t& b, bool setbus = false);
    
    JHD1313(i2cip_fqa_t fqa, const i2cip_id_t& id);

    virtual size_t write(uint8_t b) override;
  public:
    i2cip_errorlevel_t set(const String& buf, const i2cip_jhd1313_args_t& len);
    // i2cip_errorlevel_t set(const char* const& buf, const i2cip_jhd1313_args_t& len);
    static i2cip_errorlevel_t _begin(const i2cip_fqa_t& fqa, const bool setbus = false);
    static i2cip_errorlevel_t _setCursor(const i2cip_fqa_t& fqa, const uint8_t& col, const uint8_t& row, const bool setbus = false);
    static i2cip_errorlevel_t _setRGB(const i2cip_fqa_t& fqa, const uint8_t& r, const uint8_t& g, const uint8_t& b, bool setbus = false);

    static i2cip_jhd1313_args_t randomRGBLCD(void);
};

#endif