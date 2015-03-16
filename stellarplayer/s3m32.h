#ifndef __S3M32_H__
#define __S3M32_H__

#include <stdint.h>

// Effects
#define S3M32_SETSPEED                 0x1  // Axx
#define S3M32_JUMPTOORDER              0x2  // Bxx
#define S3M32_BREAKPATTERNTOROW        0x3  // Cxx
#define S3M32_VOLUMESLIDE              0x4  // Dxx
#define S3M32_PORTAMENTODOWN           0x5  // Exx
#define S3M32_PORTAMENTOUP             0x6  // Fxx
#define S3M32_TONEPORTAMENTO           0x7  // Gxx
#define S3M32_VIBRATO                  0x8  // Hxy
#define S3M32_TREMOR                   0x9  // Ixy
#define S3M32_ARPEGGIO                 0xA  // Jxy
#define S3M32_VIBRATOVOLUMESLIDE       0xB  // Kxy
#define S3M32_PORTAMENTOVOLUMESLIDE    0xC  // Lxy
#define S3M32_SETSAMPLEOFFSET          0xF  // Oxy
#define S3M32_RETRIGGERNOTEVOLUMESLIDE 0x11 // Qxy
#define S3M32_TREMOLO                  0x12 // Rxy
#define S3M32_SETTEMPO                 0x14 // Txx
#define S3M32_FINEVIBRATO              0x15 // Uxy
#define S3M32_SETGLOBALVOLUME          0x16 // Vxx

// 0x13 subset
#define S3M32_SETFILTER                0x0
#define S3M32_SETGLISSANDOCONTROL      0x1
#define S3M32_SETFINETUNE              0x2
#define S3M32_SETVIBRATOWAVEFORM       0x3
#define S3M32_SETTREMOLOWAVEFORM       0x4
#define S3M32_SETCHANNELPANNING        0x8
#define S3M32_STEREOCONTROL            0xA
#define S3M32_PATTERNLOOP              0xB
#define S3M32_NOTECUT                  0xC
#define S3M32_NOTEDELAY                0xD
#define S3M32_PATTERNDELAY             0xE
#define S3M32_FUNKREPEAT               0xF

//Prototypes
void s3m_player();
void s3m_mixer();
void loadS3m();
uint16_t s3m_getSamplesPerTick();

#endif
