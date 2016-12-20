//NOTHING WORKS AND I HATE MY LIFE

/*#include <Wire.h>
#include <I2CEEPROM.h>

void WireRequestArray(int address, uint8_t* buffer, uint8_t amount);
void WireWriteRegister(int address, uint8_t reg, uint8_t value);
void WireWriteByte(int address, uint8_t value);

static uint8_t const  EEPROM = 0x54; //default -> A2 currently = 1

void I2CInit() {
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    HWREG(I2C0_BASE + I2C_O_MCR) |= 0x01;
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
    I2CMasterEnable(I2C0_BASE);  
}*/
