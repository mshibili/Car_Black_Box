#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "ds1307.h"
#include "i2c.h"
#include "matrix_keypad.h"
#include "adc.h"


short adc_reg_val;
char str[3];

void speed_adc(void)
{
    adc_reg_val = ((read_adc(CHANNEL4)) / 10);

    if(adc_reg_val >= 100)
    {
        adc_reg_val = 99;
    }

    str[2] = '\0';

    for(int i=1; i>=0; i--)
    {
        str[i] = (adc_reg_val % 10) + '0';
        adc_reg_val = adc_reg_val / 10 ;
    }

    clcd_print(str, LINE2(14));
}


