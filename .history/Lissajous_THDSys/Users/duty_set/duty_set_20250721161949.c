#include "duty_set.h"


void SetDuty(uint16_t duty)
{
    float fduty = (float)duty / 100.0f;
    float V = DDS_OUT_AMP * (1 - 2 * fduty);
}







