#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/UART2.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define DISPLAY(x) UART2_write(uart, &output, x, NULL);

// I2C and UART global variables
static const struct {
    uint8_t address;
    uint8_t resultReg;
    char *id;
} sensors[3] = {
    { 0x48, 0x0000, "11X" },
    { 0x49, 0x0000, "116" },
    { 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];
uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;
char output[64];

// Driver Handles
I2C_Handle i2c;
UART2_Handle uart;
Timer_Handle timer0;

volatile unsigned char TimerFlag = 0;

// GPIO interrupt flags
volatile unsigned char Button0Flag = 0;
volatile unsigned char Button1Flag = 0;

// System variables
volatile int16_t setPoint = 25; // Default set-point
volatile uint32_t seconds = 0;
volatile int16_t roomTemperature = 0;
volatile uint8_t heat = 0;

// Timer callback
void timerCallback(Timer_Handle myHandle, int_fast16_t status) {
    TimerFlag = 1; // Set the timer flag
}

// GPIO button callbacks
void gpioButtonFxn0(uint_least8_t index) {
    Button0Flag = 1; // Flag for button 0 press
}
void gpioButtonFxn1(uint_least8_t index) {
    Button1Flag = 1; // Flag for button 1 press
}

// Initialize UART
void initUART(void) {
    UART2_Params uartParams;
    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200;

    uart = UART2_open(CONFIG_UART2_0, &uartParams);
    if (uart == NULL) {
        while (1); // Halt if UART initialization fails
    }

    DISPLAY(snprintf(output, sizeof(output), "UART Initialized\n\r"));
}

// Initialize I2C
void initI2C(void) {
    I2C_Params i2cParams;
    I2C_init();
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz; // Set to 100kHz for compatibility

    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL) {
        printf("I2C initialization failed.\n");
        while (1);
    }
    printf("I2C Initialized at 100kHz\n");
}

void initTMP006(void) {
    txBuffer[0] = 0x02; // Configuration register for TMP006
    txBuffer[1] = 0x74; // Enable continuous conversion (example value)
    txBuffer[2] = 0x00; // Example configuration

    i2cTransaction.targetAddress = sensors[2].address;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 3; // Writing to a register requires 3 bytes
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(i2c, &i2cTransaction)) {
        DISPLAY(snprintf(output, sizeof(output), "TMP006 initialized successfully\n\r"));
    } else {
        DISPLAY(snprintf(output, sizeof(output), "TMP006 initialization failed with status: %d\n\r", i2cTransaction.status));
    }
}

// Read temperature using I2C
int16_t readTemp(void) {
    txBuffer[0] = sensors[2].resultReg; // Use the register for TMP006
    i2cTransaction.targetAddress = sensors[2].address;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;

    if (I2C_transfer(i2c, &i2cTransaction)) {
        int16_t tempRaw = (rxBuffer[0] << 8) | rxBuffer[1];
        // TMP006 has a 14-bit temperature resolution
        tempRaw >>= 2; // Adjust for 14-bit data
        return tempRaw * 0.03125; // Convert raw value to Celsius (0.03125 = 1/32)
    } else {
        DISPLAY(snprintf(output, sizeof(output), "Error: TMP006 I2C transfer failed. Status: %d\n\r", i2cTransaction.status));
        return -1000; // Return an error value
    }
}

// Initialize Timer
void initTimer(void) {
    Timer_Params params;
    Timer_init();
    Timer_Params_init(&params);
    params.period = 100000; // 100ms period
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL || Timer_start(timer0) == Timer_STATUS_ERROR) {
        while (1); // Halt if Timer initialization fails
    }

    DISPLAY(snprintf(output, sizeof(output), "Timer Initialized\n\r"));
}

// Update LED based on heating status
void updateLED(void) {
    if (roomTemperature < setPoint) {
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        heat = 1;
    } else {
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        heat = 0;
    }
}

// Report system status via UART
void reportStatus(void) {
    DISPLAY(snprintf(output, sizeof(output), "<%02d,%02d,%d,%04d>\n\r",
                     roomTemperature, setPoint, heat, seconds));
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0) {
    // Initialize peripherals
    GPIO_init();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);

    initUART();

    initI2C();
    initTMP006();
    initTimer();

    uint32_t taskCounter = 0;

    while (1) {
        while (!TimerFlag) {} // Wait for the timer flag
        TimerFlag = 0; // Reset the flag
        taskCounter++;

        // Handle button presses every 200ms
        if (taskCounter % 2 == 0) {
            if (Button0Flag) {
                Button0Flag = 0;
                setPoint++;
                printf("Button 0 pressed: Increasing set point to %d (200ms).\n", setPoint);
            }
            if (Button1Flag) {
                Button1Flag = 0;
                setPoint--;
                printf("Button 1 pressed: Decreasing set point to %d (200ms).\n", setPoint);
            }
        }

        // Read temperature and update LED every 500ms
        if (taskCounter % 5 == 0) {
            roomTemperature = readTemp();
            updateLED();
            printf("Reading temperature: %d°C, updating LED (500ms).\n", roomTemperature);
        }

        // Report system status every 1 second
        if (taskCounter % 10 == 0) {
            reportStatus();
            seconds++;
        }
    }

    return NULL;
}
