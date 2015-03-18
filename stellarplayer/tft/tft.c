#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "..\global.h"
#include "tft.h"
#include "glcdfont.h"
#include "integer.h"
#include "..\fatfs\ff.h"
#include "..\fatfs\diskio.h"
#include "string.h"
#include <stdio.h>
#include <ctype.h>

SHORT 	cursor_x, cursor_y;
USHORT 	textcolor, textbgcolor;
UCHAR 	wrap;

static void write_spi_tft(UCHAR b);
static void write_tft_command_tft(UCHAR b);
static void write_tft_data_tft(UCHAR b);
static void init_sequence_tft(void);
static void draw_fast_v_line_tft(SHORT x, SHORT y, SHORT h, USHORT color);
static void draw_fast_h_line_tft(SHORT x, SHORT y, SHORT w, USHORT color);
static void write_tft(UCHAR c);
static void TrimLeadingTrailingSpaces(char *string);

void InitialiseDisplayTFT(void)
{
	/* Enable peripherals */
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	SysCtlDelay(100);

	/* Set command and reset pin as outputs */
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, TFT_RESET_PIN|TFT_COMMAND_PIN);

	/* Enable CS pin as output */
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);

	/* Enable pin PB7 for SSI2 SSI2TX */
	ROM_GPIOPinConfigure(GPIO_PB7_SSI2TX);
	ROM_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_7);

	/* Enable pin PB6 for SSI2 SSI2RX */
	ROM_GPIOPinConfigure(GPIO_PB6_SSI2RX);
	ROM_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_6);

	/* Enable pin PB4 for SSI2 SSI2CLK */
	ROM_GPIOPinConfigure(GPIO_PB4_SSI2CLK);
	ROM_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4);

	/* Configure the SSI2 port to run at 15.0MHz */
	ROM_SSIConfigSetExpClk(SSI2_BASE,SysCtlClockGet(),SSI_FRF_MOTO_MODE_0,SSI_MODE_MASTER,15000000,8);
	ROM_SSIEnable(SSI2_BASE);

	/* Reset the display */
	TFT_RESET_HIGH;
	my_delay_tft(500);

	TFT_RESET_LOW;
	my_delay_tft(500);

	TFT_RESET_HIGH;
	my_delay_tft(500);

	/* Send command sequence */
	init_sequence_tft();

	cursor_y = cursor_x = 0;
	textcolor =  0xFFFF;
	textbgcolor = 0x0000;
	wrap = 1;
}

void UpdateVolumeDisplays(uint8_t left, uint8_t right)
{
	uint8_t f_val=0;

	f_val = left/4;
	fill_rect_tft(0,_height-5,f_val,5,Color565(255,0,0));
	fill_rect_tft(f_val,_height-5,64-f_val,5,Color565(0,0,0));

	f_val = right/4;
	fill_rect_tft(_width-f_val,_height-5,f_val,5,Color565(0,255,0));
	fill_rect_tft(64,_height-5,64-f_val,5,Color565(0,0,0));
}

void UpdateFileListBox(uint16_t current, uint16_t last)
{
	uint16_t i=0;
	uint16_t line=0;
	uint8_t tempBuffer[13];

	CenterTextTFT("Select a file",10,Color565(255,0,0),Color565(0,0,0),1);
	for(i=last-MAX_DISPLAY_ITEMS;i<last;i++)
	{
		getModFileNameNew(tempBuffer,i);
		if(i==current)
		{
			CenterTextTFT((char*)tempBuffer,(line*10)+30,Color565(255,0,0),Color565(0,0,255),1);
		}
		else
		{
			CenterTextTFT((char*)tempBuffer,(line*10)+30,Color565(255,255,255),Color565(0,0,0),1);
		}
		line++;
	}
}

void incrementMenu(uint16_t *current, uint16_t *last, uint16_t totalFiles)
{
	uint16_t l_current=0;
	uint16_t l_last=0;

	l_current = *current;
	l_last = *last;

	if(l_current<totalFiles-1)
	{
		l_current++;
		if(l_current>=l_last)
		{
			l_last++;
		}
	}
	else
	{
		l_current = 0;
		l_last = MAX_DISPLAY_ITEMS;
	}

	UpdateFileListBox(l_current,l_last);

	*current = l_current;
	*last = l_last;
}

void decrementMenu(uint16_t *current, uint16_t *last)
{
	uint16_t l_current=0;
	uint16_t l_last=0;

	l_current = *current;
	l_last = *last;

	if(l_current>FIRST_FILENAME)
	{
		l_current--;
		if(l_current<(l_last-MAX_DISPLAY_ITEMS))
		{
			l_last--;
		}
	}
	UpdateFileListBox(l_current,l_last);

	*current = l_current;
	*last = l_last;
}

void LoadBitmapFileTFT(UCHAR *bitmapFileName, UCHAR x, UCHAR y)
{
	FIL file;
	mBITMAPFILEHEADER bitmapFileHeader;
	mBITMAPINFOHEADER bitmapInfoHeader;
	UCHAR flip = true;
	int w, h, row, col;
	UCHAR r, g, b;
	UINT rowSize;
	UCHAR sdbuffer[3*BITMAP_BUFFPIXEL];
	UCHAR buffidx = sizeof(sdbuffer);
	UINT pos = 0;
	UINT read;

	// Check offset is valid
	if( (x >= _width) || (y >= _height) ) return;

	// Open file
	if( f_open(&file, (const TCHAR*)bitmapFileName, FA_READ) != FR_OK )
	{
		UARTprintf("Unable to open file %s\n", bitmapFileName);
		return;
	}

	// Read the bitmap file header
	f_read(&file,&bitmapFileHeader,sizeof(mBITMAPFILEHEADER),&read);

	// Verify that this is a bmp file by checking bitmap id
	if( bitmapFileHeader.bfType != 0x4D42 )
	{
		f_close(&file);
		return;
	}

	// Read the bitmap info header
	f_read(&file,&bitmapInfoHeader,sizeof(mBITMAPINFOHEADER),&read);

	// If planes = 1, 24 bit colour and no compression */
	if( bitmapInfoHeader.biPlanes == 1 &&
		bitmapInfoHeader.biBitCount == 24 &&
		bitmapInfoHeader.biCompression == 0 )
	{
		w = bitmapInfoHeader.biWidth;
		h = bitmapInfoHeader.biHeight;

		// BMP rows are padded (if needed) to 4-byte boundary
		rowSize = ((w*3)+3)&~3;

		// If bmpHeight is negative, image is in top-down order.
		// This is not common but has been observed in the wild.
		if(h < 0)
		{
			h = -h;
			flip = false;
		}

		// Crop area to be loaded
		if( (x+w-1) >= _width )  w = _width  - x;
		if( (y+h-1) >= _height ) h = _height - y;

		// Set TFT address window to clipped image bounds
		set_addr_window_tft(x, y, x+w-1, y+h-1);

		// For each row
		for(row=0;row<h;row++)
		{
			if(flip)
			{
				// Bitmap is stored bottom-to-top order (normal BMP)
				pos = bitmapFileHeader.bOffBits + (h - 1 - row) * rowSize;
			}
			else
			{
				// Bitmap is stored top-to-bottom
				pos = bitmapFileHeader.bOffBits + row * rowSize;
			}

			// Seek position in file
			f_lseek(&file,pos);

			// Force read of file
			buffidx = sizeof(sdbuffer);

			// For each pixel...
			for(col=0;col<w;col++)
			{
				// Time to read more pixel data?
				if(buffidx >= sizeof(sdbuffer))
				{
					// Read more
					f_read(&file,&sdbuffer,sizeof(sdbuffer),&read);

					// Reset index
					buffidx = 0;
				}

				// Convert pixel from BMP to TFT format, push to display
				b = sdbuffer[buffidx++];
				g = sdbuffer[buffidx++];
				r = sdbuffer[buffidx++];
				TFT_COMMAND_HIGH;
				spi_stream_pixel_tft(Color565(r,g,b));
			}
		}
	}

	// Close file
	f_close(&file);
}

void ShowDemoTFT(void)
{
	UINT w = 0;
	const char mystring[20] = "Hello";

	fill_screen_tft(Color565(0,0,0));
	fill_rect_tft(0,0,127,50,Color565(255,0,0));
	set_text_color_tft(Color565(255,255,255),Color565(255,00,00));
	set_cursor_tft(55,20);
	print_tft("red");
	fill_rect_tft(0,50,127,100,Color565(0,255,0));
	set_text_color_tft(Color565(255,255,255),Color565(0,255,00));
	set_cursor_tft(50,70);
	print_tft("green");
	fill_rect_tft(0,100,127,159,Color565(0,0,255));
	set_text_color_tft(Color565(255,255,255),Color565(0,00,255));
	set_cursor_tft(55,120);
	print_tft("blue");
	draw_rect_tft(5,5,118,150,Color565(255,255,255));
	my_delay_tft(2000);

	fill_screen_tft(Color565(0,0,0));

	unsigned char y1 = 0;
	for (y1=0; y1<160; y1+=8)
	{
		draw_line_tft(0, 0, 127, y1, Color565(255,0,0));
		draw_line_tft(0, 159, 127, y1, Color565(0,255,0));
		draw_line_tft(127, 0, 0, y1, Color565(0,0,255));
		draw_line_tft(127, 159, 0, y1, Color565(255,255,255));
	}
	my_delay_tft(2000);

	// CENTER TEXT
	fill_screen_tft(Color565(0,0,0));
	w = strlen(mystring)*6;
	if(w<=_width)
	{
		set_cursor_tft((_width-w)/2,_height-20);
		set_text_wrap_tft(0);
		set_text_color_tft(Color565(255,255,255),Color565(0,0,0));
		print_tft(mystring);
	}

	// TEXT
	fill_screen_tft(Color565(0,0,0));
	set_cursor_tft(0,0);
	set_text_wrap_tft(1);

	set_text_color_tft(Color565(255,255,255),Color565(0,0,255));
	print_tft("All available chars:\n\n");
	set_text_color_tft(Color565(200,200,255),Color565(50,50,50));
	unsigned char z;
	char ff[]="a";
	for (z=32; z<128; z++)
	{
		ff[0]=z;
		print_tft(ff);
	}
	my_delay_tft(2000);
}

void CenterTextTFT(char *string, UINT y_pos, USHORT f_col, USHORT b_col, UCHAR fill)
{
	UINT w = 0;

	TrimLeadingTrailingSpaces(string);
	w = strlen(string)*6;
	if(w<=_width)
	{
		if( fill ) fill_rect_tft(0,y_pos,_width,8,b_col);
		set_cursor_tft((_width-w)/2,y_pos);
		set_text_wrap_tft(0);
		set_text_color_tft(f_col,b_col);
		print_tft(string);
	}

}

void fill_screen_tft(USHORT color)
{
	fill_rect_tft(0, 0, _width, _height,color);
}

void draw_rect_tft(SHORT x, SHORT y, SHORT w, SHORT h, USHORT color)
{
	draw_fast_h_line_tft(x,y,w,color);
	draw_fast_h_line_tft(x,y+h-1,w,color);
	draw_fast_v_line_tft(x,y,h,color);
	draw_fast_v_line_tft(x+w-1,y,h,color);
}

void draw_line_tft(SHORT x0, SHORT y0, SHORT x1, SHORT y1, USHORT color)
{
	SHORT steep = abs(y1-y0) > abs(x1-x0);
	if( steep )
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if( x0 > x1 )
	{
		swap(x0,x1);
		swap(y0, y1);
	}

	SHORT dx, dy;
	dx = x1 - x0;
	dy = abs(y1-y0);

	SHORT err = dx / 2;
	SHORT ystep;

	if( y0 < y1 )
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for(; x0<=x1; x0++)
	{
		if( steep )
		{
			draw_pixel_tft(y0, x0, color);
		}
		else
		{
			draw_pixel_tft(x0, y0, color);
		}
		err -= dy;
		if( err < 0 )
		{
			y0 += ystep;
			err += dx;
		}
	}
}

void draw_char_tft(SHORT x, SHORT y, UCHAR c, USHORT color, USHORT bg)
{
	UCHAR i = 0;
	UCHAR j = 0;

	if( ( x >= _width ) ||		// Clip right
		( y >= _height ) ||		// Clip bottom
		( (x + 5 - 1) < 0 ) ||	// Clip left
		( (y + 8 - 1) < 0 ) )   // Clip top
	{
		return;
	}

	c=c-32;
	for( i=0; i<6; i++ )
	{
		UCHAR line;
		if( (i == 5) || (c>(128-32)) )
		{
			// All invalid characters will print as a space
			line = 0;
		}
		else
		{
			line = font[(c*5)+i];
		}

		set_addr_window_tft(x+i,y,x+i,y+7);
		TFT_COMMAND_HIGH;

		for( j = 0; j<8; j++ )
		{
			if( line & 0x1 )
			{
				write_spi_tft(color>>8);
				write_spi_tft(color&0xff);
			}
			else
			{
				write_spi_tft(bg>>8);
				write_spi_tft(bg&0xff);
			}
			line >>= 1;
		}
	}
}

void print_tft(const char str[])
{
	int x=0;
	while( str[x] )
	{
		write_tft(str[x]);
		x++;
	}
}

void set_cursor_tft(SHORT x, SHORT y)
{
	cursor_x = x;
	cursor_y = y;
}

void set_text_color_tft(USHORT c, USHORT b)
{
	textcolor = c;
	textbgcolor = b;
}

void set_text_wrap_tft(UCHAR w)
{
	wrap = w;
}

void fill_rect_tft(SHORT x, SHORT y, SHORT w, SHORT h, USHORT color)
{
	// rudimentary clipping (draw_char_tft w/big text requires this)
	if( (x >= _width) || ( y >= _height) ) return;
	if( (x + w - 1) >= _width )  w = _width  - x;
	if( (y + h - 1) >= _height ) h = _height - y;

	set_addr_window_tft(x,y,x+w-1,y+h-1);

	TFT_COMMAND_HIGH;

	for(y=h;y>0;y--)
	{
		for(x=w;x>0;x--)
		{
			spi_stream_pixel_tft(color);
		}
	}
}

void spi_stream_pixel_tft(USHORT color)
{
	write_spi_tft(color>>8);
	write_spi_tft(color&0xff);
}

void draw_pixel_tft(SHORT x, SHORT y, USHORT color)
{
	if( (x < 0) ||(x >= _width) || (y < 0) || (y >= _height) ) return;

	set_addr_window_tft(x,y,x+1,y+1);

	TFT_COMMAND_HIGH;
	spi_stream_pixel_tft(color);
}

void my_delay_tft(USHORT ms)
{
	USHORT x = 0;
	for(x=0; x<ms; x+=5)
	{
		//_delay_ms(5);
		// 3 cycles per loop. 1/80 Mhz = 12.5 ns
		// 12.5 ns * 3 = 37.5 ns per loop.
		// 150000 * 37.5 ns = 5.625 ms
		SysCtlDelay(150000);
	}
}

void set_addr_window_tft(UCHAR x0, UCHAR y0, UCHAR x1, UCHAR y1)
{
	write_tft_command_tft(ST7735_CASET);		// Column addr set
	write_tft_data_tft(0x00);
	write_tft_data_tft(x0);						// XSTART
	write_tft_data_tft(0x00);
	write_tft_data_tft(x1);						// XEND

	write_tft_command_tft(ST7735_RASET);		// Row addr set
	write_tft_data_tft(0x00);
	write_tft_data_tft(y0);						// YSTART
	write_tft_data_tft(0x00);
	write_tft_data_tft(y1);						// YEND

	write_tft_command_tft(ST7735_RAMWR);		// write_tft to RAM
}

static void write_spi_tft(BYTE b)
{
	DWORD rcvdat;

	TFT_CS_LOW;
	ROM_SSIDataPut(SSI2_BASE, b); 				/* Write the data to the tx fifo */
	ROM_SSIDataGet(SSI2_BASE, &rcvdat); 		/* flush data read during the write */
	TFT_CS_HIGH;
}

static void write_tft_command_tft(UCHAR b)
{
	TFT_COMMAND_LOW;
	write_spi_tft(b);
}

static void write_tft_data_tft(UCHAR b)
{
	TFT_COMMAND_HIGH;
	write_spi_tft(b);
}

static void init_sequence_tft(void)
{
	write_tft_command_tft(ST7735_SWRESET);		//  1: Software reset, 0 args, w/delay
	my_delay_tft(150);

	write_tft_command_tft(ST7735_SLPOUT);		//  2: Out of sleep mode, 0 args, w/delay
	my_delay_tft(500);

	write_tft_command_tft(ST7735_FRMCTR1);		//  3: Frame rate ctrl - normal mode, 3 args:
	write_tft_data_tft(0x01);					//     Rate = fosc/(1x2+40) * (LINE+2C+2D)
	write_tft_data_tft(0x2C);
	write_tft_data_tft(0x2D);

	write_tft_command_tft(ST7735_FRMCTR2);		//  4: Frame rate control - idle mode, 3 args:
	write_tft_data_tft(0x01);					//     Rate = fosc/(1x2+40) * (LINE+2C+2D)
	write_tft_data_tft(0x2C);
	write_tft_data_tft(0x2D);

	write_tft_command_tft(ST7735_FRMCTR3);		//  5: Frame rate ctrl - partial mode, 6 args:
	write_tft_data_tft(0x01);					//     Dot inversion mode
	write_tft_data_tft(0x2C);					//     Line inversion mode
	write_tft_data_tft(0x2D);
	write_tft_data_tft(0x01);
	write_tft_data_tft(0x2C);
	write_tft_data_tft(0x2D);

	write_tft_command_tft(ST7735_INVCTR);		//  6: Display inversion ctrl, 1 arg, no delay:
	write_tft_data_tft(0x07);					//     No inversion

	write_tft_command_tft(ST7735_PWCTR1);		//  7: Power control, 3 args, no delay:
	write_tft_data_tft(0xA2);
	write_tft_data_tft(0x02);					//     -4.6V
	write_tft_data_tft(0x84);					//     AUTO mode

	write_tft_command_tft(ST7735_PWCTR2);		//  8: Power control, 1 arg, no delay:
	write_tft_data_tft(0xC5);					//     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	write_tft_command_tft(ST7735_PWCTR3);		//  9: Power control, 2 args, no delay:
	write_tft_data_tft(0x0A);					//     Opamp current small
	write_tft_data_tft(0x00);					//     Boost frequency

	write_tft_command_tft(ST7735_PWCTR4);		// 10: Power control, 2 args, no delay:
	write_tft_data_tft(0x8A);					//     BCLK/2, Opamp current small & Medium low
	write_tft_data_tft(0x2A);

	write_tft_command_tft(ST7735_PWCTR5);		// 11: Power control, 2 args, no delay:
	write_tft_data_tft(0x8A);
	write_tft_data_tft(0xEE);

	write_tft_command_tft(ST7735_VMCTR1);		// 12: Power control, 1 arg, no delay:
	write_tft_data_tft(0x0E);

	write_tft_command_tft(ST7735_INVOFF);		// 13: Don't invert display, no args, no delay

	write_tft_command_tft(ST7735_MADCTL);		// 14: Memory access control (directions), 1 arg:
	write_tft_data_tft(0xC8);					//     row addr/col addr, bottom to top refresh

	write_tft_command_tft(ST7735_COLMOD);		// 15: set color mode, 1 arg, no delay:
	write_tft_data_tft(0x05);					//     16-bit color

	write_tft_command_tft(ST7735_CASET);		//  1: Column addr set, 4 args, no delay:
	write_tft_data_tft(0x00);					//     XSTART = 0
	write_tft_data_tft(0x02);
	write_tft_data_tft(0x00);					//     XEND = 127
	write_tft_data_tft(0x7F+0x02);

	write_tft_command_tft(ST7735_RASET);		//  2: Row addr set, 4 args, no delay:
	write_tft_data_tft(0x00);					//     XSTART = 0
	write_tft_data_tft(0x01);
	write_tft_data_tft(0x00);
	write_tft_data_tft(0x9F+0x01);

	write_tft_command_tft(ST7735_GMCTRP1);		//  1: Magical unicorn dust, 16 args, no delay:
	write_tft_data_tft(0x02);
	write_tft_data_tft(0x1c);
	write_tft_data_tft(0x07);
	write_tft_data_tft(0x12);
	write_tft_data_tft(0x37);
	write_tft_data_tft(0x32);
	write_tft_data_tft(0x29);
	write_tft_data_tft(0x2d);
	write_tft_data_tft(0x29);
	write_tft_data_tft(0x25);
	write_tft_data_tft(0x2b);
	write_tft_data_tft(0x39);
	write_tft_data_tft(0x00);
	write_tft_data_tft(0x01);
	write_tft_data_tft(0x03);
	write_tft_data_tft(0x10);

	write_tft_command_tft(ST7735_GMCTRN1);		//  2: Sparkles and rainbows, 16 args, no delay:
	write_tft_data_tft(0x03);
	write_tft_data_tft(0x1d);
	write_tft_data_tft(0x07);
	write_tft_data_tft(0x06);
	write_tft_data_tft(0x2e);
	write_tft_data_tft(0x2c);
	write_tft_data_tft(0x29);
	write_tft_data_tft(0x2d);
	write_tft_data_tft(0x2e);
	write_tft_data_tft(0x2e);
	write_tft_data_tft(0x37);
	write_tft_data_tft(0x3f);
	write_tft_data_tft(0x00);
	write_tft_data_tft(0x00);
	write_tft_data_tft(0x02);
	write_tft_data_tft(0x10);

	write_tft_command_tft(ST7735_NORON);		//  3: Normal display on, no args, w/delay
	my_delay_tft(10);							//     10 ms delay

	write_tft_command_tft(ST7735_DISPON);		//  4: Main screen turn on, no args w/delay
	my_delay_tft(100);							//     100 ms delay
}

static void draw_fast_v_line_tft(SHORT x, SHORT y, SHORT h, USHORT color)
{
	// Rudimentary clipping
	if( (x >= _width) || (y >= _height) ) return;
	if( (y+h-1) >= _height ) h = _height-y;
	set_addr_window_tft(x,y,x,y+h-1);

	TFT_COMMAND_HIGH;

	while (h--)
	{
		spi_stream_pixel_tft(color);
	}
}

static void draw_fast_h_line_tft(SHORT x, SHORT y, SHORT w, USHORT color)
{
	// Rudimentary clipping
	if( (x >= _width) || (y >= _height) ) return;
	if( (x+w-1) >= _width )  w = _width-x;
	set_addr_window_tft(x,y,x+w-1,y);

	TFT_COMMAND_HIGH;

	while( w-- )
	{
		spi_stream_pixel_tft(color);
	}
}

static void write_tft(UCHAR c)
{
	if( c == '\n' )
	{
		cursor_y += 8;
		cursor_x = 0;
	}
	else if( c == '\r' )
	{
		// skip em
	}
	else
	{
		draw_char_tft(cursor_x,cursor_y,c,textcolor,textbgcolor);
		cursor_x += 6;
		if( wrap && (cursor_x > (_width - 6)) )
		{
			cursor_y += 8;
			cursor_x = 0;
		}
	}
}

static void TrimLeadingTrailingSpaces(char *string)
{
	const char *firstNonSpace = string;
	while(*firstNonSpace != '\0' && isspace(*firstNonSpace))
	{
		firstNonSpace++;
	}

	size_t len = strlen(firstNonSpace);
	memmove(string, firstNonSpace, len);

	char* endOfString = string + len - 1;
	while(string < endOfString  && isspace(*endOfString))
	{
		endOfString--;
	}
	*++endOfString = '\0';
}

