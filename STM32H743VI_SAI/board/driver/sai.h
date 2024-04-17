#ifndef __SAI_H
#define __SAI_H


extern SAI_HandleTypeDef SAI1A_Handler;        //SAI1 Block A句柄

extern void (*sai_tx_callback)(void);		//sai tx回调函数指针  

uint8_t SAIA_SampleRate_Set(uint32_t samplerate);
void SAIA_TX_DMA_Init(uint8_t* buf0,uint8_t *buf1,uint16_t num,uint8_t width);
void SAIA_DMA_Enable(void);
void SAI_Play_Start(void); 
void SAI_Play_Stop(void); 



void SAI1A_Init(void);

#endif

