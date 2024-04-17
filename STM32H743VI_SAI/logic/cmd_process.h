#ifndef __CMD_PROCESS_H
#define __CMD_PROCESS_H

void IIC_Write_Process(struct IIC_GPIO *IIC, uint8_t *buf, uint32_t len);
void IIC_Read_Process(struct IIC_GPIO *IIC, uint8_t *buf, uint32_t len);
void ADC_Process(uint8_t *buf, uint32_t len);
    
#endif


