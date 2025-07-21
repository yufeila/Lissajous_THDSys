#include "duty_set.h"


void SetDuty(uint16_t duty)
{
    AD9833_SetFrequencyQuick(1000.0,AD9833_OUT_SINUS);
}







