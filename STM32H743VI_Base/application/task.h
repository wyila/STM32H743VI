#ifndef __CM7_TASK_H
#define __CM7_TASK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hal_sys.h"
#include "usart.h"
#include "LED.h"
#include "Key.h"
#include "backupregist.h"
#include "tim.h"
#include "adc.h"
#include "iic_master.h"

void BSP_Init(void);
void BSP_DeInit(void);
void LED_Task(void);
void USART_Task(void);
void ADC_Task(void);




#endif
