#ifndef __LED_H
#define __LED_H

#define LED0_PORT       GPIOB
#define LED0_PIN        GPIO_PIN_4

#define LED1_PORT       GPIOA
#define LED1_PIN        GPIO_PIN_13

#define LED2_PORT       GPIOA
#define LED2_PIN        GPIO_PIN_14

#define LED3_PORT       GPIOA
#define LED3_PIN        GPIO_PIN_15



#define LED_OPEN        1
#define LED_CLOSE       0

void LED_Init(void);
void LED0_SW(uint8_t SW);
void LED0_Toggles(void);

void LED1_SW(uint8_t SW);
void LED1_Toggles(void);

void LED2_SW(uint8_t SW);
void LED2_Toggles(void);

void LED3_SW(uint8_t SW);
void LED3_Toggles(void);


void LED_ALL_SW(uint8_t SW);
void LED_ALL_Toggles(void);


#endif

