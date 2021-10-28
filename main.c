////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// PROJECT INIT /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define   __PIC24FJ64GA002__
#define _XTAL_FREQ   8000000
#define FCY _XTAL_FREQ/2

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// PROJECT LIBRARIES //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/i2c1_driver.h"
#include "INA219_LIB.h"

// INA219 Typedef Struct 
INA219 ina219;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// STANDARD LIBRARIES /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libpic30.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////// I/O PINOUT AND DEFINITIONS //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// INPUTS ////////////////////////////////////

/////////////////////////////////// OUTPUTS ////////////////////////////////////

// PAPER SENSOR RELAY
#define I2C_CONTROL_DIR	TRISBbits.TRISB13
#define I2C_CONTROL		LATBbits.LATB13

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// CONSTANTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// DEFINITIONS //////////////////////////////////

#define INPUT		1
#define OUTPUT		0

#define ON			1
#define OFF			0

#define HIGH		1
#define LOW			0

// SERVOMOTOR CONST (Values in usecs))
#define SERVO_MIN_WIDTH_PULSE   1
#define SERVO_MAX_WIDTH_PULSE   2

// EMG1 MINIMUM AND MAXIMUM (FROM 0 to 1023)
#define EMG1_MINIMUM_VALUE      200
#define EMG1_MAXIMUM_VALUE      800

// EMG2 MINIMUM AND MAXIMUM (FROM 0 to 1023)
#define EMG2_MINIMUM_VALUE      300
#define EMG2_MAXIMUM_VALUE      700

#define EMG_SENS_1              0x00
#define EMG_SENS_2              0x01

#define SERVO_CLOCKWISE         500     // (1ms   * 9999) / 20mS = 499.95
#define SERVO_NEUTRAL           749     // (1.5ms * 9999) / 20mS = 749.925
#define SERVO_COUNTERCLOCKWISE  1000    // (2ms   * 9999) / 20mS = 999.9

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VARIABLES  ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int x;

uint32_t EMG_SENS_1_Val, 
        EMG_SENS_2_Val;
//
//uint32_t PWM_Temp_Servo_Value_Ch1, 
//         PWM_Temp_Servo_Value_Ch2, 
//         PWM_Temp_Servo_Value_Ch3,
//         PWM_Temp_Servo_Value_Ch4,
//         PWM_Temp_Servo_Value_Ch5;

unsigned char buffer[128];

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROTOTYPE FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//extern inline static void ADC1_ChannelSelect( ADC1_CHANNEL channel );

void delay_ms(long delay_value);

void INA219_One_sensor_Measurement();
void INA219_Measurement_Stage();

void ADC_Channels_Test();

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// MAIN FUNCTION /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    OC1_SecondaryValueSet(SERVO_CLOCKWISE);
    OC2_SecondaryValueSet(SERVO_CLOCKWISE);
    OC3_SecondaryValueSet(SERVO_NEUTRAL);
    OC4_SecondaryValueSet(SERVO_COUNTERCLOCKWISE);
    OC5_SecondaryValueSet(SERVO_COUNTERCLOCKWISE);
        
    while(1);
    
    do{
        EMG_SENS_1_Val  = ADC1_GetConversion(EMG_SENS_1);
    }while(EMG_SENS_1_Val < EMG1_MAXIMUM_VALUE);
    
    ADC_Channels_Test();
    
    INA219_Measurement_Stage();
    
    while (1)
    {
        // Data Adquisition
        EMG_SENS_1_Val  = ADC1_GetConversion(EMG_SENS_1);
        EMG_SENS_2_Val  = ADC1_GetConversion(EMG_SENS_2);
        
        // PWM Update Values
        
        OC1_SecondaryValueSet(0);
        OC2_SecondaryValueSet(200);
        OC3_SecondaryValueSet(500);
        OC4_SecondaryValueSet(800);
        OC5_SecondaryValueSet(1000);
        
        delay_ms(200);
        
    }

    return 1;
}

void ADC_Channels_Test(){
    
    do{
        EMG_SENS_1_Val  = ADC1_GetConversion(EMG_SENS_1);
        EMG_SENS_2_Val  = ADC1_GetConversion(EMG_SENS_2);

        printf("\nchannel_AN0_Val  = %lu", EMG_SENS_1_Val);
        printf("\nchannel_AN1_Val  = %lu", EMG_SENS_2_Val);
        printf("\n\n");
        delay_ms(200);
    }while(1);
    
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// AUX FUNCTIONS /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////// GENERAL DELAY FUNCTION ////////////////////////////

void delay_ms(long delay_value){
    long x;
    for(x=0; x < delay_value; x++) __delay_ms(1);
}

void INA219_Measurement_Stage(){

    i2c1_driver_driver_open();
    
    I2C_CONTROL_DIR = OUTPUT;
        
    I2C_CONTROL = LOW;
    
    ina219_begin(&ina219, INA219_ADDRESS_1);
    delay_ms(5);
    ina219_begin(&ina219, INA219_ADDRESS_2);
    delay_ms(5);
    ina219_begin(&ina219, INA219_ADDRESS_3);
    delay_ms(5);
    ina219_begin(&ina219, INA219_ADDRESS_4);
    delay_ms(5);
    
    I2C_CONTROL = HIGH;
    
    ina219_begin(&ina219, INA219_ADDRESS_5);
    delay_ms(5);
    
    float current_mA[6];
    
    while(true){
        
        I2C_CONTROL = LOW;
        
        ina219_setCalibration_16V_400mA(&ina219, INA219_ADDRESS_1);
        delay_ms(5);
        ina219_setCalibration_16V_400mA(&ina219, INA219_ADDRESS_2);
        delay_ms(5);
        ina219_setCalibration_16V_400mA(&ina219, INA219_ADDRESS_3);
        delay_ms(5);
        ina219_setCalibration_16V_400mA(&ina219, INA219_ADDRESS_4);
        delay_ms(5);
        
        I2C_CONTROL = HIGH;
        
        ina219_setCalibration_16V_400mA(&ina219, INA219_ADDRESS_5);
        delay_ms(5);
        
        I2C_CONTROL = LOW;
        
        current_mA[1] = ina219_getShuntVoltage_mV(&ina219, INA219_ADDRESS_1) * 10;
        delay_ms(5);
        current_mA[2] = ina219_getShuntVoltage_mV(&ina219, INA219_ADDRESS_2) * 10;
        delay_ms(5);
        current_mA[3] = ina219_getShuntVoltage_mV(&ina219, INA219_ADDRESS_3) * 10;
        delay_ms(5);
        current_mA[4] = ina219_getShuntVoltage_mV(&ina219, INA219_ADDRESS_4) * 10;
        delay_ms(5);
        
        I2C_CONTROL = HIGH;
        
        current_mA[5] = ina219_getShuntVoltage_mV(&ina219, INA219_ADDRESS_5) * 10;
        delay_ms(5);

        printf("I1:    %.2f mA\n", current_mA[1]);
        printf("I2:    %.2f mA\n", current_mA[2]);
        printf("I3:    %.2f mA\n", current_mA[3]);
        printf("I4:    %.2f mA\n", current_mA[4]);
        printf("I5:    %.2f mA\n", current_mA[5]);
        printf("\n");
        delay_ms(200);
    
    }
}


void INA219_One_sensor_Measurement(){

    i2c1_driver_driver_open();
    
    ina219_begin(&ina219, INA219_ADDRESS_1);
    delay_ms(5);
    
    float current_mA[6];
    
    while(true){
        
        ina219_setCalibration_16V_400mA(&ina219, INA219_ADDRESS_1);
        delay_ms(5);
        
        current_mA[1] = ina219_getShuntVoltage_mV(&ina219, INA219_ADDRESS_1) * 10;
        delay_ms(5);

        printf("I1:    %.2f mA\n", current_mA[1]);
        printf("\n");
        delay_ms(500);
    
    }

}
