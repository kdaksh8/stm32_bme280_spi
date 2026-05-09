/*
 * bme_280.h
 *
 *  Created on: May 9, 2026
 *      Author: daksh
 */

#ifndef INC_BME_280_H_
#define INC_BME_280_H_

#include <inttypes.h>
#include "main.h"

#define BME280_CHIP_ID_REG 0xD0

#define BME280_CS_PIN GPIO_PIN_4
#define BME280_CS_PORT GPIOC

#define BME280_MISO_PIN GPIO_PIN_6
#define BME280_MISO_PORT GPIOA

#define BME280_SCK_PIN GPIO_PIN_5
#define BME280_SCK_PORT GPIOA

#define BME280_MOSI_PIN GPIO_PIN_7
#define BME280_MOSI_PORT GPIOA

/* BME registors */

#define BME280_RESET_REG 0xE0
#define BME280_ID_REG 0xD0
extern SPI_HandleTypeDef hspi1;

extern uint8_t chip_id;
extern uint8_t tempread[8];
extern uint8_t config[2];
extern uint8_t comp[32];
extern volatile int temperature_raw, pressure_raw, humidity_raw;
extern float finaltemp;
extern uint32_t finalpressure, final_humidity;
/*Function prototypes*/
void bme280_write_register(uint8_t reg, uint8_t data);
uint8_t bme280_read_register(uint8_t reg);
void bme280_select();
void bme280_deselect();

#endif /* INC_BME_280_H_ */
