/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSOC C1 MCU: Hello world 
*              Example for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cybsp.h"
#include "cy_utils.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* SysTick timer frequency in Hz */
#define TICKS_PER_SECOND 1000
/* USER LED toggle period in milliseconds */
#define USER_LED_TOGGLE_PERIOD_MS   500

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Variable for keeping track of time */
static volatile uint32_t ticks = 0;

/* Flag to control LED blinking: true = blinking active, false = paused */
static volatile bool led_blink_active_flag = true;


/*******************************************************************************
* Function Name: SysTick_Handler
********************************************************************************
* Summary:
* This is the interrupt handler function for the SysTick timer interrupt.
* It counts the time elapsed in milliseconds since the timer started.
*
* Parameters:
*  none
*
* Return:
*  none
*
*******************************************************************************/
void SysTick_Handler(void)
{
    ticks++;
}

/*******************************************************************************
* Function Name: sys_now
********************************************************************************
* Summary:
* Returns the current time in milliseconds.
*
* Parameters:
*  none
*
* Return:
*  the current time in milliseconds
*
*******************************************************************************/
__STATIC_INLINE uint32_t sys_now(void)
{
    return ticks;
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function. It sets up a timer to trigger a
* periodic interrupt. The main while loop checks for the elapsed time
* and toggles an LED at 1Hz to create an LED blinky.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    cy_retarget_io_init(CYBSP_DEBUG_UART_HW);

    /* System timer configuration */
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
        "PDL: Hello World! Example "
        "****************** \r\n\n");

    printf("Hello World!!!\r\n\n");
    printf("For more projects, "
    "visit our code examples repositories:\r\n\n");
    printf("https://github.com/Infineon/"
    "Code-Examples-for-ModusToolbox-Software\r\n\n");
    printf("Press 'Enter' to pause or resume LED blinking.\r\n");

    uint32_t last_toggle = sys_now();
    
    while (1)
    {
        /* Non-blocking check for received UART character */
        if (Cy_USIC_CH_GetReceiveBufferStatus(CYBSP_DEBUG_UART_HW) != 0U)
        {
            uint16_t ch = Cy_UART_CH_GetReceivedData(CYBSP_DEBUG_UART_HW);
            if (ch == '\r')
            {
                if (led_blink_active_flag)
                {
                    printf("LED blinking paused \r\n");
                }
                else
                {
                    printf("LED blinking resumed\r\n");
                }
                /* Move cursor to previous line */
                printf("\x1b[1F");
                led_blink_active_flag ^= 1;
            }
        }

        /* Non-blocking LED toggle at USER_LED_TOGGLE_PERIOD_MS rate */
        if (led_blink_active_flag)
        {
            uint32_t now = sys_now();
            if ((now - last_toggle) >= USER_LED_TOGGLE_PERIOD_MS)
            {
                last_toggle = now;
                Cy_GPIO_ToggleOutput(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
            }
        }
        else
        {
            /* Keep last_toggle current so blinking resumes without a skipped beat */
            last_toggle = sys_now();
        }
    }
}

/* [] END OF FILE */
