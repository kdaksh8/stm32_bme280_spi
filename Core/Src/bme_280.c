/*
 * bme_280.c
 *
 *  Created on: May 9, 2026
 *      Author: daksh
 */

#include "bme_280.h"
//#include "main.h"

uint8_t chip_id;
uint8_t tempread[8];
uint8_t config[2];
uint8_t comp[32];
volatile int temperature_raw, pressure_raw, humidity_raw = 0;
float finaltemp = 0;
uint32_t finalpressure, final_humidity = 0;
unsigned char dig_H1, dig_H3;
signed char dig_H6;
unsigned short dig_T1, dig_P1;
signed short dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, dig_H2, dig_H4, dig_H5;


void bme280_select()
{
	  HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_RESET);
}

void bme280_deselect()
{
	  HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_SET);
}

uint8_t bme280_read_register(uint8_t reg)
{
	uint8_t tx_data = reg | 0x80; // Set MSB for read operation
	uint8_t rx_data = 0;

	bme280_select();
	HAL_SPI_Transmit(&hspi1, &tx_data, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi1, &rx_data, 1, HAL_MAX_DELAY);
	bme280_deselect();

	return rx_data;
}

void bme280_write_register(uint8_t reg, uint8_t data)
{
	uint8_t tx_data[2] = {reg & 0x7F, data}; // Clear MSB for write operation

	bme280_select();
	HAL_SPI_Transmit(&hspi1, tx_data, 2, HAL_MAX_DELAY);
	bme280_deselect();
}

void bme280_config()
{
//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_RESET); //SET CS LOW
	bme280_select();
	config[0] = CTRLMEASREG;
	config[1] = CTRLMEASVAL;
	HAL_SPI_Transmit(&hspi1, config, 2, 1000); //CONFIG
	bme280_deselect();
	//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_RESET); //SET CS LOW
	bme280_select();
	config[0] = CONFIGREG;
	config[1] = CONFIGVAL;
	HAL_SPI_Transmit(&hspi1, config, 2, 1000); //CONFIG
	bme280_deselect();
//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_RESET); //SET CS LOW
	bme280_select();
	config[0] = CTRLHUMREG;
	config[1] = CTRLHUMVAL;
	HAL_SPI_Transmit(&hspi1, config, 2, 1000); //CONFIG
	bme280_deselect();
//	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_SET);
	HAL_Delay(10);
}

void bme280_get_comp_val(void)
{
//	HAL_GPIO_WritePin(BME280_CS_PORT, GPIO_PIN_11, GPIO_PIN_RESET); //SET CS LOW
	bme280_select();
	config[0] = COMPTEMPPRES;
	HAL_SPI_Transmit(&hspi1, config, 1, 10);
	HAL_SPI_Receive(&hspi1, comp, 24, 120);
	bme280_deselect();
//	HAL_GPIO_WritePin(BME280_CS_PORT, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_Delay(10);

	dig_T1 = (comp[0])+(comp[1]<<8);
	dig_T2 = (comp[2])+(comp[3]<<8);
	dig_T3 = (comp[4])+(comp[5]<<8);
	dig_P1 = (comp[6])+(comp[7]<<8);
	dig_P2 = (comp[8])+(comp[9]<<8);
	dig_P3 = (comp[10])+(comp[11]<<8);
	dig_P4 = (comp[12])+(comp[13]<<8);
	dig_P5 = (comp[14])+(comp[15]<<8);
	dig_P6 = (comp[16])+(comp[17]<<8);
	dig_P7 = (comp[18])+(comp[19]<<8);
	dig_P8 = (comp[20])+(comp[21]<<8);
	dig_P9 = (comp[22])+(comp[23]<<8);

	config[0] = COMPHUMINIT;
//	HAL_GPIO_WritePin(BME280_CS_PORT, GPIO_PIN_11, GPIO_PIN_RESET); //SET CS LOW
	bme280_select();
	HAL_SPI_Transmit(&hspi1, config, 1, 10);
	HAL_SPI_Receive(&hspi1, &comp[24], 1, 120);
	bme280_deselect();
//	HAL_GPIO_WritePin(BME280_CS_PORT, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_Delay(10);
	dig_H1 = comp[24];

	config[0] = COMPHUMREST;
	bme280_select();
//	HAL_GPIO_WritePin(BME280_CS_PORT, GPIO_PIN_11, GPIO_PIN_RESET); //SET CS LOW
	HAL_SPI_Transmit(&hspi1, config, 1, 10);
	HAL_SPI_Receive(&hspi1, &comp[25], 7, 120);
	bme280_deselect();
//	HAL_GPIO_WritePin(BME280_CS_PORT, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_Delay(10);
	dig_H2 = (comp[25])+(comp[26]<< 8);
	dig_H3 = comp[27];
	dig_H4 = (comp[28] << 4) +(comp[29] & 0xF);
	dig_H5 = (comp[29] & 0xF0) +(comp[30]<< 4);
	dig_H6 = comp[31];
}

void bme280_get_raw_val()
{
	bme280_config();
	bme280_select();
	HAL_GPIO_WritePin(BME280_CS_PORT, BME280_CS_PIN, GPIO_PIN_RESET); //SET CS LOW
	config[0] = RAWREAD;
	HAL_SPI_Transmit(&hspi1, config, 1, 10); //GET ID
	HAL_SPI_Receive(&hspi1, tempread, 8, 10);
	bme280_deselect();

	temperature_raw =(tempread[3]<<12)+(tempread[4]<<4)+(tempread[5]>>4);
	pressure_raw = (tempread[0]<<12)+(tempread[1]<<4)+(tempread[2]>>4);
	humidity_raw = (tempread[6] << 8) + (tempread[7]);
}
void bme_cal_final_val()
{
	int var1, var2, t_fine;
	var1 = ((((temperature_raw >> 3) - ((int)dig_T1 << 1))) * ((int)dig_T2)) >> 11;
	var2 = (((((temperature_raw >> 4) - ((int)dig_T1)) * ((temperature_raw >> 4) - ((int)dig_T1))) >> 12) * ((int)dig_T3)) >> 14;
	t_fine = (var1 + var2);
	finaltemp = (t_fine * 5 + 128) >> 8;

	finaltemp = finaltemp/100;

	var1 = (((int)t_fine) >> 1) - 64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11 ) * ((int)dig_P6);
	var2 = var2 + ((var1 * ((int)dig_P5)) << 1);
	var2 = (var2 >> 2) + (((int) dig_P4) << 16);
	var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13 )) >> 3) + ((((int) dig_P2) * var1) >> 1 )) >> 18;
	var1 = ((((32768 + var1)) * ((int)dig_P1)) >> 15);
	if (var1 == 0)
	{
		finalpressure = 0;
	}
	else{
		finalpressure = (((uint32_t) (((int)1048576)-pressure_raw) - (var2 >> 12))) * 3125;
		if (finalpressure < 0x80000000){
			finalpressure = (finalpressure << 1) / (( uint32_t)var1);
		}
		else{
			finalpressure = (finalpressure / (uint32_t)var1) * 2;
		}
		var1 = (((int)dig_P9) * ((int) ((( finalpressure >> 3) * ( finalpressure >> 3)) >> 13))) >> 12;
		var2 = (((int) (finalpressure >> 2)) * ((int)dig_P8)) >> 13;
		finalpressure = ((uint32_t)((int)finalpressure + ((var1 + var2 + dig_P7) >> 4)))/100; //kPA
	}

	var1 = (t_fine - ((int) 76800));
	var1 = (((((humidity_raw << 14) - (((int) dig_H4) << 20) - (((int)dig_H5) * var1)) + ((int) 16384)) >> 15) * \
	(((((((var1 * ((int) dig_H6)) >> 10) * (((var1 * ((int) dig_H3)) >> 11) + ((int) 32768))) >> 10) + \
	((int) 2097152)) * ((int) dig_H2) + 8192) >> 14));

	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int)dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419330400 : var1);
	final_humidity = (var1 >> 12)/1024;
}
