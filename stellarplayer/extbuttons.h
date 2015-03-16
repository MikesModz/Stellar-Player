
#ifndef __EXT_BUTTONS_H__
#define __EXT_BUTTONS_H__

#define EXT_BUTTONS_GPIO_PERIPH     SYSCTL_PERIPH_GPIOD
#define EXT_BUTTONS_GPIO_BASE       GPIO_PORTD_BASE

#define EXT_NUM_BUTTONS             4
#define EXT_UP_BUTTON				GPIO_PIN_0
#define EXT_DOWN_BUTTON				GPIO_PIN_1
#define EXT_LEFT_BUTTON             GPIO_PIN_2
#define EXT_RIGHT_BUTTON            GPIO_PIN_3

#define EXT_ALL_BUTTONS             (EXT_UP_BUTTON|EXT_DOWN_BUTTON|EXT_LEFT_BUTTON|EXT_RIGHT_BUTTON)

void ExtButtonsInit(void);
unsigned char ExtButtonsPoll(void);
unsigned char TestExternalButtons(void);

#endif // __EXT_BUTTONS_H__
