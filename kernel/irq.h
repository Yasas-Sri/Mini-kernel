#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void irq_handler_c(uint8_t irq);

#ifdef __cplusplus
}
#endif

#endif
