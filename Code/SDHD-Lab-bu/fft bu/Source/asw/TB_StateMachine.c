/**
* \file TB_StateMachine.c
* \author Juarez
* \date 27.05.2023
*
* \brief Test Bench State Machine
*
* \copyright Copyright ©2016
* Department of electrical engineering and information technology, Hochschule Darmstadt - University of applied sciences (h_da). All Rights Reserved.
* Permission to use, copy, modify, and distribute this software and its documentation for educational, and research purposes in the context of non-commercial
* (unless permitted by h_da) and official h_da projects, is hereby granted for enrolled students of h_da, provided that the above copyright notice,
* this paragraph and the following paragraph appear in all copies, modifications, and distributions.
* Contact Prof.Dr.-Ing. Peter Fromm, peter.fromm@h-da.de, Birkenweg 8 64295 Darmstadt - GERMANY for commercial requests.
*
* \warning This software is a PROTOTYPE version and is not designed or intended for use in production, especially not for safety-critical applications!
* The user represents and warrants that it will NOT use or redistribute the Software for such purposes.
* This prototype is for research purposes only. This software is provided "AS IS," without a warranty of any kind.
*/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "TB_StateMachine.h"
#include "TB_ADC_Driver.h"
#include "UART_Log.h"
#include "fft_application.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/
volatile event_t ev = ev_lastEvent;

/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/


/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

/**
 * Initializes state machine object
 * @param TB_SM_t *obj
 * @return obj
 */
void TB_SM_Init(TB_SM_t* obj)
{
    /*
        state = IDLE;
        samplesCount = 0;
        samplesArray[1024] = {0};
        fft_array[2*ADC_SAMPLES];
        transferCount = 0;
    */
    TB_SM_t newObj = {IDLE, INIT_VALUE, {INIT_VALUE}, {INIT_VALUE}, 0};
    *obj = newObj;
}

/**
 * Runs Test Bench State Machine
 * @param TB_SM_t *const me, event_t event
 * @return None
 */
void TB_SM_run(TB_SM_t *const me, event_t event)
{
    uint16_t adcIn = INIT_VALUE;
    
    switch(me->state)
    {
        case IDLE:
        {
            TB_SM_UpdateLEDStateStatus(me->state);
            me->transferCount = INIT_VALUE;
            if(event == ev_pushButton)
            {
                me->state = SAMPLING;
                me->samplesCount = INIT_VALUE;
            }
        }
        break;
        
        case SAMPLING:
        {
            TB_SM_UpdateLEDStateStatus(me->state);

            if(me->samplesCount < ADC_SAMPLES && event == ev_adcDone)
            {
                TB_ADC_Read(&adcIn);
                me->samplesArray[me->samplesCount] = adcIn;
                me->samplesCount++;
            }
            if((me->samplesCount == ADC_SAMPLES) && (event == ev_matlab_s))
            {
                //Adding FFT
                fft_app(me->samplesArray, me->fft_array, ADC_SAMPLES);
                me->samplesCount = INIT_VALUE;//Reset to start counting in the transfer from 0 to 1024
                me->state = UART_TRANSFER;
            }
        }
        break;
        
        case UART_TRANSFER:
        {
            TB_SM_UpdateLEDStateStatus(me->state);
            //Code to transfer to Matlab
            if(me->samplesCount < ADC_SAMPLES && (me->transferState == 0))
            {
                uint8_t lsb = (uint8)(me->samplesArray[me->samplesCount] & 0x00FF);
                uint8_t msb = (uint8)((me->samplesArray[me->samplesCount] & 0xFF00) >> 8);
                
                UART_Log_PutChar(lsb);
                UART_Log_PutChar(msb);
                //Clears sample once it has been sent
                me->samplesArray[me->samplesCount] = 0;
                //Go to next sample
                me->samplesCount++;
            }
            else
            {
                me->transferState = 1;
            }
            
            if ((me->fftSamplesCountReal < (ADC_SAMPLES*2)) && me->transferState == 1 && (me->samplesCount>=ADC_SAMPLES))
            {
                //realMSB realLSB 
                //imaginaryMSB  imaginaryLSB
                uint8_t realMSB  = (uint8)(me->fft_array[me->fftSamplesCountReal] & 0xff000000) >> 24;
                uint8_t realLSB = (uint8)(me->fft_array[me->fftSamplesCountReal] & 0x00ff0000) >> 16;

                UART_Log_PutChar(realMSB);
                UART_Log_PutChar(realLSB);

                
                me->samplesArray[me->fftSamplesCountReal] = 0;
                //Go to next sample
                me->fftSamplesCountReal++;
            }
            else
            {
                me->transferState = 2;
            }
            
            if ((me->fftSamplesCountReal < (ADC_SAMPLES*2)) && me->transferState == 2 && (me->fftSamplesCountReal>= (2*ADC_SAMPLES)))
            {
                uint8_t imaginaryMSB = (uint8)(me->fft_array[me->fftSamplesCountImaginary] & 0x0000ff00) >>  8;
                uint8_t imaginaryLSB  = (uint8)(me->fft_array[me->fftSamplesCountImaginary] & 0x000000ff);

                UART_Log_PutChar(imaginaryMSB);
                UART_Log_PutChar(imaginaryLSB);

                
                me->samplesArray[me->fftSamplesCountImaginary] = 0;
                //Go to next sample
                me->fftSamplesCountImaginary++;
            }
            else
            {
                me->transferState = 3;
            }
            
            if(me->transferCount == MAX_TRANSFER_COUNT && event == ev_matlab_o)
            {
                me->state = IDLE;
            }
            else if(me->transferCount < MAX_TRANSFER_COUNT && event == ev_matlab_o && (me->transferState == 3))
            {
                me->transferCount++;
                me->samplesCount = INIT_VALUE;
                me->fftSamplesCountReal = INIT_VALUE;
                me->fftSamplesCountImaginary = INIT_VALUE;
                me->state = SAMPLING;
            }

            else
            {
                //Do nothing
            }

        }
        break;

        default:
        {
            //Do nothing
        }
        break;
    }
}

/**
 * Gets Event
 * @param event_t* event
 * @return event
 */
void TB_SM_getEvent(event_t *event)
{
    *event = ev;
}

/**
 * Sets Event
 * @param event_t event
 * @return ev
 */
void TB_SM_setEvent(event_t event)
{
    ev = event;
}

void TB_SM_UpdateLEDStateStatus(TB_SM_state_t state)
{
    switch(state)
    {
        case IDLE:
        {
            LED_green_Write(ON);
            LED_orange_Write(OFF);
            LED_red_Write(OFF);
        }
        break;
        
        case SAMPLING:
        {
            LED_green_Write(OFF);
            LED_orange_Write(ON);
            LED_red_Write(OFF);
        }
        break;
        
        case UART_TRANSFER:
        {
            LED_green_Write(OFF);
            LED_orange_Write(OFF);
            LED_red_Write(ON);
        }
        break;
        
        default:
            LED_green_Write(OFF);
            LED_orange_Write(OFF);
            LED_red_Write(OFF);
        break;
    }
}