/******************************************************************
 *  FreeRTOS & TivaWare Example
 *
 ******************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include"timers.h"

#define SWITCHTASKSTACKSIZE        256

TimerHandle_t Timer_Function1;

void PORTF_Init (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    /*Enabling PORTF*/
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);    /*Enabling internal LEDs*/
}


void UART0_Init (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    /*Enabling PORTA*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);     /*Enabling UART0*/

    UARTClockSourceSet(UART0_BASE,UART_CLOCK_SYSTEM);   /*UART0 is working using the system clock*/
    UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
                    /*Setting the UART baudrate, 8 bit mode, one stop bit and no parity bit*/
    UARTEnable(UART0_BASE);                         /*Enabling UART0*/
    GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1); /*Setting UART0 pins PA0,PA1*/

}


void MyTimer( TimerHandle_t Timer_Function1 )
{
    uint8_t colors[7]={2,4,6,8,10,12,14};   /*LED colors*/
      static  uint8_t i=0;
        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,colors[i]);
                    i= (i+1)%7;
}
void Task1(void* para)
{
    char in;
    uint32_t period = 100;  //initial timer period
    while(1)
    {
        in = UARTCharGet(UART0_BASE);  //wait untill get a char
        if(in == 's')
        {
            xTimerStop( Timer_Function1 ,1000 );
        }
        else if(in == 'r' )
        {
            xTimerReset( Timer_Function1, 1000 );
        }
        else if(in == 'd' )
        {
            period = period * 2 ;
            xTimerChangePeriod( Timer_Function1, period, 1000 );
        }
        else if(in == 'i' )
        {
            period = period / 2 ;
            xTimerChangePeriod( Timer_Function1, period, 1000 );
        }

    }
}

int main(void) {
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); /*80 MHz Clock*/
    PORTF_Init();
    UART0_Init();
    TaskHandle_t  First_handle;
    Timer_Function1= xTimerCreate(  "MyTimer",100, pdTRUE,( void * ) 0, MyTimer);
    xTimerStart( Timer_Function1, 0 );
    xTaskCreate(Task1, "Task1",SWITCHTASKSTACKSIZE, NULL,1, &First_handle);
    vTaskStartScheduler();
    while(1)
    {

    }
    return 0;
}
