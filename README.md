# StellarPlayer

A MOD & S3M Player for the Texas Instruments Stellaris® LM4F120 launch pad evaluation kit.

I take minimal credit for this project. This is simply a modified version of the Stellaris mod player by Ronen K. 
Which in turn was based on a similar implementation on the PIC32 by Serveur Perso.

[Stellaris Launchpad MOD Player](http://mobile4dev.blogspot.de/2012/11/stellaris-launchpad-mod-player.html)

[Stellarplayer MOD & S3M Module Player](http://mobile4dev.blogspot.de/search/label/MOD%20Player)

[My SD card MOD & S3M Player engine for PIC32 with source code](https://www.youtube.com/watch?v=i3Yl0TISQBE)

## Additions

- 128 x 160 SPI TFT module.
- Displays list of .mod and .s3m files found on SD card.
- Next and previous modules can be selected.
- Shows current row, current pattern and total patterns while playing.

[StellarPlayer with TFT display](https://www.youtube.com/watch?v=9vW0ljh3YDw)

##Launch Pad Connections

Connections     | Launch Pad
:----------------|:-----------
| TFT_RESET     | PA7 |
| TFT_COMMAND   | PA6 |
| TFT_CS        | PB5 |
| TFT_MOSI      | PB7	|			
| TFT_MISO      | PB6 |
| TFT_SCK       | PB4 |
| SD_SCK        | PA2 |
| SD_CS         | PA3 |
| SD_MISO       | PA4 |
| SD_MOSI       | PA5 |
| Left channel  | PB0 |
| Right channel | PB1 |

## Note

Assumes StellarisWare Driverlib is installed in the folder "C:\StellarisWare\".
