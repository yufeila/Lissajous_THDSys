#include "duty_set.h"


void SetDuty(uint16_t duty)
{
    float fduty = (float)duty / 100.0f;
    float compare_vol = DDS_OUT_AMP * fduty;
}







