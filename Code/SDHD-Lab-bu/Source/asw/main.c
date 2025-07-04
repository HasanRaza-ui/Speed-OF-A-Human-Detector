/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "global.h"
#include "UART_Log.h"
#include "TB_ADC_Driver.h"
#include "TB_StateMachine.h"

//ISR Prototypes
CY_ISR_PROTO(isr_PushButton);
CY_ISR_PROTO(isr_UART);
CY_ISR_PROTO(isr_ADC);

int main(void)
{    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    //Init Drivers
    UART_Log_Start();
    WaveDAC8_1_Start();
    TB_ADC_Init();
    
    //Init Interrupts
    isr_ADC_StartEx(isr_ADC);
    isr_UART_StartEx(isr_UART);
    isr_PushButton_StartEx(isr_PushButton);
    
    //Init Test Bench State Machine
    TB_SM_t tb_sm_obj;
    TB_SM_Init(&tb_sm_obj);
    //TB_SM_t tb_sm_obj = {IDLE, INIT_VALUE, {INIT_VALUE}, INIT_VALUE};
    event_t evm;

    while(1)
    {
        TB_SM_getEvent(&evm);
        TB_SM_run(&tb_sm_obj, evm);
    }
}

/*ISR Definitions*/

CY_ISR(isr_PushButton)
{
    PushButton_ClearInterrupt();
    if(PushButton_Read() == 0)
    {
        TB_SM_setEvent(ev_pushButton);
    }
}
CY_ISR(isr_ADC)
{
    TB_SM_setEvent(ev_adcDone);
}

CY_ISR(isr_UART)
{
    uint8_t rxData = UART_Log_GetChar();
    
    if (rxData  == 's')
    {
        TB_SM_setEvent(ev_matlab_s);
    }
    else if(rxData == 'o')
    {
        TB_SM_setEvent(ev_matlab_o);
    }
    else
    {
        //Do nothing
    }
}

/* [] END OF FILE */