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

