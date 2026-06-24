/*
 * display.c
 *
 *  Created on: Jun 23, 2026
 *      Author: Luke Fadel
 *
 *  Created for ILI9488 display driver
 *  https://www.mouser.com/pdfDocs/ILI9488_Data_Sheet_100.pdf
 */

#include "image.h"
#include "main.h"
#include "memory.h"
#include <display-ili9488.h>
#include <stdbool.h>

static ImageTransferState_t state;

//lookup table for possible 3 byte packages
static const uint8_t table[256][3] = { { 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x07 }, { 0x00, 0x00, 0x38 }, { 0x00, 0x00, 0x3f }, {
				0x00, 0x01, 0xc0 }, { 0x00, 0x01, 0xc7 }, { 0x00, 0x01, 0xf8 },
		{ 0x00, 0x01, 0xff }, { 0x00, 0x0e, 0x00 }, { 0x00, 0x0e, 0x07 }, {
				0x00, 0x0e, 0x38 }, { 0x00, 0x0e, 0x3f }, { 0x00, 0x0f, 0xc0 },
		{ 0x00, 0x0f, 0xc7 }, { 0x00, 0x0f, 0xf8 }, { 0x00, 0x0f, 0xff }, {
				0x00, 0x70, 0x00 }, { 0x00, 0x70, 0x07 }, { 0x00, 0x70, 0x38 },
		{ 0x00, 0x70, 0x3f }, { 0x00, 0x71, 0xc0 }, { 0x00, 0x71, 0xc7 }, {
				0x00, 0x71, 0xf8 }, { 0x00, 0x71, 0xff }, { 0x00, 0x7e, 0x00 },
		{ 0x00, 0x7e, 0x07 }, { 0x00, 0x7e, 0x38 }, { 0x00, 0x7e, 0x3f }, {
				0x00, 0x7f, 0xc0 }, { 0x00, 0x7f, 0xc7 }, { 0x00, 0x7f, 0xf8 },
		{ 0x00, 0x7f, 0xff }, { 0x03, 0x80, 0x00 }, { 0x03, 0x80, 0x07 }, {
				0x03, 0x80, 0x38 }, { 0x03, 0x80, 0x3f }, { 0x03, 0x81, 0xc0 },
		{ 0x03, 0x81, 0xc7 }, { 0x03, 0x81, 0xf8 }, { 0x03, 0x81, 0xff }, {
				0x03, 0x8e, 0x00 }, { 0x03, 0x8e, 0x07 }, { 0x03, 0x8e, 0x38 },
		{ 0x03, 0x8e, 0x3f }, { 0x03, 0x8f, 0xc0 }, { 0x03, 0x8f, 0xc7 }, {
				0x03, 0x8f, 0xf8 }, { 0x03, 0x8f, 0xff }, { 0x03, 0xf0, 0x00 },
		{ 0x03, 0xf0, 0x07 }, { 0x03, 0xf0, 0x38 }, { 0x03, 0xf0, 0x3f }, {
				0x03, 0xf1, 0xc0 }, { 0x03, 0xf1, 0xc7 }, { 0x03, 0xf1, 0xf8 },
		{ 0x03, 0xf1, 0xff }, { 0x03, 0xfe, 0x00 }, { 0x03, 0xfe, 0x07 }, {
				0x03, 0xfe, 0x38 }, { 0x03, 0xfe, 0x3f }, { 0x03, 0xff, 0xc0 },
		{ 0x03, 0xff, 0xc7 }, { 0x03, 0xff, 0xf8 }, { 0x03, 0xff, 0xff }, {
				0x1c, 0x00, 0x00 }, { 0x1c, 0x00, 0x07 }, { 0x1c, 0x00, 0x38 },
		{ 0x1c, 0x00, 0x3f }, { 0x1c, 0x01, 0xc0 }, { 0x1c, 0x01, 0xc7 }, {
				0x1c, 0x01, 0xf8 }, { 0x1c, 0x01, 0xff }, { 0x1c, 0x0e, 0x00 },
		{ 0x1c, 0x0e, 0x07 }, { 0x1c, 0x0e, 0x38 }, { 0x1c, 0x0e, 0x3f }, {
				0x1c, 0x0f, 0xc0 }, { 0x1c, 0x0f, 0xc7 }, { 0x1c, 0x0f, 0xf8 },
		{ 0x1c, 0x0f, 0xff }, { 0x1c, 0x70, 0x00 }, { 0x1c, 0x70, 0x07 }, {
				0x1c, 0x70, 0x38 }, { 0x1c, 0x70, 0x3f }, { 0x1c, 0x71, 0xc0 },
		{ 0x1c, 0x71, 0xc7 }, { 0x1c, 0x71, 0xf8 }, { 0x1c, 0x71, 0xff }, {
				0x1c, 0x7e, 0x00 }, { 0x1c, 0x7e, 0x07 }, { 0x1c, 0x7e, 0x38 },
		{ 0x1c, 0x7e, 0x3f }, { 0x1c, 0x7f, 0xc0 }, { 0x1c, 0x7f, 0xc7 }, {
				0x1c, 0x7f, 0xf8 }, { 0x1c, 0x7f, 0xff }, { 0x1f, 0x80, 0x00 },
		{ 0x1f, 0x80, 0x07 }, { 0x1f, 0x80, 0x38 }, { 0x1f, 0x80, 0x3f }, {
				0x1f, 0x81, 0xc0 }, { 0x1f, 0x81, 0xc7 }, { 0x1f, 0x81, 0xf8 },
		{ 0x1f, 0x81, 0xff }, { 0x1f, 0x8e, 0x00 }, { 0x1f, 0x8e, 0x07 }, {
				0x1f, 0x8e, 0x38 }, { 0x1f, 0x8e, 0x3f }, { 0x1f, 0x8f, 0xc0 },
		{ 0x1f, 0x8f, 0xc7 }, { 0x1f, 0x8f, 0xf8 }, { 0x1f, 0x8f, 0xff }, {
				0x1f, 0xf0, 0x00 }, { 0x1f, 0xf0, 0x07 }, { 0x1f, 0xf0, 0x38 },
		{ 0x1f, 0xf0, 0x3f }, { 0x1f, 0xf1, 0xc0 }, { 0x1f, 0xf1, 0xc7 }, {
				0x1f, 0xf1, 0xf8 }, { 0x1f, 0xf1, 0xff }, { 0x1f, 0xfe, 0x00 },
		{ 0x1f, 0xfe, 0x07 }, { 0x1f, 0xfe, 0x38 }, { 0x1f, 0xfe, 0x3f }, {
				0x1f, 0xff, 0xc0 }, { 0x1f, 0xff, 0xc7 }, { 0x1f, 0xff, 0xf8 },
		{ 0x1f, 0xff, 0xff }, { 0xe0, 0x00, 0x00 }, { 0xe0, 0x00, 0x07 }, {
				0xe0, 0x00, 0x38 }, { 0xe0, 0x00, 0x3f }, { 0xe0, 0x01, 0xc0 },
		{ 0xe0, 0x01, 0xc7 }, { 0xe0, 0x01, 0xf8 }, { 0xe0, 0x01, 0xff }, {
				0xe0, 0x0e, 0x00 }, { 0xe0, 0x0e, 0x07 }, { 0xe0, 0x0e, 0x38 },
		{ 0xe0, 0x0e, 0x3f }, { 0xe0, 0x0f, 0xc0 }, { 0xe0, 0x0f, 0xc7 }, {
				0xe0, 0x0f, 0xf8 }, { 0xe0, 0x0f, 0xff }, { 0xe0, 0x70, 0x00 },
		{ 0xe0, 0x70, 0x07 }, { 0xe0, 0x70, 0x38 }, { 0xe0, 0x70, 0x3f }, {
				0xe0, 0x71, 0xc0 }, { 0xe0, 0x71, 0xc7 }, { 0xe0, 0x71, 0xf8 },
		{ 0xe0, 0x71, 0xff }, { 0xe0, 0x7e, 0x00 }, { 0xe0, 0x7e, 0x07 }, {
				0xe0, 0x7e, 0x38 }, { 0xe0, 0x7e, 0x3f }, { 0xe0, 0x7f, 0xc0 },
		{ 0xe0, 0x7f, 0xc7 }, { 0xe0, 0x7f, 0xf8 }, { 0xe0, 0x7f, 0xff }, {
				0xe3, 0x80, 0x00 }, { 0xe3, 0x80, 0x07 }, { 0xe3, 0x80, 0x38 },
		{ 0xe3, 0x80, 0x3f }, { 0xe3, 0x81, 0xc0 }, { 0xe3, 0x81, 0xc7 }, {
				0xe3, 0x81, 0xf8 }, { 0xe3, 0x81, 0xff }, { 0xe3, 0x8e, 0x00 },
		{ 0xe3, 0x8e, 0x07 }, { 0xe3, 0x8e, 0x38 }, { 0xe3, 0x8e, 0x3f }, {
				0xe3, 0x8f, 0xc0 }, { 0xe3, 0x8f, 0xc7 }, { 0xe3, 0x8f, 0xf8 },
		{ 0xe3, 0x8f, 0xff }, { 0xe3, 0xf0, 0x00 }, { 0xe3, 0xf0, 0x07 }, {
				0xe3, 0xf0, 0x38 }, { 0xe3, 0xf0, 0x3f }, { 0xe3, 0xf1, 0xc0 },
		{ 0xe3, 0xf1, 0xc7 }, { 0xe3, 0xf1, 0xf8 }, { 0xe3, 0xf1, 0xff }, {
				0xe3, 0xfe, 0x00 }, { 0xe3, 0xfe, 0x07 }, { 0xe3, 0xfe, 0x38 },
		{ 0xe3, 0xfe, 0x3f }, { 0xe3, 0xff, 0xc0 }, { 0xe3, 0xff, 0xc7 }, {
				0xe3, 0xff, 0xf8 }, { 0xe3, 0xff, 0xff }, { 0xfc, 0x00, 0x00 },
		{ 0xfc, 0x00, 0x07 }, { 0xfc, 0x00, 0x38 }, { 0xfc, 0x00, 0x3f }, {
				0xfc, 0x01, 0xc0 }, { 0xfc, 0x01, 0xc7 }, { 0xfc, 0x01, 0xf8 },
		{ 0xfc, 0x01, 0xff }, { 0xfc, 0x0e, 0x00 }, { 0xfc, 0x0e, 0x07 }, {
				0xfc, 0x0e, 0x38 }, { 0xfc, 0x0e, 0x3f }, { 0xfc, 0x0f, 0xc0 },
		{ 0xfc, 0x0f, 0xc7 }, { 0xfc, 0x0f, 0xf8 }, { 0xfc, 0x0f, 0xff }, {
				0xfc, 0x70, 0x00 }, { 0xfc, 0x70, 0x07 }, { 0xfc, 0x70, 0x38 },
		{ 0xfc, 0x70, 0x3f }, { 0xfc, 0x71, 0xc0 }, { 0xfc, 0x71, 0xc7 }, {
				0xfc, 0x71, 0xf8 }, { 0xfc, 0x71, 0xff }, { 0xfc, 0x7e, 0x00 },
		{ 0xfc, 0x7e, 0x07 }, { 0xfc, 0x7e, 0x38 }, { 0xfc, 0x7e, 0x3f }, {
				0xfc, 0x7f, 0xc0 }, { 0xfc, 0x7f, 0xc7 }, { 0xfc, 0x7f, 0xf8 },
		{ 0xfc, 0x7f, 0xff }, { 0xff, 0x80, 0x00 }, { 0xff, 0x80, 0x07 }, {
				0xff, 0x80, 0x38 }, { 0xff, 0x80, 0x3f }, { 0xff, 0x81, 0xc0 },
		{ 0xff, 0x81, 0xc7 }, { 0xff, 0x81, 0xf8 }, { 0xff, 0x81, 0xff }, {
				0xff, 0x8e, 0x00 }, { 0xff, 0x8e, 0x07 }, { 0xff, 0x8e, 0x38 },
		{ 0xff, 0x8e, 0x3f }, { 0xff, 0x8f, 0xc0 }, { 0xff, 0x8f, 0xc7 }, {
				0xff, 0x8f, 0xf8 }, { 0xff, 0x8f, 0xff }, { 0xff, 0xf0, 0x00 },
		{ 0xff, 0xf0, 0x07 }, { 0xff, 0xf0, 0x38 }, { 0xff, 0xf0, 0x3f }, {
				0xff, 0xf1, 0xc0 }, { 0xff, 0xf1, 0xc7 }, { 0xff, 0xf1, 0xf8 },
		{ 0xff, 0xf1, 0xff }, { 0xff, 0xfe, 0x00 }, { 0xff, 0xfe, 0x07 }, {
				0xff, 0xfe, 0x38 }, { 0xff, 0xfe, 0x3f }, { 0xff, 0xff, 0xc0 },
		{ 0xff, 0xff, 0xc7 }, { 0xff, 0xff, 0xf8 }, { 0xff, 0xff, 0xff }, };

//static const uint8_t speed[65535] = { 0 };
//uint8_t speedCount = 0;

// LCD hardware Reset, active low
void ILI9488_RESET(void) {
	// setting the reset pin to low to signal a reset
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin,
			GPIO_PIN_RESET);

	// small delay
	HAL_Delay(10);

	// setting the pin to high (default state)
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);

	HAL_Delay(100);
}

// LCD chip select signal, active low
void ILI9488_SELECT(void) {
	// setting the select pin to low
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
}

void ILI9488_DESELECT(void) {
	// setting the select pin to high
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
}

// sends cmd to lcd
void ILI9488_CMD(SPI_HandleTypeDef *spi, uint8_t cmd) {
	// setting DC pin to command mode (low)
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);

	// selecting SPI device
	ILI9488_SELECT();

	// using SPI to transmit data
	HAL_SPI_Transmit(spi, &cmd, 1, HAL_MAX_DELAY);

	// deselecting SPI device
	ILI9488_DESELECT();
}

// sends data to lcd
void ILI9488_DATA(SPI_HandleTypeDef *spi, uint8_t *data, uint16_t size) {
	// setting DC pin to command mode (high)
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

	// selecting SPI device
	ILI9488_SELECT();

	// using SPI to transmit data
	HAL_SPI_Transmit(spi, data, size, HAL_MAX_DELAY);

	// deselecting SPI device
	ILI9488_DESELECT();
}

// initializes the ST7796S
void ILI9488_INIT(SPI_HandleTypeDef *spi) {

	HAL_Delay(200);
	// hardware reset
	ILI9488_RESET();
	HAL_Delay(200);

	// software reset
	ILI9488_CMD(spi, 0x01);
	HAL_Delay(120);

	// exit sleep mode
	ILI9488_CMD(spi, 0x11);
	HAL_Delay(120);

	// backlight on
	HAL_GPIO_WritePin(DISPLAY_LED_GPIO_Port, DISPLAY_LED_Pin, GPIO_PIN_SET);

	// configuring extended command set for spi write
	ILI9488_CMD(spi, 0xF7);
	uint8_t unlockData[2] = { 0xA9, 0x51, 0x2C, 0x82 };
	ILI9488_DATA(spi, &unlockData[0], 2);

	// memory data access control - instruction 36h MADCTL
	ILI9488_CMD(spi, 0x36);
	// same as st7796s
	uint8_t madctl = 0x20;
	ILI9488_DATA(spi, &madctl, 1);

	// Interface Pixel Format - instruction 3Ah COLMOD
	ILI9488_CMD(spi, 0x3A);
	// Lowest available is 3bit/pixel
	// 00000001
	uint8_t colmod = 0x01;
	ILI9488_DATA(spi, &colmod, 1);

	// Column inversion for display longevity
	// 1 dot mode
//	ILI9488_CMD(spi, 0xB4);
//	uint8_t inversion = 0x01;
//	ILI9488_DATA(spi, &inversion, 1);

	// display on
	ILI9488_CMD(spi, 0x29);
}

// testing function to see fill direction and measure min refresh speed
void ILI9488_FILL(SPI_HandleTypeDef *spi) {
	// set column address command
	ILI9488_CMD(spi, 0x2A);
	// parameters: starting col MSB, starting col LSB, ending col MSB, ending col
	// LSB
	uint8_t caset[] = {

	(uint8_t) (0),

	(uint8_t) (0),

	(uint8_t) (ILI9488_WIDTH >> 8),

	(uint8_t) (ILI9488_WIDTH & 0xFF) };

	ILI9488_DATA(spi, caset, 4);

	// set page address command
	ILI9488_CMD(spi, 0x2B);
	// parameters: starting row MSB, starting row LSB, ending row MSB, ending row
	// LSB
	uint8_t paset[] = {

	(uint8_t) (0),

	(uint8_t) (0),

	(uint8_t) (ILI9488_HEIGHT >> 8),

	(uint8_t) (ILI9488_HEIGHT & 0xFF) };

	ILI9488_DATA(spi, paset, 4);

	// write data command
	ILI9488_CMD(spi, 0x2C);

	// setting to data mode
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

	// selecting spi device
	ILI9488_SELECT();

	uint8_t data[] = { 0xFF, 0xFF, 0xFF, 0 };
	uint8_t padded = 238;
	for (uint32_t r = 0; r < 28800; r++) {
		HAL_SPI_Transmit(spi, &data[0], 2, HAL_MAX_DELAY);
	}
//	HAL_SPI_Transmit_DMA(spi, speed, sizeof(speed));

//	speedCount--;
	ILI9488_DESELECT();
}

//// speed testing function for ILI9488_FILL
//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
//	if (hspi->Instance == SPI1) {
//		ILI9488_DESELECT();
//	}
//}

// displays what's on the cloneBuffer
bool ILI9488_REFRESH(SPI_HandleTypeDef *spi) {
	if (!state.currentlyDrawing) {
		// setting status to busy

		state.currentlyDrawing = true;

		// set column address command
		ILI9488_CMD(spi, 0x2A);
		// parameters: starting col MSB, starting col LSB, ending col MSB, ending
		// col LSB
		uint8_t caset[] = {

		(uint8_t) (0),

		(uint8_t) (0),

		(uint8_t) ((ILI9488_WIDTH - 1) >> 8),

		(uint8_t) ((ILI9488_WIDTH - 1) & 0xFF) };

		ILI9488_DATA(spi, caset, 4);

		// set row address command
		ILI9488_CMD(spi, 0x2B);
		// parameters: starting row MSB, starting row LSB, ending row MSB, ending
		// row LSB
		uint8_t raset[] = {

		(uint8_t) (0),

		(uint8_t) (0),

		(uint8_t) ((ILI9488_HEIGHT - 1) >> 8),

		(uint8_t) ((ILI9488_HEIGHT - 1) & 0xFF) };

		ILI9488_DATA(spi, raset, 4);

		// write data command
		ILI9488_CMD(spi, 0x2C);

		// setting to data mode
		HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

		// selecting spi device
		ILI9488_SELECT();
		// sending image data. chunking data for DMA and memory saving purposes

		for (uint32_t i = 0; i < sizeof(state.screenCopy); i++) {
			HAL_SPI_Transmit(spi, &table[state.screenCopy[i]][0], 3,
			HAL_MAX_DELAY);
		}
		ILI9488_DESELECT();
		state.currentlyDrawing = false;
		return true;
	} else {
		return false;
	}
}

bool ILI9488_WRITE(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y,
		Image_t *image, bool overWrite) {
	if (!state.currentlyDrawing) {
		// setting status to busy

		state.currentlyDrawing = true;
		// set column address command
		ILI9488_CMD(spi, 0x2A);
		// parameters: starting col MSB, starting col LSB, ending col MSB, ending
		// col LSB
		uint8_t caset[] =
				{

				(uint8_t) (x >> 8),

				(uint8_t) (x & 0xFF),

				(uint8_t) ((
						x + image->width - 1 > ILI9488_WIDTH - 1 ?
								ILI9488_WIDTH - 1 : x + image->width - 1) >> 8),

						(uint8_t) ((
								x + image->width - 1 > ILI9488_WIDTH - 1 ?
										ILI9488_WIDTH - 1 : x + image->width - 1)
								& 0xFF)

				};

		ILI9488_DATA(spi, caset, 4);

		// set row address command
		ILI9488_CMD(spi, 0x2B);
		// parameters: starting row MSB, starting row LSB, ending row MSB, ending
		// row LSB
		uint8_t raset[] = {

		(uint8_t) (y >> 8),

		(uint8_t) (y & 0xFF),

		(uint8_t) ((
				y + image->height - 1 > ILI9488_HEIGHT - 1 ?
						ILI9488_HEIGHT - 1 : y + image->height - 1) >> 8),

		(uint8_t) ((
				y + image->height - 1 > ILI9488_HEIGHT - 1 ?
						ILI9488_HEIGHT - 1 : y + image->height - 1) & 0xFF) };

		ILI9488_DATA(spi, raset, 4);

		// decompressing image to cloned buffer

		// an index throughout the loop
		state.dcompImage_SIZE = 0;
		uint32_t count = 0;
		// iterating through compressed image
		for (uint32_t i = 0; i < image->size; i++) {
			// iterating through the current RLE value
			for (uint8_t j = 0; j < image->data[i]; j++) {
				if ((count % image->width) + x < ILI9488_WIDTH
						&& (count / image->width) + y < ILI9488_HEIGHT) {
					// calculating screen pixel index from current position within image
					uint32_t globalpos = ILI9488_WIDTH
							* (y + (count / image->width)) + x
							+ (count % image->width);

					if (i % 2) {
						// pixel is high, 1 % 2 = 1
						//  will always be high in overwrite and OR mode
						SET_PIXEL(state.screenCopy, globalpos);
					} else if (overWrite) {
						// pixel is low and overwrite is on. if in OR mode, just leave
						// current pixel as it is
						CLR_PIXEL(state.screenCopy, globalpos);
					}
					state.dcompImage_SIZE++;
				}
				count++;
			}
		}

		state.x = x;
		state.y = y;

		// TODO FIX OBOE HERE
		if (x + image->width > ILI9488_WIDTH) {
			state.width = ILI9488_WIDTH - x;
		} else {
			state.width = image->width;
		}

		if (y + image->height > ILI9488_HEIGHT) {
			state.height = ILI9488_HEIGHT - y;
		} else {
			state.height = image->height;
		}

		// write data command
		ILI9488_CMD(spi, 0x2C);

		// setting to data mode
		HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

		// selecting spi device
		ILI9488_SELECT();

		// double buffering

		// sending image data. chunking data for DMA and memory saving purposes
		state.imageProgress = 0;

		state.imageTarget = state.dcompImage_SIZE * 2;

		state.activeBuf = 0;
		// checking if chunking is required or not
		if (state.imageTarget <= CHUNK) {
			// not required

			// expanding to buffer
			for (uint32_t i = 0; i < state.dcompImage_SIZE; i++) {
				// converting image space index to screen space index
				uint32_t globalpos =
				ILI9488_WIDTH * (y + (i / state.width)) + x + (i % state.width);

				// expanding bit to 2 bytes as per the color specification
				state.buf[state.activeBuf][i * 2] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][i * 2 + 1] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
			}

			HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf],
					state.imageTarget);
			state.imageProgress = state.imageTarget;
		} else {
			// required

			// expanding to buffer
			for (uint32_t i = 0; i < CHUNK / 2; i++) {
				// converting image space index to screen space index
				uint32_t globalpos = (ILI9488_WIDTH * (y + (i / state.width)))
						+ x + (i % state.width);

				// expanding bit to 2 bytes as per the color specification
				state.buf[state.activeBuf][i * 2] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][i * 2 + 1] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
			}

			HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf], CHUNK);
			state.imageProgress += CHUNK;
			// loading the next chunk of the image into ram for faster transfer

			state.activeBuf = !state.activeBuf;
			// debug clock
			state.startTime = HAL_GetTick();
			uint32_t shift = state.imageProgress / 2;
			uint8_t *current = state.buf[state.activeBuf];
			uint16_t imgRow = shift / state.width;
			uint16_t imgCol = shift % state.width;
			state.THING = 1;
			for (uint32_t i = 0;
					i
							< (state.imageTarget - state.imageProgress > CHUNK ?
									CHUNK :
									state.imageTarget - state.imageProgress) / 2;
					i++) {

				uint32_t globalpos =
				ILI9488_WIDTH * (state.y + imgRow) + state.x + (imgCol);

				// expanding bit to 2 bytes as per the color specification
				uint8_t color =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;

				*current++ = color;
				*current++ = color;

				if (++imgCol == state.width) {
					imgCol = 0;
					++imgRow;
				}
			}
			state.THING = 0;
			state.finalTime = HAL_GetTick() - state.startTime;
		}

		return true;
	} else {
		// called if function is already drawing when called
		return false;
	}
}

// callback that is called when HAL_SPI_Transmit_DMA finishes
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == SPI1) {
		if (state.imageProgress >= state.imageTarget) {
			// done drawing
			state.finalTime = HAL_GetTick() - state.startTime;
			ILI9488_DESELECT();
			state.currentlyDrawing = 0;
			return;
		} else if (state.imageTarget - state.imageProgress < CHUNK) {
			// partial chunk
			HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf],
					state.imageTarget - state.imageProgress);
			// done drawing criteria
			state.imageProgress += CHUNK;
		} else {
			// full chunk
			state.imageProgress += CHUNK;
			HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf], CHUNK);
			uint32_t shift = state.imageProgress / 2;
			uint8_t *current = state.buf[state.activeBuf];
			uint16_t imgRow = shift / state.width;
			uint16_t imgCol = shift % state.width;
			// loading the next chunk of the image into ram for faster transfer, and
			// swapping buffers
			state.activeBuf = !state.activeBuf;
			for (uint32_t i = 0; i < CHUNK / 2; i++) {
				// checking if image is in bounds

				uint32_t globalpos =
				ILI9488_WIDTH * (state.y + imgRow) + state.x + (imgCol);

				// expanding bit to 2 bytes as per the color specification
				uint8_t color =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;

				*current++ = color;
				*current++ = color;

				if (++imgCol == state.width) {
					imgCol = 0;
					++imgRow;
				}
			}
		}
	}
}

