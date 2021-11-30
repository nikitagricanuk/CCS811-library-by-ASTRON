/*
* Creted by ASTRON by nikitagricanuk 2021 Nov 30
* If you are going to study this libraly, I am recommend print out the following documents and keep them close to you.
* List of docs:
* CCS811 Datasheet
* CCS811 Programming and Interfacing Guide
*/

#ifndef ASTRON_CCS811_h
#define ASTRON_CCS811_h
#include <Arduino.h>

#if BYTE_ORDER == BIG_ENDIAN

#define HTONS(n) (n)
#define NTOHS(n) (n)

#else

#define HTONS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))
#define NTOHS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#endif

//If it doesn`t work - switch places
#define ADDRESS 0x5A
#define ALT_ADDRESS 0x5B

// Timings
#define CCS811_WAIT_AFTER_RESET_US     2000 
#define CCS811_WAIT_AFTER_APPSTART_US  1000 

// CCS811 registers/mailboxes
#define CCS811_STATUS_REG       0x00
#define CCS811_MEAS_MODE_REG    0x01
#define CCS811_ALG_RESULT_DATA  0x02 
#define CCS811_HW_ID_REG        0x20
#define CCS811_APP_START_REG    0xF4 
#define CCS811_SW_RESET         0xFF 

#define CCS811_MODE_IDLE        0x0
#define CCS811_MODE_1SEC        0x10
#define CCS811_MODE_10SEC       0x20
#define CCS811_MODE_60SEC       0x30

#define INTERRUPT               0x8
#define THRESHOLDS_ENABLED      0x4

//Structure for saving data
typedef struct {
	uint16_t eco2; //CO2
	uint16_t tvoc; //TVOC (Total Volatile Organic Compounds)
	//Check status, errors and other data
	uint8_t status;
	uint8_t error_id;
	uint16_t raw_data;
} ccs811_measurement_t;

class CCS811
{
public:
	CCS811(int delay = 0, int addr = ADDRESS, int mode = CCS811_MODE_1SEC); //constructor
	bool begin(void); //Begin function (see CCS811 Programming and Interfacing Guide page 29)
	uint16_t readCO2(void); //Reading CO2 value
	uint16_t readCO2mg(void); //Reading CO2 value and convert to mg/m3
	uint16_t readTVOC(void); //Reading TVOC value (Total Volatile Organic Compounds)
private:
	bool SendI2C(int regaddr, int count, const uint8_t* buffer); //Send data using the I2C bus
	bool ReadI2C(int regaddr, int count, uint8_t* buffer); //Read data using the I2C bus
	void setDelay(uint8_t time); //Need for some MCUs (Example: ESP8266)
	//System values
	int _delayi2c = 0;
	int _address;
	uint8_t i2c_buff[8];
	int _mode;
	int _delay;
};

#endif