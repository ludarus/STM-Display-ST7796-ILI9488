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
const uint32_t table[256] = {
    0x00000000, 0x000000A0, 0x0000000A, 0x000000AA, 0x0000A000, 0x0000A0A0,
    0x0000A00A, 0x0000A0AA, 0x00000A00, 0x00000AA0, 0x00000A0A, 0x00000AAA,
    0x0000AA00, 0x0000AAA0, 0x0000AA0A, 0x0000AAAA, 0x00A00000, 0x00A000A0,
    0x00A0000A, 0x00A000AA, 0x00A0A000, 0x00A0A0A0, 0x00A0A00A, 0x00A0A0AA,
    0x00A00A00, 0x00A00AA0, 0x00A00A0A, 0x00A00AAA, 0x00A0AA00, 0x00A0AAA0,
    0x00A0AA0A, 0x00A0AAAA, 0x000A0000, 0x000A00A0, 0x000A000A, 0x000A00AA,
    0x000AA000, 0x000AA0A0, 0x000AA00A, 0x000AA0AA, 0x000A0A00, 0x000A0AA0,
    0x000A0A0A, 0x000A0AAA, 0x000AAA00, 0x000AAAA0, 0x000AAA0A, 0x000AAAAA,
    0x00AA0000, 0x00AA00A0, 0x00AA000A, 0x00AA00AA, 0x00AAA000, 0x00AAA0A0,
    0x00AAA00A, 0x00AAA0AA, 0x00AA0A00, 0x00AA0AA0, 0x00AA0A0A, 0x00AA0AAA,
    0x00AAAA00, 0x00AAAAA0, 0x00AAAA0A, 0x00AAAAAA, 0xA0000000, 0xA00000A0,
    0xA000000A, 0xA00000AA, 0xA000A000, 0xA000A0A0, 0xA000A00A, 0xA000A0AA,
    0xA0000A00, 0xA0000AA0, 0xA0000A0A, 0xA0000AAA, 0xA000AA00, 0xA000AAA0,
    0xA000AA0A, 0xA000AAAA, 0xA0A00000, 0xA0A000A0, 0xA0A0000A, 0xA0A000AA,
    0xA0A0A000, 0xA0A0A0A0, 0xA0A0A00A, 0xA0A0A0AA, 0xA0A00A00, 0xA0A00AA0,
    0xA0A00A0A, 0xA0A00AAA, 0xA0A0AA00, 0xA0A0AAA0, 0xA0A0AA0A, 0xA0A0AAAA,
    0xA00A0000, 0xA00A00A0, 0xA00A000A, 0xA00A00AA, 0xA00AA000, 0xA00AA0A0,
    0xA00AA00A, 0xA00AA0AA, 0xA00A0A00, 0xA00A0AA0, 0xA00A0A0A, 0xA00A0AAA,
    0xA00AAA00, 0xA00AAAA0, 0xA00AAA0A, 0xA00AAAAA, 0xA0AA0000, 0xA0AA00A0,
    0xA0AA000A, 0xA0AA00AA, 0xA0AAA000, 0xA0AAA0A0, 0xA0AAA00A, 0xA0AAA0AA,
    0xA0AA0A00, 0xA0AA0AA0, 0xA0AA0A0A, 0xA0AA0AAA, 0xA0AAAA00, 0xA0AAAAA0,
    0xA0AAAA0A, 0xA0AAAAAA, 0x0A000000, 0x0A0000A0, 0x0A00000A, 0x0A0000AA,
    0x0A00A000, 0x0A00A0A0, 0x0A00A00A, 0x0A00A0AA, 0x0A000A00, 0x0A000AA0,
    0x0A000A0A, 0x0A000AAA, 0x0A00AA00, 0x0A00AAA0, 0x0A00AA0A, 0x0A00AAAA,
    0x0AA00000, 0x0AA000A0, 0x0AA0000A, 0x0AA000AA, 0x0AA0A000, 0x0AA0A0A0,
    0x0AA0A00A, 0x0AA0A0AA, 0x0AA00A00, 0x0AA00AA0, 0x0AA00A0A, 0x0AA00AAA,
    0x0AA0AA00, 0x0AA0AAA0, 0x0AA0AA0A, 0x0AA0AAAA, 0x0A0A0000, 0x0A0A00A0,
    0x0A0A000A, 0x0A0A00AA, 0x0A0AA000, 0x0A0AA0A0, 0x0A0AA00A, 0x0A0AA0AA,
    0x0A0A0A00, 0x0A0A0AA0, 0x0A0A0A0A, 0x0A0A0AAA, 0x0A0AAA00, 0x0A0AAAA0,
    0x0A0AAA0A, 0x0A0AAAAA, 0x0AAA0000, 0x0AAA00A0, 0x0AAA000A, 0x0AAA00AA,
    0x0AAAA000, 0x0AAAA0A0, 0x0AAAA00A, 0x0AAAA0AA, 0x0AAA0A00, 0x0AAA0AA0,
    0x0AAA0A0A, 0x0AAA0AAA, 0x0AAAAA00, 0x0AAAAAA0, 0x0AAAAA0A, 0x0AAAAAAA,
    0xAA000000, 0xAA0000A0, 0xAA00000A, 0xAA0000AA, 0xAA00A000, 0xAA00A0A0,
    0xAA00A00A, 0xAA00A0AA, 0xAA000A00, 0xAA000AA0, 0xAA000A0A, 0xAA000AAA,
    0xAA00AA00, 0xAA00AAA0, 0xAA00AA0A, 0xAA00AAAA, 0xAAA00000, 0xAAA000A0,
    0xAAA0000A, 0xAAA000AA, 0xAAA0A000, 0xAAA0A0A0, 0xAAA0A00A, 0xAAA0A0AA,
    0xAAA00A00, 0xAAA00AA0, 0xAAA00A0A, 0xAAA00AAA, 0xAAA0AA00, 0xAAA0AAA0,
    0xAAA0AA0A, 0xAAA0AAAA, 0xAA0A0000, 0xAA0A00A0, 0xAA0A000A, 0xAA0A00AA,
    0xAA0AA000, 0xAA0AA0A0, 0xAA0AA00A, 0xAA0AA0AA, 0xAA0A0A00, 0xAA0A0AA0,
    0xAA0A0A0A, 0xAA0A0AAA, 0xAA0AAA00, 0xAA0AAAA0, 0xAA0AAA0A, 0xAA0AAAAA,
    0xAAAA0000, 0xAAAA00A0, 0xAAAA000A, 0xAAAA00AA, 0xAAAAA000, 0xAAAAA0A0,
    0xAAAAA00A, 0xAAAAA0AA, 0xAAAA0A00, 0xAAAA0AA0, 0xAAAA0A0A, 0xAAAA0AAA,
    0xAAAAAA00, 0xAAAAAAA0, 0xAAAAAA0A, 0xAAAAAAAA};
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
  // copying state variables for compiler optimization (pointer aliasing)
  uint16_t col = 0, row = 0;
  const uint16_t imgWidth = image->width;
  uint8_t *imgData = image->data;
  uint32_t decompiledImageSize = 0;

  // iterating through compressed image
  for (uint32_t i = 0; i < image->size; i++) {
    // getting the remaining number of pixels in the current RLE value
    // uint8_t remainingPixels = imgData[i];
    // bool isOn = (i % 2);
    //
    // while (remainingPixels > 0) {
    //   // getting the remaining number of pixels in the current row
    //   uint16_t rowAvailable = imgWidth - col;
    //   // getting the amount to chunk the write by
    //   uint16_t chunk =
    //       remainingPixels < rowAvailable ? remainingPixels : rowAvailable;
    //
    //   // checking if current row is inside the boundaries of the screen
    //   if (col + x < ILI9488_WIDTH && row + y < ILI9488_HEIGHT) {
    //     // filling up the leading byte of the chunk (bitwise)
    //
    //     uint32_t globalPos = ILI9488_WIDTH * (y + row) + x + col;
    //
    //     // decrementing the remaining pixels
    //     remainingPixels -= chunk;
    //
    //     // updating col and row
    //     if ((col += chunk) >= imgWidth) {
    //       col -= imgWidth;
    //       row++;
    //     }
    //
    //     if (globalPos % 8 != 0) {
    //       for (uint8_t leading = 0; leading < 8 - (globalPos % 8); leading++) {
    //         if (isOn) {
    //           // pixel will always be on
    //           SET_PIXEL(state.screenCopy, globalPos);
    //         } else if (overWrite) {
    //           // byte is OFF colour and overwriting
    //           CLR_PIXEL(state.screenCopy, globalPos);
    //         }
    //         globalPos++;
    //         chunk--;
    //       }
    //     }
    //
    //     // filling up the middle bytes (byte-wise)
    //     for (uint16_t byte = 0; byte < chunk / 8; byte++) {
    //       if (isOn) {
    //         // pixel will always be on
    //         state.screenCopy[(globalPos / 8) + byte] = 0xFF;
    //       } else if (overWrite) {
    //         // byte is OFF colour and overwriting
    //         state.screenCopy[(globalPos / 8) + byte] = 0;
    //       }
    //       globalPos += 8;
    //     }
    //
    //     // filling up the trailing byte of the chunk (bitwise)
    //   }
    // }

    // iterating through the current RLE value
    for (uint8_t j = 0; j < imgData[i]; j++) {
      // if the current pixel is in bounds of the screen
      if (col + x < ILI9488_WIDTH && row + y < ILI9488_HEIGHT) {
        // calculating screen pixel index from current position within image
        uint32_t globalpos = ILI9488_WIDTH * (y + row) + x + col;

        if (i % 2) {
          // pixel is high, 1 % 2 = 1
          //  will always be high in overwrite and OR mode
          SET_PIXEL(state.screenCopy, globalpos);
        } else if (overWrite) {
          // pixel is low and overwrite is on. if in OR mode, just leave
          // current pixel as it is
          CLR_PIXEL(state.screenCopy, globalpos);
        }
		  decompiledImageSize++;
      }
      // incrementing the column and width
      if (++col == imgWidth) {
        col = 0;
        row++;
      }
    }
  }

  uint32_t finalTime = HAL_GetTick() - startTime;

  state.x = x;
  state.y = y;
  state.width = image->width;
  state.height = image->height;
  state.dcompImage_SIZE = decompiledImageSize;

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
      state.startTime = HAL_GetTick();
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
      state.finalTime = HAL_GetTick() - state.startTime;
      return;
    } else if (state.imageTarget - state.imageProgress < CHUNK) {
      // partial chunk
      HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf],
                           state.imageTarget - state.imageProgress);
      // done drawing condition
      state.imageProgress += CHUNK;
    } else {
      // full chunk

      state.imageProgress += CHUNK;
      HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf], CHUNK);
      state.activeBuf = !state.activeBuf;
      const uint16_t shift = state.imageProgress / 4;
      const uint16_t y = state.y;
      const uint16_t x = state.x;
      const uint16_t width = state.width;
      // pointer to the active 4 byte section of memory
      uint32_t *dst = (uint32_t *)state.buf[state.activeBuf];
      for (uint32_t i = 0; i < CHUNK / 4; i++) {
        // converting image space index to screen space index
        // this line is heavily compiler optimized because any obvious
        // optimizations make it slower
        uint32_t globalpos =
            ILI9488_SCALED_WIDTH * (y + ((i + shift) / width)) + x +
            ((i + shift) % width);
        *dst++ = table[state.screenCopy[globalpos]];
      }
    }
  }
}
