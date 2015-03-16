#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "drivers/buttons.h"

static unsigned char g_ucButtonStatesExt = EXT_ALL_BUTTONS;

unsigned char ExtButtonsPoll(void)
{
    unsigned long ulDelta;
    unsigned long ulData;
    static unsigned char ucSwitchClockA = 0;
    static unsigned char ucSwitchClockB = 0;

    //
    // Read the raw state of the push buttons.  Save the raw state
    // (inverting the bit sense) if the caller supplied storage for the
    // raw value.
    //
    ulData = (ROM_GPIOPinRead(EXT_BUTTONS_GPIO_BASE, EXT_ALL_BUTTONS));

    //
    // Determine the switches that are at a different state than the debounced
    // state.
    //
    ulDelta = ulData ^ g_ucButtonStatesExt;

    //
    // Increment the clocks by one.
    //
    ucSwitchClockA ^= ucSwitchClockB;
    ucSwitchClockB = ~ucSwitchClockB;

    //
    // Reset the clocks corresponding to switches that have not changed state.
    //
    ucSwitchClockA &= ulDelta;
    ucSwitchClockB &= ulDelta;

    //
    // Get the new debounced switch state.
    //
    g_ucButtonStatesExt &= ucSwitchClockA | ucSwitchClockB;
    g_ucButtonStatesExt |= (~(ucSwitchClockA | ucSwitchClockB)) & ulData;

    //
    // Determine the switches that just changed debounced state.
    //
    ulDelta ^= (ucSwitchClockA | ucSwitchClockB);

    //
    // Return the debounced buttons states to the caller.  Invert the bit
    // sense so that a '1' indicates the button is pressed, which is a
    // sensible way to interpret the return value.
    //
    return(~g_ucButtonStatesExt);
}

void ExtButtonsInit(void)
{
    //
    // Enable the GPIO port to which the pushbuttons are connected.
    //
    ROM_SysCtlPeripheralEnable(EXT_BUTTONS_GPIO_PERIPH);

    // Set additional external pins as inputs
    ROM_GPIOPinTypeGPIOInput(EXT_BUTTONS_GPIO_BASE, EXT_ALL_BUTTONS);

    // Enable pull ups on these pins
    ROM_GPIOPadConfigSet(EXT_BUTTONS_GPIO_BASE, EXT_ALL_BUTTONS, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //
    // Initialize the debounced button state with the current state read from
    // the GPIO bank.
    //
    g_ucButtonStatesExt = ROM_GPIOPinRead(EXT_BUTTONS_GPIO_BASE, EXT_ALL_BUTTONS);
}
