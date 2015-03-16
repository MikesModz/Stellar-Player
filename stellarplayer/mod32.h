#ifndef __MOD32_H__
#define __MOD32_H__

// Effects
#define MOD32_ARPEGGIO              0x0
#define MOD32_PORTAMENTOUP          0x1
#define MOD32_PORTAMENTODOWN        0x2
#define MOD32_TONEPORTAMENTO        0x3
#define MOD32_VIBRATO               0x4
#define MOD32_PORTAMENTOVOLUMESLIDE 0x5
#define MOD32_VIBRATOVOLUMESLIDE    0x6
#define MOD32_TREMOLO               0x7
#define MOD32_SETCHANNELPANNING     0x8
#define MOD32_SETSAMPLEOFFSET       0x9
#define MOD32_VOLUMESLIDE           0xA
#define MOD32_JUMPTOORDER           0xB
#define MOD32_SETVOLUME             0xC
#define MOD32_BREAKPATTERNTOROW     0xD
#define MOD32_SETSPEED              0xF

// 0xE subset
#define MOD32_SETFILTER             0x0
#define MOD32_FINEPORTAMENTOUP      0x1
#define MOD32_FINEPORTAMENTODOWN    0x2
#define MOD32_GLISSANDOCONTROL      0x3
#define MOD32_SETVIBRATOWAVEFORM    0x4
#define MOD32_SETFINETUNE           0x5
#define MOD32_PATTERNLOOP           0x6
#define MOD32_SETTREMOLOWAVEFORM    0x7
#define MOD32_RETRIGGERNOTE         0x9
#define MOD32_FINEVOLUMESLIDEUP     0xA
#define MOD32_FINEVOLUMESLIDEDOWN   0xB
#define MOD32_NOTECUT               0xC
#define MOD32_NOTEDELAY             0xD
#define MOD32_PATTERNDELAY          0xE
#define MOD32_INVERTLOOP            0xF

//Prototypes
void mod_player();
void mod_mixer();
void loadMod();
uint16_t mod_getSamplesPerTick();
void restartMod();

#endif
