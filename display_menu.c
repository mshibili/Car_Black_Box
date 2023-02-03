#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "ds1307.h"
#include "i2c.h"
#include "matrix_keypad.h"
#include "adc.h"

char menu[5][17] = {"view log     " , "clear log       ", "set time        ", "change pwd       ", "download log    "};

unsigned int wait2 = 0;
void display_menu(void)
{
    static int flag = 1;
    unsigned char prekey , key;
    unsigned int i = 0;
    while(1)
    {
        if(i == 0 && flag)
        {
            clcd_print("* " , LINE1(0));
            clcd_print(menu[i] , LINE1(2));
            clcd_print(menu[i+1] , LINE2(2));
        }

        key = read_switches(STATE_CHANGE);
        prekey = read_switches(LEVEL_CHANGE);

        if(key == MK_SW11 && i <= 4 && flag)
        {


            if(i >= 5)
                i = 4;
            i++;

            if(i < 5)
            {
                clcd_print(menu[i-1] , LINE1(2));
                clcd_print(menu[i] , LINE2(2));
                clcd_print("  " , LINE1(0));
                clcd_print("* " , LINE2(0));
            }

            else
            {
                clcd_print(menu[3] , LINE1(2));
                clcd_print(menu[4] , LINE2(2));
                clcd_print("  " , LINE1(0));
                clcd_print("* " , LINE2(0));
            }


        }

        if(key  == MK_SW12 && i > 0 && flag)
        {
            if(i < 5)
            {
                clcd_print("* " , LINE1(0));
                clcd_print(menu[i-1] , LINE1(2));
                clcd_print("  " , LINE2(0));
                clcd_print(menu[i] , LINE2(2));
            }
            else
            {
                clcd_print(menu[3] , LINE1(2));
                clcd_print(menu[4] , LINE2(2));
                clcd_print("  " , LINE1(0));
                clcd_print("* " , LINE2(0));
            }

            i--;
            if(i < 0)
                i = 0;
        }

        if(prekey == MK_SW11 && wait2++ == 50000)
        {
            flag = 1;
            wait2 = 0;

            i--;
            if(i == 0)
            {
                view_log();
            }

            if(i == 1)
            {
                clear_log();
                i = 0;
            }

            if(i == 2)

            {
                set_time();
                i = 0;
            }

            if(i == 3)
            {
                change_password();
                i = 0;
            }

            if(i == 4)
            {
                download_log();
                i = 0;
            }

        }

        if(key == MK_SW10)
        {
            CLEAR_DISP_SCREEN;
            break;
        }
    }
}


