/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uart2echo.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{

    enum LED_STATES { LED_START, STATE_0, STATE_1, LED_OFF, LED_ON } LED_STATE;
    LED_STATE = LED_START;
    char input;
    const char echoPrompt[] = "Type ON or OFF to toggle the LED:\r\n";
    UART2_Handle uart;
    UART2_Params uartParams;
    size_t bytesRead;
    size_t bytesWritten = 0;
    uint32_t status = UART2_STATUS_SUCCESS;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Create a UART where the default read and write mode is BLOCKING */
    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200;

    uart = UART2_open(CONFIG_UART2_0, &uartParams);

    if (uart == NULL)
    {
        /* UART2_open() failed */
        while (1) {}
    }

    /* Turn on user LED to indicate successful initialization */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    UART2_write(uart, echoPrompt, sizeof(echoPrompt), &bytesWritten);

    /* Loop forever echoing */
    while (1)
    {
        bytesRead = 0;
        while (bytesRead == 0)
        {
            status = UART2_read(uart, &input, 1, &bytesRead);

            if (status != UART2_STATUS_SUCCESS)
            {
                /* UART2_read() failed */
                while (1) {}
            }
        }

        bytesWritten = 0;
        while (bytesWritten == 0)
        {
            status = UART2_write(uart, &input, 1, &bytesWritten);

            if (status != UART2_STATUS_SUCCESS)
            {
                /* UART2_write() failed */
                while (1) {}
            }
        }
        switch (LED_STATE)
                {
                    case LED_START:
                        if (input == 'O' || input == 'o') LED_STATE = STATE_0; // Start the sequence
                        else LED_STATE = LED_START; //reset if invalid input
                        break;

                    case STATE_0:
                        if (input == 'N' || input == 'n') LED_STATE = LED_ON; // Continue ON sequence
                        else if (input == 'F' || input == 'f') LED_STATE = STATE_1;
                        else LED_STATE = LED_START; //reset if invalid input
                        break;

                    case STATE_1:
                        if (input == 'F' || input == 'f') LED_STATE = LED_OFF; // Continue Off sequence
                        else LED_STATE = LED_START; //reset if invalid input
                        break;

                    case LED_ON:
                        if (input == '\r' || input == '\n') // end of input
                        {
                            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON); //LED ON
                            UART2_write (uart, "LED ON\r\n", 8, &bytesWritten);
                        }
                        LED_STATE = LED_START; //RESET
                        break;

                    case LED_OFF:
                        if (input == '\r' || input == '\n') // end of input
                        {
                            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); //LED OFF
                            UART2_write (uart, "LED OFF\r\n", 9, &bytesWritten);
                         }
                         LED_STATE = LED_START; //RESET
                         break;

                    default:
                        LED_STATE = LED_START; //RESET
                        break;
                }
    }
}
