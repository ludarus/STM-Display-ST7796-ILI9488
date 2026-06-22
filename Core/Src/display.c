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
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);

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
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

	//selecting SPI device
	DISPLAY_SELECT();

	//using SPI to transmit data
	HAL_SPI_Transmit(spi, data, size, HAL_MAX_DELAY);

	//deselecting SPI device
	DISPLAY_DESELECT();
}

void DISPLAY_UNLOCK_EXTENDED(SPI_HandleTypeDef *spi) {
	// unlocking extended command set
	DISPLAY_CMD(spi, 0xF0);
	uint8_t unlockKeys[2] = { 0xC3, 0x96 };
	DISPLAY_DATA(spi, &unlockKeys[0], 1);
	DISPLAY_CMD(spi, 0xF0);
	DISPLAY_DATA(spi, &unlockKeys[1], 1);
}

void DISPLAY_LOCK_EXTENDED(SPI_HandleTypeDef *spi) {
	// locking extended command set
	DISPLAY_CMD(spi, 0xF0);
	uint8_t lockKeys[2] = { 0xC3, 0x69 };
	DISPLAY_DATA(spi, &lockKeys[0], 1);
	DISPLAY_CMD(spi, 0xF0);
	DISPLAY_DATA(spi, &lockKeys[1], 1);
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

	// backlight on
	HAL_GPIO_WritePin(DISPLAY_LED_GPIO_Port, DISPLAY_LED_Pin, GPIO_PIN_SET);

	// unlocking extended command set
//	DISPLAY_UNLOCK_EXTENDED(spi);

// memory data access control - instruction 36h MADCTL
	DISPLAY_CMD(spi, 0x36);
	uint8_t madctl = 0x20;
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

	// locking extended command set
//	DISPLAY_LOCK_EXTENDED(spi);

	// display on
	DISPLAY_CMD(spi, 0x29);

}

void DISPLAY_FILL(SPI_HandleTypeDef *spi) {
	// set column address command
	DISPLAY_CMD(spi, 0x2A);
	// parameters: starting col MSB, starting col LSB, ending col MSB, ending col LSB
	uint8_t caset[] = {

	(uint8_t) (0),

	(uint8_t) (0),

	(uint8_t) (1),

	(uint8_t) (224) };

	DISPLAY_DATA(spi, caset, 4);

	// set row address command
	DISPLAY_CMD(spi, 0x2B);
	// parameters: starting row MSB, starting row LSB, ending row MSB, ending row LSB
	uint8_t raset[] = {

	(uint8_t) (0),

	(uint8_t) (0),

	(uint8_t) (1),

	(uint8_t) (64) };

	DISPLAY_DATA(spi, raset, 4);

	// write data command
	DISPLAY_CMD(spi, 0x2C);

	// setting to data mode
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

	// selecting spi device
	DISPLAY_SELECT();
	uint8_t data[2] = { 0xFF, 0xFF };
	for (uint32_t r = 0; r < 320 * 480; r++) {
		HAL_SPI_Transmit(spi, data, 2, HAL_MAX_DELAY);
	}

	DISPLAY_DESELECT();
}

// TODO: add bounds to x and y
void DISPLAY_WRITE(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y,
		Image_t *image, bool overWrite) {
	if (!state.currentlyDrawing) {

		state.x = x;
		state.y = y;

		state.width = image->width;
		state.height = image->height;

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

		// decompressing image to cloned buffer

		//size of decompressed image in bits, should equal width * height. acts as an index throughout the loop
		state.dcompImage_SIZE = 0;
		// iterating through compressed image
		for (uint32_t i = 0; i < image->size; i++) {
			// iterating through the current RLE value
			for (uint8_t j = 0; j < image->data[i]; j++) {
				// calculating screen pixel index from current position within image
				uint32_t globalpos = 480
						* (y + (state.dcompImage_SIZE / image->width)) + x
						+ (state.dcompImage_SIZE % image->width);

				if (i % 2) {
					//pixel is high, 1 % 2 = 1
					// will always be high in overwrite and OR mode
					SET_PIXEL(state.screenCopy, globalpos);
				} else {
					//pixel is low
					if (overWrite) {
						CLR_PIXEL(state.screenCopy, globalpos);
					} else {
						OR_PIXEL(state.screenCopy, globalpos, 0);
					}
				}
				state.dcompImage_SIZE++;
			}
		}

		// should be true already (with possible off by one error)
//		state.dcompImage_SIZE = image->width * image->height;

		// write data command
		DISPLAY_CMD(spi, 0x2C);

		// setting to data mode
		HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

		// selecting spi device
		DISPLAY_SELECT();

		// double buffering

		//sending image data. chunking data for DMA and memory saving purposes
		state.imageProgress = 0;
		state.imageTarget = state.dcompImage_SIZE * 2;

		//setting status to busy - maybe move this to earlier in func
		state.currentlyDrawing = true;

		//debug clock
		state.startTime = HAL_GetTick();

		state.activeBuf = 0;
		//checking if chunking is required or not
		if (state.imageTarget <= CHUNK) {
			//not required

//			//expanding to buffer
			for (uint32_t i = 0; i < state.dcompImage_SIZE; i++) {
				// converting image space index to screen space index
				uint32_t globalpos = 480 * (y + (i / image->width)) + x
						+ (i % image->width);

				//expanding bit to 2 bytes as per the color specification
				state.buf[state.activeBuf][i] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][i + 1] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
			}

//			uint8_t data[2] = { 0, 0 };
//				for (uint32_t r = 0; r < state.dcompImage_SIZE; r++) {
//					HAL_SPI_Transmit(spi, data, 2, HAL_MAX_DELAY);
//				}

			HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf],
					state.imageTarget);
			state.imageProgress = state.imageTarget;
		} else {
			//required

			//expanding to buffer
			for (uint32_t i = 0; i < CHUNK / 2; i++) {
				// converting image space index to screen space index
				uint32_t globalpos = 480 * (y + (i / image->width)) + x
						+ (i % image->width);

				//expanding bit to 2 bytes as per the color specification
				state.buf[state.activeBuf][i] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][i + 1] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;

			}

			HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf], CHUNK);
			state.activeBuf = !state.activeBuf;
			state.imageProgress += CHUNK;
			//loading the next chunk of the image into ram for faster transfer
			for (uint32_t i = 0; i < state.imageProgress / 2; i++) {
				uint32_t globalpos = 480
						* (y + ((i + (state.imageProgress / 2)) / image->width))
						+ x + ((i + state.imageProgress / 2) % image->width);

				//expanding bit to 2 bytes as per the color specification
				state.buf[state.activeBuf][i] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][i + 1] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;

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
			for (uint32_t i = 0; i < state.imageProgress / 2; i++) {
				uint32_t globalpos = 480
						* (state.y + ((i + (state.imageProgress / 2)) / state.width))
						+ state.x + ((i + state.imageProgress / 2) % state.width);

				//expanding bit to 2 bytes as per the color specification
				state.buf[state.activeBuf][i] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
				state.buf[state.activeBuf][i + 1] =
				GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;

			}

		}
	}

}
