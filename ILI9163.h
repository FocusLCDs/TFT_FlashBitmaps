/*
More register settings modified for the ILI9163 driver IC.
Adapted from the ST77535 register settings

Tested on ST Nucleo-L476RG microcontroller and created for 1.8" TFT 
Display:
https://focuslcds.com/product/1-8-tft-display-no-tp-e17rg11216lw6m300-n

=========================================================================
*/

#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>



#define ILI9163_TFTWIDTH_128   128 
#define ILI9163_TFTHEIGHT_160  160 

#define ST_CMD_DELAY      0x80    
#define SWRESET    0x01


#define SLPOUT     0x11
#define NORON      0x13
#define INVOFF     0x20
#define INVON      0x21
#define DISPOFF    0x28
#define DISPON     0x29
#define CASET      0x2A
#define RASET      0x2B
#define RAMWR      0x2C


#define TEOFF      0x34
#define TEON       0x35
#define MADCTL     0x36  
#define COLMOD     0x3A 

#define MADCTL_BGR 0x08 
#define MADCTL_MY  0x80  
#define MADCTL_MX  0x40	

#define FRMCTR1    0xB1
#define FRMCTR2    0xB2
#define FRMCTR3    0xB3
#define INVCTR     0xB4
#define DISSET5    0xB6

#define PWCTR1     0xC0
#define PWCTR2     0xC1
#define PWCTR3     0xC2
#define PWCTR4     0xC3
#define PWCTR5     0xC4
#define VMCTR1     0xC5

#define GMCTRP1    0xE0
#define GMCTRN1    0xE1



class ILI9163 : public Adafruit_SPITFT {
	public: 
	ILI9163(uint16_t w, uint16_t h, int8_t _CS, int8_t _DC,
      int8_t _MOSI, int8_t _SCLK, int8_t _RST = -1, int8_t _MISO = 12);
	  
	   ILI9163(uint16_t w, uint16_t h, int8_t CS, int8_t RS,
      int8_t RST = -1);
	  
    ILI9163(uint16_t w, uint16_t h, SPIClass *spiClass,
      int8_t CS, int8_t RS, int8_t RST = -1);


	void    initR();
	
	void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

    void enableDisplay(boolean enable);
    void enableTearing(boolean enable);

 protected:
    uint8_t _colstart = 0, ///< Some displays need this changed to offset
            _rowstart = 0, ///< Some displays need this changed to offset
            spiMode   = SPI_MODE0; 

    void    begin(uint32_t freq = 0);
    void    commonInit(const uint8_t *cmdList);
    void    displayInit(const uint8_t *addr);
    void    setColRowStart(int8_t col, int8_t row);

};
