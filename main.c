#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include <math.h>
//void I2C_SLAVE_HANDLER();
void ISR();

/**
 * main.c
 */

uint32_t TxData = 0x00;
uint32_t sine_digitized = 0;
int main(void)
{
    INIT_GPIO_PORTF_REGISTERS();
    INIT_GPIO_PORTB_REGISTERS();
    INIT_GPIO_PORTA_REGISTERS();

    SYSCTL_RCGCI2C_R = 0x03;
    NVIC_EN1_R = 0x00000020; // NVIC enable for I2C module 1
    // slave rx
    I2C1_MCR_R = 0x20;
    I2C1_SOAR_R = 0x3B;
    I2C1_SCSR_R = 0x01;
    I2C1_SIMR_R = 0x01; // interrupt enabled for data rx at slave

    NVIC_ST_RELOAD_R = 160; // 0.01 ms
    NVIC_ST_CURRENT_R = 0x00;
    NVIC_ST_CTRL_R = 0x00000007;

    // master tx
    I2C0_MCR_R = 0x10;
    I2C0_MTPR_R = 0x09;
    I2C0_MSA_R = 0x00;// initializing


    uint8_t slave_address = 0x60;

    while(1){
        sine_digitized = 2048.0*sin(2*3.14*TxData/4096) + 2047.0;
        TxDAC(slave_address, 2, sine_digitized);

    }
    return 0;
}

void INIT_GPIO_PORTF_REGISTERS(){

    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x1F;             /* make PORTF configurable */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 4 : 0 pins */
    GPIO_PORTF_DIR_R = 0x0E;            /*  */
    GPIO_PORTF_PUR_R = 0x11;            /* PORTF0 and PORTF4 are pulled up */

}

void INIT_GPIO_PORTB_REGISTERS(){

    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTB_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTB_DEN_R = 0x0C;
    GPIO_PORTB_AFSEL_R = 0x0C;
    GPIO_PORTB_ODR_R = 0x08;
    GPIO_PORTB_PCTL_R = 0x3300;
    GPIO_PORTB_PUR_R = 0x0C;
    //GPIODIR
}

void INIT_GPIO_PORTA_REGISTERS(){

    SYSCTL_RCGCGPIO_R |= 0x01;
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTA_DEN_R = 0xC0;
    GPIO_PORTA_AFSEL_R = 0xC0;
    GPIO_PORTA_ODR_R = 0x80;
    GPIO_PORTA_PCTL_R = 0x33000000;
//    GPIO_PORTA_PUR_R = 0xC0;
    //GPIODIR
}

void TxDAC(uint8_t Slave_Addr, int n_bytes, uint32_t data){
    uint32_t LSByte = (data & 0xFF);
    data = data >> 8;
    data |= (LSByte << 8);

    int num_bytes_sent = 0;
    I2C0_MSA_R = (Slave_Addr << 1);
    I2C0_MDR_R = (data & 0xFF);
    if (n_bytes == 1){
        I2C0_MCS_R = 0x07;
        num_bytes_sent ++;
    }
    else{
        I2C0_MCS_R = 0x03;
    }
    while (num_bytes_sent < n_bytes){
        num_bytes_sent ++;

        while(I2C0_MCS_R & 0x01){
            ;// wait till BUSBSY bit of MCS is cleared
        }
        if (I2C0_MCS_R & 0x02){ // check for error bit in MCS
            GPIO_PORTF_DATA_R = 0x02;
            return;
        }

        I2C0_MDR_R = ( (data >> 8*num_bytes_sent) & 0xFF );
        if (num_bytes_sent < n_bytes){
            I2C0_MCS_R = 0x01;
        }
        else{
            I2C0_MCS_R = 0x05;
            while(I2C0_MCS_R & 0x01){
                ;// wait till BUSBSY bit of MCS is cleared
            }
            if (I2C0_MCS_R & 0x02){ // check for error bit in MCS
                GPIO_PORTF_DATA_R = 0x02;
            }
        }
    }
}

void I2C_SLAVE_HANDLER(){
    if(I2C1_SDR_R == 0x01){
        GPIO_PORTF_DATA_R = 0x0A;
    }

    else if(I2C1_SDR_R == 0x02){
        GPIO_PORTF_DATA_R = 0x04;
    }
    else if(I2C1_SDR_R == 0x03){
        GPIO_PORTF_DATA_R = 0x08;
    }
    I2C1_SICR_R = 0x01;
//    I2C1_SIMR_R = 0x01; // interrupt enabled for data rx at slave
}

void ISR(){
    GPIO_PORTF_DATA_R ^= 0x04;
    TxData += 1;

    if(TxData >= 4096){
        TxData = 0;
    }
//    NVIC_ST_CURRENT_R = 16000*500;
}
