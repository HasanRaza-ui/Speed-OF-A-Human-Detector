/**
* \file TB_ADC_Driver.c
* \author Juarez
* \date 27.05.2023
*
* \brief Test Bench ADC Driver
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
#include "TB_ADC_Driver.h"
/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/


/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/


/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

/**
 * TB_ADC_Init()
 * @param none
 * @return none
 */
void TB_ADC_Init()
{
    ADC_Start();
    ADC_StartConvert();
}

/**
 * TB_ADC_Read()
 * @param uint16_t *dataIn
 * @return dataIn
 */
void TB_ADC_Read(uint16_t *dataIn)
{
    //Wait for end of convertion
     ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    
    //Pass data
    *dataIn = (uint16_t)ADC_GetResult32(); //Needed when single ended 16-bit resolution is selected
}
