
/*

Adapted register settings for the ILI9163 driver IC and tested on the ST Nucleo-L476RG

 **************************************************************************/

#include "ILI9163.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include "SPI.h"
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#define SPI_DEFAULT_FREQ 10000000 ///< Default SPI data clock frequency



ILI9163::ILI9163(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t mosi,
  int8_t sclk, int8_t rst, int8_t miso) : Adafruit_SPITFT(ILI9163_TFTWIDTH_128,
  ILI9163_TFTHEIGHT_160, cs, dc, mosi, sclk, rst, miso) {
}


ILI9163::ILI9163(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t rst) :
  Adafruit_SPITFT(ILI9163_TFTWIDTH_128, ILI9163_TFTHEIGHT_160, cs, dc, rst) {
}



ILI9163::ILI9163(uint16_t w, uint16_t h, SPIClass *spiClass,
  int8_t cs, int8_t dc, int8_t rst) : Adafruit_SPITFT(ILI9163_TFTWIDTH_128, ILI9163_TFTHEIGHT_160, spiClass, cs,
  dc, rst) {
}



static const uint8_t PROGMEM Rcmd1[] = {                      
    15,                             // 15 commands 
    SWRESET,   ST_CMD_DELAY, 		//  1: Software reset
      50,                           
    SLPOUT,    ST_CMD_DELAY, 		//  2: Out of sleep mode
      50,                           
    FRMCTR1, 3,             //  3: Framerate ctrl - normal mode
      0x08, 0x02, 0x02,             
    FRMCTR2, 3,             //  4: Framerate ctrl - idle mode
      0xB2, 0xB2, 0xB2,             
    FRMCTR3, 6,             //  5: Framerate - partial mode
      0x08, 0x02, 0x02,             
      0x08, 0x02, 0x02,            
    INVCTR,  1,             //  6: Display inversion ctrl
      0xB4,                        
    PWCTR1,  3,             //  7: Power control
      0x0A,
      0x02,                        
      0x84,                         
    PWCTR2,  1,             //  8: Power control
      0x02,                         
    PWCTR3,  2,             //  9: Power control
      0x02,                         
      0x00,                         
    PWCTR4,  2,             // 10: Power control
      0x8A,                         
      0x2A,                         
    PWCTR5,  2,             // 11: Power control
      0x8A, 0xEE,
    VMCTR1,  1,             // 12: Power control
      0x50,
    INVOFF,  0,              		// 13: Don't invert display, no args
    MADCTL,  1,             		// 14: Mem access ctl directions
      0x08,                         
    COLMOD,  1,              		// 15: 16-bit color mode
      0x05 },                       



  Rcmd2[] = {                       
    4,              	
	
    GMCTRP1, 15     ,       		//  1: Gamma Adjustments, pos. polarity 
      0x36, 0x29, 0x12, 0x22,       
      0x1C, 0x15, 0x42, 0xB7,       
      0x2F, 0x13, 0x12, 0x0A,
      0x11, 0x0B, 0x06, 
    GMCTRN1, 15      ,      		//  2: Gamma Adjustments, neg. polarity
      0x09, 0x16, 0x2D, 0x0D,       
      0x13, 0x15, 0x40, 0x48,       
      0x53, 0x0C, 0x1D, 0x25,
      0x2E, 0x34, 0x39, 

    NORON,     ST_CMD_DELAY, 		//  3: Normal display on
      10,                           
    DISPON,    ST_CMD_DELAY, 		//  4: Main screen turn on
      100 };                        



void ILI9163::displayInit(const uint8_t *addr) {

  uint8_t  numCommands, cmd, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    cmd = pgm_read_byte(addr++);         // Read command
    numArgs  = pgm_read_byte(addr++);    // Number of args to follow
    ms       = numArgs & ST_CMD_DELAY;   // If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;            // Mask out delay bit
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 255;     // If 255, delay for 255 ms
      delay(ms);
    }
  }
}


void ILI9163::begin(uint32_t freq) {
  if(!freq) {
    freq = SPI_DEFAULT_FREQ;
  }
  _freq = freq;

  invertOnCommand  = INVON;
  invertOffCommand = INVOFF;

  initSPI(freq, spiMode);
}

void ILI9163::commonInit(const uint8_t *cmdList) {
  begin();

  if(cmdList) {
    displayInit(cmdList);
  }
}


void ILI9163::setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
  uint16_t h) {
  x += _xstart;
  y += _ystart;
  uint32_t xa = ((uint32_t)x << 16) | (x+w-1);
  uint32_t ya = ((uint32_t)y << 16) | (y+h-1); 

  writeCommand(CASET); // Column addr set
  SPI_WRITE32(xa);

  writeCommand(RASET); // Row addr set
  SPI_WRITE32(ya);

  writeCommand(RAMWR); // write to RAM
}



void ILI9163::setColRowStart(int8_t col, int8_t row) {
  _colstart = col;
  _rowstart = row;
}

void ILI9163::enableDisplay(boolean enable) {
  sendCommand(enable ? DISPON : DISPOFF);
}



void ILI9163::enableTearing(boolean enable) {
  sendCommand(enable ? TEON : TEOFF);
}



void ILI9163::initR() {
	commonInit(Rcmd1);
	displayInit(Rcmd2);


	uint8_t madctl = 0;
	madctl = MADCTL_MX | MADCTL_MY | MADCTL_BGR;
    
	_height = ILI9163_TFTHEIGHT_160;
	_width  = ILI9163_TFTWIDTH_128;
     
    _xstart   = _colstart;
    _ystart   = _rowstart;
	 
	 sendCommand(MADCTL, &madctl, 1);
}

