/*
 * display.c
 *
 *  Created on: Jun 17, 2026
 *      Author: Luke Fadel
 *
 *  Created for ST7796S display driver
 *  https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf
 */

#include "main.h"
#include <stdbool.h>
#include "image.h"
#include <memory.h>
#include "display.h"

static ImageTransferState_t state;

// LCD hardware Reset, active low
void DISPLAY_RESET(void) {
	//setting the reset pin to low to signal a reset
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin,
			GPIO_PIN_RESET);

	//small delay
	HAL_Delay(10);

	//setting the pin to high (default state)
	HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);

	HAL_Delay(100);
}

// LCD chip select signal, active low
void DISPLAY_SELECT(void) {
	//setting the select pin to low
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
}

void DISPLAY_DESELECT(void) {
	//setting the select pin to high
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
}

//sends cmd to lcd
void DISPLAY_CMD(SPI_HandleTypeDef *spi, uint8_t cmd) {
	//setting DC pin to command mode (high)
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

	//selecting SPI device
	DISPLAY_SELECT();

	//using SPI to transmit data
	HAL_SPI_Transmit(spi, &cmd, 1, HAL_MAX_DELAY);

	//deselecting SPI device
	DISPLAY_DESELECT();
}

//sends data to lcd
void DISPLAY_DATA(SPI_HandleTypeDef *spi, uint8_t *data, uint16_t size) {
	//setting DC pin to command mode (low)
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);

	//selecting SPI device
	DISPLAY_SELECT();

	//using SPI to transmit data
	HAL_SPI_Transmit(spi, data, size, HAL_MAX_DELAY);

	//deselecting SPI device
	DISPLAY_DESELECT();
}

// initializes the ST7796S
void DISPLAY_INIT(SPI_HandleTypeDef *spi) {

	HAL_Delay(200);
	// hardware reset
	DISPLAY_RESET();
	HAL_Delay(200);

	//software reset
	DISPLAY_CMD(spi, 0x01);
	HAL_Delay(120);

	// exit sleep mode
	DISPLAY_CMD(spi, 0x11);
	HAL_Delay(120);

	// NOTE: apparently an unlock sequence is required to use these commands

	// memory data access control - instruction 36h MADCTL
	DISPLAY_CMD(spi, 0x36);
	// MX = 0, MY = 0, MV = 0, ML = 0, RGB = 0, MH = 0
	uint8_t madctl = 0;
	DISPLAY_DATA(spi, &madctl, 1);

	// Interface Pixel Format - instruction 3Ah COLMOD
	DISPLAY_CMD(spi, 0x3A);
	// Lowest available is 16bit/pixel
	// 01010101
	uint8_t colmod = 0x55;
	DISPLAY_DATA(spi, &colmod, 1);

	// Column inversion for display longevity
	DISPLAY_CMD(spi, 0xB4);
	uint8_t inversion = 0x01;
	DISPLAY_DATA(spi, &inversion, 1);

	// NOTE: if unlock was required before, a lock is required now

	// display on
	DISPLAY_CMD(spi, 0x29);

}

void DISPLAY_WRITE(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y, Image_t *image, bool overWrite) {
	if (!state.currentlyDrawing) {

		// set column address command
		DISPLAY_CMD(spi, 0x2A);
		// parameters: starting col MSB, starting col LSB, ending col MSB, ending col LSB
		uint8_t caset[] = {

		(uint8_t) (x >> 8),

		(uint8_t) (x & 0xFF),

		(uint8_t) ((x + image->width - 1) >> 8),

		(uint8_t) ((x + image->width - 1) & 0xFF) };

		DISPLAY_DATA(spi, caset, 4);

		// set row address command
		DISPLAY_CMD(spi, 0x2B);
		// parameters: starting row MSB, starting row LSB, ending row MSB, ending row LSB
		uint8_t raset[] = { (uint8_t) (y >> 8), (uint8_t) (y & 0xFF),
				(uint8_t) ((uint8_t) (y + image->height - 1) >> 8),
				(uint8_t) ((y + image->height - 1) & 0xFF) };
		DISPLAY_DATA(spi, raset, 4);

		// resetting the decompiled image
		memset(state.dcompImage, 0, sizeof(state.dcompImage));
		// size in bits
		state.dcompImage_SIZE = 0;
		for (uint32_t i = 0; i < image->size; i++) {
			for (uint8_t j = 0; j < image->data[i]; j++) {
				if (i % 2) {
					//pixel is high, 1 % 2 = 1
					SET_PIXEL(state.dcompImage, state.dcompImage_SIZE);
				} else {
					//pixel is low
					CLR_PIXEL(state.dcompImage, state.dcompImage_SIZE);
				}
				state.dcompImage_SIZE++;
			}
		}

		// modifying the cloned buffer
		for (uint16_t h = 0; h < image->height; h++) {
			for (uint16_t w = 0; w < image->width; w++) {
				uint32_t globalpos = 480 * (y + h) + x + w;
				uint32_t localpos = (image->width * h) + w;
				if (overWrite) {
					if (GET_PIXEL(state.dcompImage, localpos)) {
						SET_PIXEL(state.screenCopy, globalpos);
					} else {
						CLR_PIXEL(state.screenCopy, globalpos);
					}
				} else {
					OR_PIXEL(state.screenCopy, globalpos,
							GET_PIXEL(state.dcompImage, localpos));
				}
			}
		}

		// write data command
		DISPLAY_CMD(spi, 0x2C);

		// setting to data mode
		HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);

		// selecting spi device
		DISPLAY_SELECT();

		// double buffering

		//sending image data. chunking data for DMA and memory saving purposes
		state.imageProgress = 0;
		state.imageTarget = state.dcompImage_SIZE * 2;

		//setting status to busy
		state.currentlyDrawing = true;

		//debug clock
		state.startTime = HAL_GetTick();

		state.activeBuf = 0;
		//checking if chunking is required or not
		if (state.imageTarget <= CHUNK) {
			//not required

			//expanding to buffer
			for (uint32_t i = 0; i < state.dcompImage_SIZE; i++) {
				state.buf[state.activeBuf][i * 2] = GET_PIXEL(state.dcompImage,
						i);
				state.buf[state.activeBuf][(i * 2) + 1] = 0;
			}

			HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf],
					state.imageTarget);
			state.imageProgress = state.imageTarget;
		} else {
			//required

			//expanding to buffer
			for (uint32_t i = 0; i < CHUNK / 2; i++) {
				state.buf[state.activeBuf][i * 2] = GET_PIXEL(state.dcompImage,
						i);
				state.buf[state.activeBuf][(i * 2) + 1] = 0;
			}

			HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf], CHUNK);
			state.activeBuf = !state.activeBuf;
			state.imageProgress += CHUNK;
			//loading the next chunk of the image into ram for faster transfer
			for (uint32_t i = state.imageProgress / 2;
					i < (CHUNK / 2) + state.imageProgress / 2; i++) {
				uint32_t localIdx = (i - (state.imageProgress / 2)) * 2; // local buffer offset
				state.buf[state.activeBuf][localIdx] =
						GET_PIXEL(state.dcompImage, i) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][localIdx + 1] =
						GET_PIXEL(state.dcompImage, i) ? ON_COLOR : OFF_COLOR;
			}
		}
	} else {
		// called if function is already drawing when called
	}
}

//callback that is called when HAL_SPI_Transmit_DMA finishes
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == SPI1) {
		if (state.imageProgress >= state.imageTarget) {
			//done drawing
			state.finalTime = HAL_GetTick() - state.startTime;
			DISPLAY_DESELECT();
			state.currentlyDrawing = 0;
			return;
		} else if (state.imageTarget - state.imageProgress < CHUNK) {
			//partial chunk
			HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf],
					state.imageTarget - state.imageProgress);
			//done drawing criteria
			state.imageProgress += CHUNK;
		} else {
			//full chunk
			HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf], CHUNK);
			state.imageProgress += CHUNK;
			//loading the next chunk of the image into ram for faster transfer, and swapping buffers
			state.activeBuf = !state.activeBuf;
			for (uint32_t i = state.imageProgress / 2;
					i < (CHUNK / 2) + state.imageProgress / 2; i++) {
				state.buf[state.activeBuf][i * 2] = GET_PIXEL(state.dcompImage,
						i);
				state.buf[state.activeBuf][(i * 2) + 1] = 0;
			}

		}
	}

}
