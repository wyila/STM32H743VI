#include "message_queue.h"
#include "hal_sys.h"

int UART_TX_Queue_Init(UART_TX_QUEUE *FIFO, unsigned int len)
{
    unsigned int i;
    UART_TX_QUEUE *node = FIFO;

    if((FIFO == 0) || (len == 0))
        return 0;
    for(i = 0; i < len; i++)
    {
        node->data = 0;
        node->state = 0;
        if(i != (len - 1))
            node->next = (node + 1);
        else
            node->next = FIFO;
        node ++;
    }
    return 1;
}


int MQ_Init(UART_RX_QUEUE *MQ, unsigned int len)
{
    unsigned int i;
    UART_RX_QUEUE *node = MQ;

    if((MQ == 0) || (len == 0))
        return 0;
    for(i = 0; i < len; i++)
    {
        node->len = 0;
        node->data = 0;
        node->state = 0;
        if(i != (len - 1))
            node->next = (node + 1);
        else
            node->next = MQ;
        node ++;
    }
    return 1;
}





