#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

/**
 * main.c
 */
int main(void)
{
    INIT_GPIO_PORTF_REGISTERS();
    INIT_GPIO_PORTB_REGISTERS();
    INIT_GPIO_PORTA_REGISTERS();
//    uint8_t slave_status = 0x00;

    SYSCTL_RCGCI2C_R = 0x03;

    // master rx
    I2C1_MCR_R = 0x10;
    I2C1_MSA_R = 0x76;
    I2C1_MTPR_R = 0x09;

    // master tx
    I2C0_MCR_R = 0x10;
    I2C0_MTPR_R = 0x09;
    I2C0_MSA_R = 0x76;

    while(1){

        I2C0_MDR_R = 0xAF;


        while(I2C0_MCS_R & 0x40){
            ;// wait till BUSBSY bit of MCS is cleared
        }
        I2C0_MCS_R = 0x07; /// initiate transmit

        if (I2C0_MCS_R & 0x04){ // check if any error was detected in last operation
            ;
        }
        if ((I2C0_MCS_R & 0x04) == 0x00){
            GPIO_PORTF_DATA_R = 0x08;
        }
        else if ((I2C0_MCS_R & 0x04) != 0x00){
            GPIO_PORTF_DATA_R = 0x02;
        }
////////////////////////////////////////////////////////////////////////////////////////////////////
        while(I2C1_MCS_R & 0x40){
            ;// wait till BUSBSY bit of MCS is cleared
        }
        I2C1_MCS_R = 0x07; /// initiate transmit

        if (I2C1_MCS_R & 0x04){ // check if any error was detected in last operation
            ;
        }
        if ((I2C1_MCS_R & 0x04) == 0x00){
            GPIO_PORTF_DATA_R = 0x0E;
            if (I2C1_MDR_R == 0xAF){
                GPIO_PORTF_DATA_R = 0x04;
            }
        }
        else if ((I2C1_MCS_R & 0x04) != 0x00){
            GPIO_PORTF_DATA_R = 0x02;
        }
    }
    return 0;
}

void INIT_GPIO_PORTF_REGISTERS(){
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_CR_R = 0x1F;             /* make PORTF configurable */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 4 : 0 pins */
    GPIO_PORTF_DIR_R = 0x0E;            /*  */
    GPIO_PORTF_PUR_R = 0x11;            /* PORTF0 and PORTF4 are pulled up */

}

void INIT_GPIO_PORTB_REGISTERS(){
    GPIO_PORTB_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTB_DEN_R = 0x0C;
    GPIO_PORTB_AFSEL_R = 0x0C;
    GPIO_PORTB_ODR_R = 0x08;
    GPIO_PORTB_PCTL_R = 0x3300;
    //GPIODIR
}

void INIT_GPIO_PORTA_REGISTERS(){
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    SYSCTL_RCGCGPIO_R |= 0x01;
    GPIO_PORTA_DEN_R = 0x0C;
    GPIO_PORTA_AFSEL_R = 0xC0;
    GPIO_PORTA_ODR_R = 0x80;
    GPIO_PORTA_PCTL_R = 0x33000000;
    //GPIODIR
}
