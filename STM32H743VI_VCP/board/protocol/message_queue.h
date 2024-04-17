#ifndef __MESSAGE_QUEUE_H
#define __MESSAGE_QUEUE_H

#define QUEUE_SIZE  200

typedef struct uart_tx_queue
{
    unsigned char state;
    unsigned char data;
    struct uart_tx_queue *next;
}UART_TX_QUEUE;


//由于对其原因，即使用了char类型，总长度还是会占用12字节
typedef struct uart_rx_queue
{
    int len:31;
    int state:1;
    struct uart_rx_queue *next;
    unsigned char *data;
}UART_RX_QUEUE;

int UART_TX_Queue_Init(UART_TX_QUEUE *FIFO, unsigned int len);
int MQ_Init(UART_RX_QUEUE *MQ, unsigned int len);


#endif


