/*
 * display.h
 *
 *  Created on: Jun 17, 2026
 *      Author: Luke Fadel
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

void DISPLAY_RESET(void);
void DISPLAY_SELECT(void);
void DISPLAY_DESELECT(void);
void DISPLAY_CMD(SPI_HandleTypeDef *spi, uint8_t cmd);
void DISPLAY_DATA(SPI_HandleTypeDef *spi, uint8_t cmd, uint16_t size);
void DISPLAY_OR(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y, uint8_t *image);
void DISPLAY_OVERWRITE(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y, uint8_t *image);

#endif /* INC_DISPLAY_H_ */
