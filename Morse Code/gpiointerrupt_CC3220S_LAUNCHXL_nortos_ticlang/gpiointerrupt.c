/*
 *  ======== gpiointerrupt_morse_buttons.c ========
 *  Implements buttons for starting/stopping and toggling Morse messages
 */

#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Morse code message states */
enum MESSAGE_STATE { SOS_MESSAGE, OK_MESSAGE, STOPPED } CURRENT_MESSAGE, BUTTON_STATE;

/* LED state enumeration */
enum LED_STATE { LED_RED, LED_GREEN, LED_OFF } LED_STATE;

/* Morse messages as arrays of LED states */
enum LED_STATE sosMessage[] = {
    LED_RED, LED_OFF,                           // DOT, break
    LED_RED, LED_OFF,                           // DOT, break
    LED_RED, LED_OFF, LED_OFF, LED_OFF,         // DOT, character break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF,   // DASH, break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF,   // DASH, break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_OFF, LED_OFF, // DASH, character break
    LED_RED, LED_OFF,                           // DOT, break
    LED_RED, LED_OFF,                           // DOT, break
    LED_RED, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF // Word break
};

enum LED_STATE okMessage[] = {
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF,   // DASH, break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF,   // DASH, break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_OFF, LED_OFF, // DASH, character break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF,   // DASH, break
    LED_RED, LED_OFF,                           // DOT, break
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF,   // DASH
    LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF // Word break
};

/* Current position in the message array */
unsigned int messageCounter = 0;

/* Flag to indicate if blinking is active */
volatile int blinkingActive = 1;

/*
 *  ======== setMorseLEDs ========
 *  Updates the LEDs based on the current LED state.
 */
void setMorseLEDs() {
    switch (LED_STATE) {
        case LED_RED:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
            break;
        case LED_GREEN:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
            break;
        case LED_OFF:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
            break;
        default:
            break;
    }
}

/*
 *  ======== timerCallback ========
 *  Processes the current message and updates the LED state.
 */
void timerCallback(Timer_Handle myHandle, int_fast16_t status) {
    if (!blinkingActive) {
        LED_STATE = LED_OFF;
        setMorseLEDs();
        return; // Do nothing if blinking is stopped
    }

    switch (CURRENT_MESSAGE) {
        case SOS_MESSAGE:
            LED_STATE = sosMessage[messageCounter];
            setMorseLEDs();
            messageCounter++;

            if (messageCounter == (sizeof(sosMessage) / sizeof(sosMessage[0]))) {
                CURRENT_MESSAGE = BUTTON_STATE;
                messageCounter = 0;
            }
            break;

        case OK_MESSAGE:
            LED_STATE = okMessage[messageCounter];
            setMorseLEDs();
            messageCounter++;

            if (messageCounter == (sizeof(okMessage) / sizeof(okMessage[0]))) {
                CURRENT_MESSAGE = BUTTON_STATE;
                messageCounter = 0;
            }
            break;

        default:
            break;
    }
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Toggles between SOS and OK messages.
 */
void gpioButtonFxn0(uint_least8_t index) {
    if (!blinkingActive) {
        return; // Ignore toggle if blinking is stopped
    }

    switch (BUTTON_STATE) {
        case SOS_MESSAGE:
            BUTTON_STATE = OK_MESSAGE;
            break;
        case OK_MESSAGE:
            BUTTON_STATE = SOS_MESSAGE;
            break;
        default:
            break;
    }
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Starts or stops the blinking.
 */
void gpioButtonFxn1(uint_least8_t index) {
    blinkingActive = !blinkingActive; // Toggle blinking state

    if (blinkingActive && CURRENT_MESSAGE == STOPPED) {
        CURRENT_MESSAGE = BUTTON_STATE;
    }
}

/*
 *  ======== initTimer ========
 *  Initializes the timer for periodic interrupts.
 */
void initTimer(void) {
    Timer_Handle timer0;
    Timer_Params params;

    Timer_init();
    Timer_Params_init(&params);

    params.period = 500000;  // 500ms period
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL) {
        while (1); // Error initializing timer
    }
    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        while (1); // Error starting timer
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0) {
    /* Initialize GPIO and Timer drivers */
    GPIO_init();
    initTimer();

    /* Configure LEDs and buttons */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Initialize LEDs to off state */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);

    /* Set initial states */
    BUTTON_STATE = SOS_MESSAGE;
    CURRENT_MESSAGE = BUTTON_STATE;

    /* Configure button callbacks */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);

    /* Enable button interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);

    return NULL;
}
