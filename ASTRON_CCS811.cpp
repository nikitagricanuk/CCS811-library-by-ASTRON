#include <ASTRON_CCS811.h>
#include <Wire.h>
#include <cmath>

CCS811::CCS811(int delay, int addr, int mode) {
	_address = addr;
	_delay = delay;
	_mode = mode;
}

bool CCS811::begin(void) {
	uint8_t sw_reset[] = {0x11,0xE5,0x72,0x8A}; //regiregisters for reset, restart sensor

	setDelay(_delay); //setting I2C delay

	SendI2C(CCS811_SW_RESET, 4, sw_reset); //reset before setup (see CCS811 Datasheet page 22)
	delayMicroseconds(CCS811_WAIT_AFTER_RESET_US);
	//Sutup is described in Programming and Interfacing Guide pages 7-13, scheme is shown on page 29
	ReadI2C(CCS811_HW_ID_REG, 1, i2c_buff); //Check Hardware Id (see CCS811 Datasheet page 19)
	if (i2c_buff[0] != 0x81) return false; //If error - return false

	ReadI2C(CCS811_STATUS_REG, 1, i2c_buff); //Check sensor status (see CCS811 Datasheet page 15)
	if (!(i2c_buff[0] & 0x10)) return false; //If error - return false

	SendI2C(CCS811_APP_START_REG, 0, i2c_buff); //Turn on the measurement mode (see CCS811 Datasheet page 24)
	delayMicroseconds(CCS811_WAIT_AFTER_APPSTART_US);

	i2c_buff[0] = _mode | INTERRUPT;
	SendI2C(CCS811_MEAS_MODE_REG, 1, i2c_buff); //Setting mode (see CCS811 Datasheet pages 8,16)
	return true;
}

uint16_t CCS811::readCO2(void) {
	ccs811_measurement_t measure_item; //Declaring structure
	memset(&measure_item, 0, sizeof(measure_item)); //Filling structure with zeros

	ReadI2C(CCS811_ALG_RESULT_DATA, sizeof(measure_item), (uint8_t*)&measure_item); //Read data from sensor (see CCS811 Datasheet page 17)
	measure_item.eco2 = NTOHS(measure_item.eco2); //convert to understandable units
	return measure_item.eco2; //return to user
}

//Doesn`t work
uint16_t CCS811::readCO2mg(void) {
	ccs811_measurement_t measure_item;
	memset(&measure_item, 0, sizeof(measure_item));

	ReadI2C(CCS811_ALG_RESULT_DATA, sizeof(measure_item), (uint8_t*)&measure_item);
	measure_item.eco2 = NTOHS(measure_item.eco2);
	double co2_mg = 0.0409 * measure_item.eco2 * 44.01;
	return co2_mg;
}

uint16_t CCS811::readTVOC(void) {
	ccs811_measurement_t measure_item;
	memset(&measure_item, 0, sizeof(measure_item));

	ReadI2C(CCS811_ALG_RESULT_DATA, sizeof(measure_item), (uint8_t*)&measure_item);
	measure_item.tvoc = NTOHS(measure_item.tvoc);
	return measure_item.tvoc;
}

void CCS811::setDelay(uint8_t time) {
	if (time <= 0) time = 0; //if value is standart or less than zero - doesn`t change
	_delayi2c = time;
}

bool CCS811::SendI2C(int regaddr, int count, const uint8_t* buffer) {
	Wire.beginTransmission(_address);                        // Send ACK start bit
	Wire.write(regaddr);                                     // Register address
	for (int i = 0; i < count; i++) Wire.write(buffer[i]);   // Write bytes
	int r = Wire.endTransmission(true);                      // Send ACK stop bit
	return r == 0;
}

bool CCS811::ReadI2C(int regaddr, int count, uint8_t* buffer) {
	Wire.beginTransmission(_address);                       // Send ACK start bit
	Wire.write(regaddr);                                      // Register address
	int wres = Wire.endTransmission(false);                   // Repeated START
	delayMicroseconds(_delayi2c);                          // Wait
	int rres = Wire.requestFrom(_address, count);           // From CCS811, read bytes, STOP
	for (int i = 0; i < count; i++) buffer[i] = Wire.read();
	return (wres == 0) && (rres == count);
}