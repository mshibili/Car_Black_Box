#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "ds1307.h"
#include "i2c.h"
#include "matrix_keypad.h"
#include "adc.h"

char str_user[5] = "0000";

int password(void)
{
    short i = 0;
    static bit flag = 0;
    char key;
    char str_pass[5]="";
    while(1)
    {
        key = read_switches(STATE_CHANGE);
        if(i < 4)
        {
            clcd_print("ENTER PASSWORD" , LINE1(0));
            if(key == MK_SW11)
            {
                str_pass[i++] = '1';                    
                clcd_putch('*' , LINE2(i));
            }
            if(key == MK_SW12)
            {
                str_pass[i++] = '0';
                clcd_putch('*' , LINE2(i));
            }	
        }

        if(i == 4)
        {
            str_pass[i] = '\0';
            flag = 1;
        }

        if(flag)
        {

            if(my_strcmp(str_user , str_pass) == 0)        
            {
                CLEAR_DISP_SCREEN;
                clcd_print("Password Entered" , LINE1(0));
                clcd_print("Successfully" , LINE2(4));
                for(long wait = 0 ; wait < 50000 ; wait++);
                return 1;
            }
            else 
            {
                CLEAR_DISP_SCREEN;
                clcd_print("Password Wrong" , LINE1(0));      

                for(long wait=0;wait<500000;wait++);
                {
                    i = 0;
                    CLEAR_DISP_SCREEN;
                }
            }

            flag = 0;
        }
    }
}

int my_strcmp(char *str1, char *str2)
{
    while(*str1 && *str2)                   
    {
        if(*str1 != *str2)               
            return *str1 - *str2;
        str1++;
        str2++;
    }
    return *str1 - *str2;
} 
