


#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "ds1307.h"
#include "i2c.h"
#include "matrix_keypad.h"
#include "adc.h"
#include "uart.h"
#include "eeprom.h"

unsigned char key;
extern char str[3] , str_user[5];
extern unsigned char time[9];
unsigned char gear[7][3] = { "ON" , "G1" , "G2" , "G3" , "G4" , "RG" , "CO"};
unsigned char event[10][17];
int ret , log = 0 , count = 0;

void add_event(void)
{
    int i = 0 , j = 0;

    if(log < 10)
    {

        for(j=0 ; time[j] != '\0' ; j++)
        {
            event[log][i++] = time[j];
        }
        event[log][i++] = ' ';


        for(j=0 ; gear[count][j] != '\0' ; j++)
        {
            event[log][i++] = gear[count][j];
        }
        event[log][i++] = ' ';


        for(j=0 ; str[j] != '\0' ; j++)
        {
            event[log][i++] = str[j];
        }

        event[log][i] = '\0';
        log++;
    }


    if(log == 10)
        log = 0;
}

void set_time(void)
{
    unsigned long wait = 0 , flag = 0  ;
    char clock_reg[3];
    unsigned char time_key , prekey  , time[10];
    CLEAR_DISP_SCREEN;
    char secs , mins , hour, SEC , MIN , HOUR;



    hour = (read_ds1307(HOUR_ADDR) / 10) << 4 & 0x01;
    hour = ((read_ds1307(HOUR_ADDR) % 10) & 0x0f) | hour;
    mins = (read_ds1307(MIN_ADDR) / 10) << 4;
    mins = ((read_ds1307(MIN_ADDR) % 10) & 0x0f) | mins;
    secs = (read_ds1307(SEC_ADDR) / 10) << 4;
    secs = ((read_ds1307(SEC_ADDR) % 10) & 0x0f) | secs;

    while(1)
    {

        clcd_print("TIME SET" , LINE1(4));
        time_key = read_switches(STATE_CHANGE);
        prekey = read_switches(LEVEL_CHANGE);


        if(time_key == MK_SW11)
        {
            if(flag == 0)
            {

                if(secs++ == 59)
                    secs  = 0;
            }

            if(flag == 1)
            {

                if(mins++ == 59)
                    mins = 0;
            }

            if(flag == 2)
            {

                if(hour++ == 12)
                    hour = 0;
            }
        }


        if(time_key == MK_SW12)
        {
            if(flag == 0)
            {

                if(secs-- == 0)
                    secs  = 59;
            }

            if(flag == 1)
            {

                if(mins-- == 0)
                    mins = 59;
            }

            if(flag == 2)
            {

                if(hour-- == 0)
                    hour = 12;
            }
        }


        if(prekey == MK_SW9 )
        {
            wait = 0;
            flag++;
            if(flag > 2)
                flag = 0;
        }


        if(time_key == MK_SW10)
        {
            CLEAR_DISP_SCREEN;
            break;
        }


        SEC = convert_to_hexa(secs);
        MIN = convert_to_hexa(mins);
        HOUR = convert_to_hexa(hour);


        write_ds1307(SEC_ADDR , SEC | 0x00);
        write_ds1307(MIN_ADDR, MIN | 0x00);
        write_ds1307(HOUR_ADDR , HOUR | 0x00);

        for(unsigned long wait = 100000; wait--;);
        clcd_putch(hour / 10 + '0', LINE2(1));
        clcd_putch(hour % 10 + '0', LINE2(2));
        clcd_putch(':', LINE2(3));
        clcd_putch(mins / 10 + '0', LINE2(4));
        clcd_putch(mins % 10 + '0', LINE2(5));
        clcd_putch(':', LINE2(6));
        clcd_putch(secs / 10 + '0', LINE2(7));
        clcd_putch(secs % 10 + '0', LINE2(8));
    }
}

char convert_to_hexa( char value)
{
    char i, j = 0,rem, num;
    rem = value % 10;
    num = value / 10;
    value = rem;
    value = num << 4 | value;    
    return value;
}

void download_log(void)
{
    char log_key;
    CLEAR_DISP_SCREEN;
    clcd_print("Downloading LOGS" , LINE1(0));
    clcd_print("in UART" , LINE2(5));

    puts("Logs are Downloaded\n\r");
    puts("Logs ae : \n\r");


    for(int i = 0 ; i < 10 ; i++)
    {
        puts(event[i]);
        puts("\n\r");
    }


    while(1)
    {
        log_key = read_switches(STATE_CHANGE);

        if(log_key == MK_SW10)
        {
            CLEAR_DISP_SCREEN;
            break;
        }
    }

}

void change_password(void)
{
    unsigned char pwd_key;
    int change = 0  , j = 0;
    CLEAR_DISP_SCREEN;
    clcd_print("Enter Password  ",LINE1(0));

    while(1)
    {
        pwd_key = read_switches(STATE_CHANGE);

        if( change < 4)
        {

            if(pwd_key == MK_SW11)
            {
                if(change == 0)
                    write_internal_eeprom(0x00,'1');
                if( change == 1)
                    write_internal_eeprom(0x01,'1');
                if(change == 2)
                    write_internal_eeprom(0x02,'1');
                if(change == 3)
                    write_internal_eeprom(0x03,'1');
                clcd_putch('*' , LINE2(j++));
                change++;
            }


            else if(pwd_key == MK_SW12)
            {
                if(change == 0)
                    write_internal_eeprom(0x00,'0');
                if( change == 1)
                    write_internal_eeprom(0x01,'0');
                if( change== 2)
                    write_internal_eeprom(0x02,'0');
                if( change== 3)
                    write_internal_eeprom(0x03,'0');
                clcd_putch('*' , LINE2(j++));
                change++;
            }
        }


        if(change == 4)
        {
            clcd_print("Password is set  ",LINE2(0));
            str_user[0] = read_internal_eeprom(0x00);
            str_user[1] = read_internal_eeprom(0x01);
            str_user[2] = read_internal_eeprom(0x02);
            str_user[3] = read_internal_eeprom(0x03);
            str_user[4] = '\0';
        }


        if(pwd_key == MK_SW10)
        {
            CLEAR_DISP_SCREEN;
            break;	
        }
    }
}


void clear_log(void)
{
    unsigned char clear_key;
    int i  , j;


    for(i = 0 ; i < 10 ; i++)
    {
        for(j = 0 ; j < 18 ; j++)
        {
            event[i][j] = 0;
        }
    }

    CLEAR_DISP_SCREEN;
    clcd_print("LOGS CLEARED" , LINE1(2));
    while(1)
    {
        clear_key = read_switches(STATE_CHANGE);


        if(clear_key == MK_SW10)
        {
            CLEAR_DISP_SCREEN;
            break;
        }
    }
}

void view_log(void)
{
    int i = 0;

    while(1)
    {
        key = read_switches(STATE_CHANGE);


        if(key == MK_SW11)
        {
            i++;
            if(i == 10)
                i = 0;
        }


        if(key == MK_SW12)
        {
            i--;
            if(i == -1)
                i = 9;
        }


        if(key == MK_SW10)
        {
            CLEAR_DISP_SCREEN;
            break;
        }


        clcd_print("   TIME   GR  SP  " , LINE1(0));
        clcd_print(event[i] , LINE2(0));
    }

}



void init_config(void)
{
    init_clcd();
    init_i2c();
    init_ds1307();
    init_matrix_keypad();
    init_adc();
    init_uart();    

}

void main(void)
{
    init_config();

    while (1)
    {
        clcd_print("   TIME   GR  SP  " , LINE1(0));

        get_time();
        display_time();


        speed_adc();

        key = read_switches(STATE_CHANGE);


        if(key == MK_SW1)
        {
            count = 6;
            add_event();
        }


        if(key == MK_SW2)
        {
            count++;
            if(count > 5)
                count = 1;
            add_event();
        }


        if(key  == MK_SW3)
        {
            count--;
            if(count < 1)
                count =5;
            add_event();
        }

        clcd_print(gear[count] , LINE2(10));


        if(key == MK_SW11)
        {
            CLEAR_DISP_SCREEN;
            ret = password();
            if(ret == 1)
            {
                display_menu();
            }
        }			

    }
}
