#ifndef __SPWM_H
#define __SPWM_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>

void SPWM_Init(uint16_t ccr0, uint16_t psc,uint16_t death);

#endif
