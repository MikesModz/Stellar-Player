
#ifndef TFT_H_
#define TFT_H_

#include <stdbool.h>
#include <stdint.h>
#include "integer.h"

#define swap(a, b) { SHORT t = a; a = b; b = t; }

#define TFT_RESET_PIN				GPIO_PIN_7
#define TFT_COMMAND_PIN				GPIO_PIN_6
#define TFT_CS_PIN					GPIO_PIN_5

#define TFT_CS_HIGH					ROM_GPIOPinWrite(GPIO_PORTB_BASE, TFT_CS_PIN, TFT_CS_PIN);
#define TFT_CS_LOW					ROM_GPIOPinWrite(GPIO_PORTB_BASE, TFT_CS_PIN, 0);

#define TFT_COMMAND_HIGH			ROM_GPIOPinWrite(GPIO_PORTA_BASE, TFT_COMMAND_PIN, TFT_COMMAND_PIN);
#define TFT_COMMAND_LOW				ROM_GPIOPinWrite(GPIO_PORTA_BASE, TFT_COMMAND_PIN, 0);

#define TFT_RESET_HIGH				ROM_GPIOPinWrite(GPIO_PORTA_BASE, TFT_RESET_PIN, TFT_RESET_PIN);
#define TFT_RESET_LOW				ROM_GPIOPinWrite(GPIO_PORTA_BASE, TFT_RESET_PIN, 0);

#define _width    128
#define _height   160

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
#define Color565(r,g,b) (USHORT)((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3)

#define ST7735_NOP				0x00
#define ST7735_SWRESET			0x01
#define ST7735_RDDID			0x04
#define ST7735_RDDST			0x09

#define ST7735_SLPIN			0x10
#define ST7735_SLPOUT			0x11
#define ST7735_PTLON			0x12
#define ST7735_NORON			0x13

#define ST7735_INVOFF			0x20
#define ST7735_INVON			0x21
#define ST7735_DISPOFF			0x28
#define ST7735_DISPON			0x29
#define ST7735_CASET			0x2A
#define ST7735_RASET			0x2B
#define ST7735_RAMWR			0x2C
#define ST7735_RAMRD			0x2E

#define ST7735_PTLAR			0x30
#define ST7735_COLMOD			0x3A
#define ST7735_MADCTL			0x36

#define ST7735_FRMCTR1			0xB1
#define ST7735_FRMCTR2			0xB2
#define ST7735_FRMCTR3			0xB3
#define ST7735_INVCTR			0xB4
#define ST7735_DISSET5			0xB6

#define ST7735_PWCTR1			0xC0
#define ST7735_PWCTR2			0xC1
#define ST7735_PWCTR3			0xC2
#define ST7735_PWCTR4			0xC3
#define ST7735_PWCTR5			0xC4
#define ST7735_VMCTR1			0xC5

#define ST7735_RDID1			0xDA
#define ST7735_RDID2			0xDB
#define ST7735_RDID3			0xDC
#define ST7735_RDID4			0xDD

#define ST7735_PWCTR6			0xFC

#define ST7735_GMCTRP1			0xE0
#define ST7735_GMCTRN1			0xE1

#define DELAY					0x80

#define ST7735_BLACK			0x0000
#define ST7735_BLUE				0x001F
#define ST7735_RED				0xF800
#define ST7735_GREEN			0x07E0
#define ST7735_CYAN				0x07FF
#define ST7735_MAGENTA			0xF81F
#define ST7735_YELLOW			0xFFE0
#define ST7735_WHITE			0xFFFF

#define BITMAP_BUFFPIXEL 		30

typedef struct __attribute__((__packed__)) mtagBITMAPFILEHEADER
{
    WORD	bfType;				// Specifies the file type	(2)
    DWORD	bfSize;				// Specifies the size in bytes of the bitmap file (4)
    WORD	bfReserved1;		// Reserved; must be 0 (2)
    WORD	bfReserved2;		// Reserved; must be 0 (2)
    DWORD	bOffBits;			// Species the offset in bytes from the bitmapfileheader to the bitmap bits (4)
} mBITMAPFILEHEADER;

typedef struct mtagBITMAPINFOHEADER
{
    DWORD	biSize;				// Specifies the number of bytes required by the struct
    LONG	biWidth;			// Specifies width in pixels
    LONG	biHeight;			// Species height in pixels
    WORD	biPlanes;			// Specifies the number of color planes, must be 1
    WORD	biBitCount;			// Specifies the number of bit per pixel
    DWORD	biCompression;		// Specifies the type of compression
    DWORD	biSizeImage;		// Size of image in bytes
    LONG	biXPelsPerMeter;	// Number of pixels per meter in x axis
    LONG	biYPelsPerMeter;	// Number of pixels per meter in y axis
    DWORD	biClrUsed;			// Number of colors used by th ebitmap
    DWORD	biClrImportant;		// Number of colors that are important
} mBITMAPINFOHEADER;

void InitialiseDisplayTFT(void);
void ShowDemoTFT(void);
void fill_screen_tft(USHORT color);
void fill_rect_tft(SHORT x, SHORT y, SHORT w, SHORT h,USHORT color);
void draw_char_tft(SHORT x, SHORT y, UCHAR c,USHORT color, USHORT bg);
void draw_line_tft(SHORT x0, SHORT y0, SHORT x1, SHORT y1, USHORT color);
void draw_rect_tft(SHORT x, SHORT y, SHORT w, SHORT h, USHORT color);
void print_tft(const char str[]);
void set_cursor_tft(SHORT x, SHORT y);
void set_text_color_tft(USHORT c, USHORT bg);
void set_text_wrap_tft(UCHAR w);
void my_delay_tft(USHORT ms);
void set_addr_window_tft(UCHAR x0, UCHAR y0, UCHAR x1, UCHAR y1);
void spi_stream_pixel_tft(USHORT colour);
void draw_pixel_tft(SHORT x, SHORT y, USHORT color);
void LoadBitmapFileTFT(UCHAR *bitmapFileName, UCHAR x, UCHAR y);
void CenterTextTFT(char *string, UINT y_pos, USHORT f_col, USHORT b_col);
void UpdateFileListBox(uint16_t current, uint16_t last);
void incrementMenu(uint16_t *current, uint16_t *last, uint16_t totalFiles);
void decrementMenu(uint16_t *current, uint16_t *last);
void UpdateVolumeDisplays(uint8_t left, uint8_t right);

#endif /* TFT_H_ */
