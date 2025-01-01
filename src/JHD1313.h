#ifndef I2CIP_PCA9632_H_
#define I2CIP_PCA9632_H_

#include <Arduino.h>
#include <I2CIP.h>

// // color define
// #define WHITE           0
// #define RED             1
// #define GREEN           2
// #define BLUE            3

// commands
#define PCA9632_CLEARDISPLAY (uint8_t)0x01
#define PCA9632_RETURNHOME (uint8_t)0x02
#define PCA9632_ENTRYMODESET (uint8_t)0x04
#define PCA9632_DISPLAYCONTROL (uint8_t)0x08
#define PCA9632_CURSORSHIFT (uint8_t)0x10
#define PCA9632_FUNCTIONSET (uint8_t)0x20
#define PCA9632_SETCGRAMADDR (uint8_t)0x40
#define PCA9632_SETDDRAMADDR (uint8_t)0x80

// flags for display entry mode
#define PCA9632_ENTRYRIGHT (uint8_t)0x00
#define PCA9632_ENTRYLEFT (uint8_t)0x02
#define PCA9632_ENTRYSHIFTINCREMENT (uint8_t)0x01
#define PCA9632_ENTRYSHIFTDECREMENT (uint8_t)0x00

// flags for display on/off control
#define PCA9632_DISPLAYON (uint8_t)0x04
#define PCA9632_DISPLAYOFF (uint8_t)0x00
#define PCA9632_CURSORON (uint8_t)0x02
#define PCA9632_CURSOROFF (uint8_t)0x00
#define PCA9632_BLINKON (uint8_t)0x01
#define PCA9632_BLINKOFF (uint8_t)0x00

// flags for display/cursor shift
#define PCA9632_DISPLAYMOVE (uint8_t)0x08
#define PCA9632_CURSORMOVE (uint8_t)0x00
#define PCA9632_MOVERIGHT (uint8_t)0x04
#define PCA9632_MOVELEFT (uint8_t)0x00

// flags for function set
#define PCA9632_8BITMODE (uint8_t)0x10
#define PCA9632_4BITMODE (uint8_t)0x00
#define PCA9632_2LINE (uint8_t)0x08
#define PCA9632_1LINE (uint8_t)0x00
#define PCA9632_5x10DOTS (uint8_t)0x04
#define PCA9632_5x8DOTS (uint8_t)0x00

// Registers
#define PCA9632_REG_MODE (uint8_t)0x80
#define PCA9632_REG_DATA (uint8_t)0x40
#define PCA9632_REG_RED (uint8_t)0x04
#define PCA9632_REG_GREEN (uint8_t)0x03
#define PCA9632_REG_BLUE (uint8_t)0x02
#define PCA9632_ROW1 (uint8_t)0x80
#define PCA9632_ROW2 (uint8_t)0xc0

// Device I2C Arress
#define I2CIP_PCA9632_ADDRESS     0x3e
#define PCA9632_ADDRESS_RGBLED     0x62
// #define RGB_ADDRESS_V5  (0x30)
#define PCA9632_ROWS 2
#define PCA9632_COLS 16
#define PCA9632_TXMAX PCA9632_ROWS * PCA9632_COLS

#define PCA9632_ENABLE_CURSOR true // Uncomment to enable cursor
#define PCA9632_ENABLE_BLINK true // Uncomment to enable cursor blinking

#define PCA9632_DELAY_MICROS 2000

// const char i2cip_pca9632_id_progmem[] PROGMEM = {"PCA9632"};

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} i2cip_pca9632_args_t;

class PCA9632 : public I2CIP::Device, public I2CIP::OutputInterface<String, i2cip_pca9632_args_t>, private Print {
  I2CIP_DEVICE_CLASS_BUNDLE(PCA9632);
  I2CIP_OUTPUT_USE_FAILSAFE(String, i2cip_pca9632_args_t);
  private:
    // PCA9632(i2cip_fqa_t fqa) : I2CIP::Device(fqa, i2cip_pca9632_id_progmem, _id), I2CIP::OutputInterface<uint8_t*, size_t>((I2CIP::Device*)this) { }
    bool initialized = false;

    const i2cip_fqa_t fqa_rgb;

    i2cip_errorlevel_t begin(uint8_t cols, uint8_t rows, bool setbus = false);
    i2cip_errorlevel_t setCursor(uint8_t col, uint8_t row, bool setbus = false);
    i2cip_errorlevel_t setRGB(uint8_t r, uint8_t g, uint8_t b, bool setbus = false);
    
    PCA9632(i2cip_fqa_t fqa, const i2cip_id_t& id);

    virtual size_t write(uint8_t b) override;
  public:

    i2cip_errorlevel_t set(const String& buf, const i2cip_pca9632_args_t& len);
    // i2cip_errorlevel_t set(const char* const& buf, const i2cip_pca9632_args_t& len);
};

#endif