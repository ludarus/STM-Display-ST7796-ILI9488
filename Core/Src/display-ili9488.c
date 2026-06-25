/*
 * display.c
 *
 *  Created on: Jun 23, 2026
 *      Author: Luke Fadel
 *
 *  Created for ILI9488 display driver
 *  https://www.mouser.com/pdfDocs/ILI9488_Data_Sheet_100.pdf
 */

// includes
#include "image.h"
#include "main.h"
#include <display-ili9488.h>
#include <stdbool.h>

// state so main functions and callbacks can all access render state
static ImageTransferState_t state;

// lookup table for possible 3 byte packages
static const uint32_t table[256] = {
    0x00000000, 0x000000E0, 0x0000000E, 0x000000EE, 0x0000E000, 0x0000E0E0,
    0x0000E00E, 0x0000E0EE, 0x00000E00, 0x00000EE0, 0x00000E0E, 0x00000EEE,
    0x0000EE00, 0x0000EEE0, 0x0000EE0E, 0x0000EEEE, 0x00E00000, 0x00E000E0,
    0x00E0000E, 0x00E000EE, 0x00E0E000, 0x00E0E0E0, 0x00E0E00E, 0x00E0E0EE,
    0x00E00E00, 0x00E00EE0, 0x00E00E0E, 0x00E00EEE, 0x00E0EE00, 0x00E0EEE0,
    0x00E0EE0E, 0x00E0EEEE, 0x000E0000, 0x000E00E0, 0x000E000E, 0x000E00EE,
    0x000EE000, 0x000EE0E0, 0x000EE00E, 0x000EE0EE, 0x000E0E00, 0x000E0EE0,
    0x000E0E0E, 0x000E0EEE, 0x000EEE00, 0x000EEEE0, 0x000EEE0E, 0x000EEEEE,
    0x00EE0000, 0x00EE00E0, 0x00EE000E, 0x00EE00EE, 0x00EEE000, 0x00EEE0E0,
    0x00EEE00E, 0x00EEE0EE, 0x00EE0E00, 0x00EE0EE0, 0x00EE0E0E, 0x00EE0EEE,
    0x00EEEE00, 0x00EEEEE0, 0x00EEEE0E, 0x00EEEEEE, 0xE0000000, 0xE00000E0,
    0xE000000E, 0xE00000EE, 0xE000E000, 0xE000E0E0, 0xE000E00E, 0xE000E0EE,
    0xE0000E00, 0xE0000EE0, 0xE0000E0E, 0xE0000EEE, 0xE000EE00, 0xE000EEE0,
    0xE000EE0E, 0xE000EEEE, 0xE0E00000, 0xE0E000E0, 0xE0E0000E, 0xE0E000EE,
    0xE0E0E000, 0xE0E0E0E0, 0xE0E0E00E, 0xE0E0E0EE, 0xE0E00E00, 0xE0E00EE0,
    0xE0E00E0E, 0xE0E00EEE, 0xE0E0EE00, 0xE0E0EEE0, 0xE0E0EE0E, 0xE0E0EEEE,
    0xE00E0000, 0xE00E00E0, 0xE00E000E, 0xE00E00EE, 0xE00EE000, 0xE00EE0E0,
    0xE00EE00E, 0xE00EE0EE, 0xE00E0E00, 0xE00E0EE0, 0xE00E0E0E, 0xE00E0EEE,
    0xE00EEE00, 0xE00EEEE0, 0xE00EEE0E, 0xE00EEEEE, 0xE0EE0000, 0xE0EE00E0,
    0xE0EE000E, 0xE0EE00EE, 0xE0EEE000, 0xE0EEE0E0, 0xE0EEE00E, 0xE0EEE0EE,
    0xE0EE0E00, 0xE0EE0EE0, 0xE0EE0E0E, 0xE0EE0EEE, 0xE0EEEE00, 0xE0EEEEE0,
    0xE0EEEE0E, 0xE0EEEEEE, 0x0E000000, 0x0E0000E0, 0x0E00000E, 0x0E0000EE,
    0x0E00E000, 0x0E00E0E0, 0x0E00E00E, 0x0E00E0EE, 0x0E000E00, 0x0E000EE0,
    0x0E000E0E, 0x0E000EEE, 0x0E00EE00, 0x0E00EEE0, 0x0E00EE0E, 0x0E00EEEE,
    0x0EE00000, 0x0EE000E0, 0x0EE0000E, 0x0EE000EE, 0x0EE0E000, 0x0EE0E0E0,
    0x0EE0E00E, 0x0EE0E0EE, 0x0EE00E00, 0x0EE00EE0, 0x0EE00E0E, 0x0EE00EEE,
    0x0EE0EE00, 0x0EE0EEE0, 0x0EE0EE0E, 0x0EE0EEEE, 0x0E0E0000, 0x0E0E00E0,
    0x0E0E000E, 0x0E0E00EE, 0x0E0EE000, 0x0E0EE0E0, 0x0E0EE00E, 0x0E0EE0EE,
    0x0E0E0E00, 0x0E0E0EE0, 0x0E0E0E0E, 0x0E0E0EEE, 0x0E0EEE00, 0x0E0EEEE0,
    0x0E0EEE0E, 0x0E0EEEEE, 0x0EEE0000, 0x0EEE00E0, 0x0EEE000E, 0x0EEE00EE,
    0x0EEEE000, 0x0EEEE0E0, 0x0EEEE00E, 0x0EEEE0EE, 0x0EEE0E00, 0x0EEE0EE0,
    0x0EEE0E0E, 0x0EEE0EEE, 0x0EEEEE00, 0x0EEEEEE0, 0x0EEEEE0E, 0x0EEEEEEE,
    0xEE000000, 0xEE0000E0, 0xEE00000E, 0xEE0000EE, 0xEE00E000, 0xEE00E0E0,
    0xEE00E00E, 0xEE00E0EE, 0xEE000E00, 0xEE000EE0, 0xEE000E0E, 0xEE000EEE,
    0xEE00EE00, 0xEE00EEE0, 0xEE00EE0E, 0xEE00EEEE, 0xEEE00000, 0xEEE000E0,
    0xEEE0000E, 0xEEE000EE, 0xEEE0E000, 0xEEE0E0E0, 0xEEE0E00E, 0xEEE0E0EE,
    0xEEE00E00, 0xEEE00EE0, 0xEEE00E0E, 0xEEE00EEE, 0xEEE0EE00, 0xEEE0EEE0,
    0xEEE0EE0E, 0xEEE0EEEE, 0xEE0E0000, 0xEE0E00E0, 0xEE0E000E, 0xEE0E00EE,
    0xEE0EE000, 0xEE0EE0E0, 0xEE0EE00E, 0xEE0EE0EE, 0xEE0E0E00, 0xEE0E0EE0,
    0xEE0E0E0E, 0xEE0E0EEE, 0xEE0EEE00, 0xEE0EEEE0, 0xEE0EEE0E, 0xEE0EEEEE,
    0xEEEE0000, 0xEEEE00E0, 0xEEEE000E, 0xEEEE00EE, 0xEEEEE000, 0xEEEEE0E0,
    0xEEEEE00E, 0xEEEEE0EE, 0xEEEE0E00, 0xEEEE0EE0, 0xEEEE0E0E, 0xEEEE0EEE,
    0xEEEEEE00, 0xEEEEEEE0, 0xEEEEEE0E, 0xEEEEEEEE};

// speed testing variables
// static const uint8_t speed1[480 * 320 / 4] = { 0 };
// static const uint8_t speed2[480 * 320 / 4] = { 0 };
// uint8_t speedCount = 0;

//--------------------------------------------------------------------------------
// private functions

// LCD hardware Reset, active low
void ILI9488_RESET(void) {
  // setting the reset pin to low to signal a reset
  HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_RESET);

  // small delay
  HAL_Delay(10);

  // setting the pin to high (default state)
  HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);

  // another delay to be safe
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

// sends command to lcd
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

//--------------------------------------------------------------------------------
// public functions

// initializes the ST7796S
void ILI9488_INIT(SPI_HandleTypeDef *spi) {
  // safety delays. can probably be removed
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
  uint8_t unlockData[2] = {0xA9, 0x51, 0x2C, 0x82};
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
  // format: R G B 0 R G B 0
  // each byte = two pixels due to padding
  uint8_t colmod = 0x01;
  ILI9488_DATA(spi, &colmod, 1);

  // Column inversion for display longevity
  // update: seems to cause flicker on this display?
  // 1 dot mode
  //	ILI9488_CMD(spi, 0xB4);
  //	uint8_t inversion = 0x01;
  //	ILI9488_DATA(spi, &inversion, 1);

  // display on
  ILI9488_CMD(spi, 0x29);
}

// testing function to see fill direction and measure min refresh speed
// currently designed for reliability and testing purposes, but can be altered
// to be fast
void ILI9488_FILL(SPI_HandleTypeDef *spi) {
  // set column address command
  ILI9488_CMD(spi, 0x2A);
  // parameters: starting col MSB, starting col LSB, ending col MSB, ending col
  // LSB
  uint8_t caset[] = {

      (uint8_t)(0),

      (uint8_t)(0),

      (uint8_t)(ILI9488_WIDTH >> 8),

      (uint8_t)(ILI9488_WIDTH & 0xFF)};

  ILI9488_DATA(spi, caset, 4);

  // set page address command
  ILI9488_CMD(spi, 0x2B);
  // parameters: starting row MSB, starting row LSB, ending row MSB, ending row
  // LSB
  uint8_t paset[] = {

      (uint8_t)(0),

      (uint8_t)(0),

      (uint8_t)(ILI9488_HEIGHT >> 8),

      (uint8_t)(ILI9488_HEIGHT & 0xFF)};

  ILI9488_DATA(spi, paset, 4);

  // write data command
  ILI9488_CMD(spi, 0x2C);

  // setting to data mode
  HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

  // selecting spi device
  ILI9488_SELECT();
  // each byte contains two bits
  uint8_t padded = 238;
  for (uint32_t r = 0; r < 76800; r++) {
    HAL_SPI_Transmit(spi, &padded, 1, HAL_MAX_DELAY);
  }
  //	HAL_Delay(500);
  //	HAL_SPI_Transmit_DMA(spi, speed1, sizeof(speed1));

  //	speedCount = 1;
  ILI9488_DESELECT();
}

// speed testing function for ILI9488_FILL
// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
//	if (hspi->Instance == SPI1) {
//		if (speedCount < 1){
//			ILI9488_DESELECT();
//		}else{
//			HAL_SPI_Transmit_DMA(hspi, speed2, sizeof(speed2));
//			speedCount--;
//		}
//	}
//}

// testing function to make sure written images are in the right spot
// can be altered for speed later in development
bool ILI9488_REFRESH(SPI_HandleTypeDef *spi) {
  // checking if the display is already being modified
  if (!state.currentlyDrawing) {
    // setting status to busy

    state.currentlyDrawing = true;

    // set column address command
    ILI9488_CMD(spi, 0x2A);
    // parameters: starting col MSB, starting col LSB, ending col MSB, ending
    // col LSB
    uint8_t caset[] = {

        (uint8_t)(0),

        (uint8_t)(0),

        (uint8_t)((ILI9488_WIDTH - 1) >> 8),

        (uint8_t)((ILI9488_WIDTH - 1) & 0xFF)};

    ILI9488_DATA(spi, caset, 4);

    // set row address command
    ILI9488_CMD(spi, 0x2B);
    // parameters: starting row MSB, starting row LSB, ending row MSB, ending
    // row LSB
    uint8_t raset[] = {

        (uint8_t)(0),

        (uint8_t)(0),

        (uint8_t)((ILI9488_HEIGHT - 1) >> 8),

        (uint8_t)((ILI9488_HEIGHT - 1) & 0xFF)};

    ILI9488_DATA(spi, raset, 4);

    // write data command
    ILI9488_CMD(spi, 0x2C);

    // setting to data mode
    HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

    // selecting spi device
    ILI9488_SELECT();

    //		for (uint32_t i = 0; i < (480 * 320) / 2; i++) {
    //			uint8_t px = (
    //					GET_PIXEL(state.screenCopy, i * 2) ?
    // 0xE0 : 0) 					|
    // (GET_PIXEL(state.screenCopy, 1 + i * 2) ?
    // 14 : 0); 			HAL_SPI_Transmit(spi, &px, 1,
    // HAL_MAX_DELAY);
    //
    //		}

    // using lookup table to expand bit packed pixels into the correct data
    // pattern for the display
    // R G B P R G B P
    for (uint32_t i = 0; i < sizeof(state.screenCopy); i++) {
      HAL_SPI_Transmit(spi, (uint8_t *)&table[state.screenCopy[i]], 4,
                       HAL_MAX_DELAY);
    }
    ILI9488_DESELECT();
    state.currentlyDrawing = false;
    return true;
  } else {
    return false;
  }
}

// loads image to screen buffer
bool ILI9488_LOAD(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y,
                  Image_t *image, bool overWrite) {
  state.currentlyLoading = true;
  // decompressing image to cloned buffer
  uint32_t startTime = HAL_GetTick();
  // size of visible image
  state.dcompImage_SIZE = 0;

  // all pixels in image including ones that are clipped off by the edge of
  // the screen index
  uint32_t count = 0;
  // iterating through compressed image

  // WORK IN PROGRESS
  //		for (uint32_t i = 0; i < image->size; i++) {
  //			// filling up current partial byte
  //			for (uint8_t j = 0; j < (8 - count % 8) % 8 && j <
  // image->data[i]; j++) { 				if ((count %
  // image->width) + x < ILI9488_WIDTH
  //						&& (count / image->width) + y <
  // ILI9488_HEIGHT) { 					uint32_t
  // globalpos = ILI9488_WIDTH
  //							* (y + (count /
  // image->width)) + x
  //							+ (count %
  // image->width); 					if (i % 2) {
  //						// pixel is high, 1 % 2 = 1
  //						//  will always be high in
  // overwrite and OR mode
  // SET_PIXEL(state.screenCopy, globalpos);
  // } else if (overWrite) {
  //						// pixel is low and overwrite is
  // on. if in OR mode, just leave
  //						// current pixel as it is
  //						CLR_PIXEL(state.screenCopy,
  // globalpos);
  //					}
  //					state.dcompImage_SIZE++;
  //				}
  //				count++;
  //			}
  //
  //			// filling up next bytes
  //			for (uint8_t j = 0; j < image->data[i] / 8; j++) {
  //				// if the next 8 pixels/bits aren't cut off by
  // the screen 				if ((count % image->width) + x +
  // 8 < ILI9488_WIDTH
  //						&& (count / image->width) + y +
  //(8 / image->width)
  //< ILI9488_HEIGHT) { 					if (i % 2) {
  //						// pixel is high, 1 % 2 = 1
  //						//  will always be high in
  // overwrite and OR mode
  // state.screenCopy[count / 8] = 1;
  // } else if (overWrite) {
  //						// pixel is low and overwrite is
  // on. if in OR mode, just leave
  //						// current pixel as it is
  //						state.screenCopy[count / 8] = 0;
  //					}
  //					state.dcompImage_SIZE += 8;
  //					count += 8;
  //				} else {
  //					//fill up partial byte
  //					for (uint8_t f = 0; f < 8; f++) {
  //						if ((count % image->width) + x <
  // ILI9488_WIDTH
  //								&& (count /
  // image->width) + y < ILI9488_HEIGHT) {
  //							// calculating screen
  // pixel index from current position within image
  // uint32_t globalpos = ILI9488_WIDTH
  //									* (y +
  //(count / image->width)) + x
  //									+ (count
  //% image->width); 							if (i %
  // 2) {
  //								// pixel is
  // high, 1 % 2 = 1
  //								//  will always
  // be high in overwrite and OR mode
  // SET_PIXEL(state.screenCopy, globalpos);
  // } else if (overWrite) {
  //								// pixel is low
  // and overwrite is on. if in OR mode, just leave
  //								// current pixel
  // as it is
  // CLR_PIXEL(state.screenCopy, globalpos);
  //							}
  //							state.dcompImage_SIZE++;
  //						}
  //						count++;
  //					}
  //				}
  //
  //				// filling up last partial byte
  //				for (uint8_t j = 0; j < image->data[i] % 8; j++)
  //{ 					if ((count % image->width) + x <
  // ILI9488_WIDTH
  //							&& (count /
  // image->width) + y < ILI9488_HEIGHT) {
  // uint32_t globalpos = ILI9488_WIDTH
  //								* (y + (count /
  // image->width)) + x
  //								+ (count %
  // image->width); 						if (i % 2) {
  //							// pixel is high, 1 % 2
  //= 1
  //							//  will always be high
  // in overwrite and OR mode
  // SET_PIXEL(state.screenCopy, globalpos);
  // } else if (overWrite) {
  //							// pixel is low and
  // overwrite is on. if in OR mode, just leave
  //							// current pixel as it
  // is
  // CLR_PIXEL(state.screenCopy, globalpos);
  //						}
  //						state.dcompImage_SIZE++;
  //					}
  //					count++;
  //				}
  //			}
  //		}

  for (uint32_t i = 0; i < image->size; i++) {
    // iterating through the current RLE value
    // TODO: OPTIMIZE THIS
    for (uint8_t j = 0; j < image->data[i]; j++) {
      if ((count % image->width) + x < ILI9488_WIDTH &&
          (count / image->width) + y < ILI9488_HEIGHT) {
        // calculating screen pixel index from current position within image
        uint32_t globalpos = ILI9488_WIDTH * (y + (count / image->width)) + x +
                             (count % image->width);

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

  uint32_t finalTime = HAL_GetTick() - startTime;

  state.x = x;
  state.y = y;
  state.width = image->width;
  state.height = image->height;

  // temporary call for convenience. TODO remove later
  ILI9488_DRAW(spi);

  return true;
}

// x and y should be multiples of 8
// draws last loaded image to screen
// TODO: make a fast refresh
bool ILI9488_DRAW(SPI_HandleTypeDef *spi) {
  if (!state.currentlyDrawing) {
    // setting status to busy

    state.currentlyDrawing = true;
    // set column address command
    ILI9488_CMD(spi, 0x2A);
    // parameters: starting col MSB, starting col LSB, ending col MSB, ending
    // col LSB
    uint8_t caset[] = {

        (uint8_t)(state.x >> 8),

        (uint8_t)(state.x & 0xFF),

        (uint8_t)((state.x + state.width - 1 > ILI9488_WIDTH - 1
                       ? ILI9488_WIDTH - 1
                       : state.x + state.width - 1) >>
                  8),

        (uint8_t)((state.x + state.width - 1 > ILI9488_WIDTH - 1
                       ? ILI9488_WIDTH - 1
                       : state.x + state.width - 1) &
                  0xFF)

    };

    ILI9488_DATA(spi, caset, 4);

    // set row address command
    ILI9488_CMD(spi, 0x2B);
    // parameters: starting row MSB, starting row LSB, ending row MSB, ending
    // row LSB
    uint8_t raset[] = {

        (uint8_t)(state.y >> 8),

        (uint8_t)(state.y & 0xFF),

        (uint8_t)((state.y + state.height - 1 > ILI9488_HEIGHT - 1
                       ? ILI9488_HEIGHT - 1
                       : state.y + state.height - 1) >>
                  8),

        (uint8_t)((state.y + state.height - 1 > ILI9488_HEIGHT - 1
                       ? ILI9488_HEIGHT - 1
                       : state.y + state.height - 1) &
                  0xFF)};

    ILI9488_DATA(spi, raset, 4);

    if (state.x + state.width > ILI9488_WIDTH) {
      state.width = ILI9488_WIDTH - state.x;
    }

    if (state.y + state.height > ILI9488_HEIGHT) {
      state.height = ILI9488_HEIGHT - state.y;
    }

    state.x /= 8;
    state.width /= 8;

    // write data command
    ILI9488_CMD(spi, 0x2C);

    // setting to data mode
    HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

    // selecting spi device
    ILI9488_SELECT();

    // double buffering

    // sending image data. chunking data for DMA and memory saving purposes
    state.imageProgress = 0;

    state.imageTarget = state.dcompImage_SIZE / 2;

    state.activeBuf = 0;
    // checking if chunking is required or not
    if (state.imageTarget <= CHUNK) {
      // not required

      // expanding to buffer
      // scan 8 pixels at once, = 4 bytes
      for (uint32_t i = 0; i < state.imageTarget / 4; i++) {
        // converting image space index to screen space index
        uint32_t globalpos =
            ILI9488_SCALED_WIDTH * (state.y + (i / state.width)) + state.x +
            (i % state.width);
        ((uint32_t *)state.buf[state.activeBuf])[i] =
            table[state.screenCopy[globalpos]];
      }

      HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf], state.imageTarget);

      state.imageProgress = state.imageTarget;
    } else {
      // required
      // PRELOADING FIRST TWO CHUNKS
      for (uint32_t i = 0; i < CHUNK / 4; i++) {
        // converting image space index to screen space index
        uint32_t globalpos =
            ILI9488_SCALED_WIDTH * (state.y + (i / state.width)) + state.x +
            (i % state.width);
        ((uint32_t *)state.buf[state.activeBuf])[i] =
            table[state.screenCopy[globalpos]];
      }

      state.imageProgress += CHUNK;
      state.activeBuf = !state.activeBuf;
      uint16_t shift = state.imageProgress / 4;
      for (uint32_t i = 0;
           i < (state.imageTarget - state.imageProgress < CHUNK
                    ? (state.imageTarget - state.imageProgress) / 4
                    : CHUNK / 4);
           i++) {
        // converting image space index to screen space index
        uint32_t globalpos =
            ILI9488_SCALED_WIDTH * (state.y + ((i + shift) / state.width)) +
            state.x + ((i + shift) % state.width);
        ((uint32_t *)state.buf[state.activeBuf])[i] =
            table[state.screenCopy[globalpos]];
      }
      // transmitting the first chunk, then the interrupt will transmit the
      // second chunk before loading the third etc

      HAL_SPI_Transmit_DMA(spi, state.buf[!state.activeBuf], CHUNK);
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
      state.activeBuf = !state.activeBuf;
      uint16_t shift = state.imageProgress / 4;
      // pointer to the active 4 byte section of memory
      uint32_t *dst = (uint32_t *)state.buf[state.activeBuf];
      for (uint32_t i = 0; i < CHUNK / 4; i++) {
        // converting image space index to screen space index
        // this line is heavily compiler optimized because any obvious
        // optimizations make it slower
        uint32_t globalpos =
            ILI9488_SCALED_WIDTH * (state.y + ((i + shift) / state.width)) +
            state.x + ((i + shift) % state.width);
        *dst++ = table[state.screenCopy[globalpos]];
      }
    }
  }
}
