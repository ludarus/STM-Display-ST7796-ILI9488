/*
 * display.c
 *
 *  Created on: Jun 17, 2026
 *      Author: Luke Fadel
 *
 *  Created for ST7796S display driver
 *  https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf
 */

#include <display-st7796s.h>
#include "image.h"
#include "main.h"
#include "memory.h"
#include <stdbool.h>

static ImageTransferState_t state;

// speed testing variables for DISPLAY_FILL
// static const uint8_t testBuffer[65535] = { 0 };
// static uint8_t testCount = (320 * 480 * 2) / sizeof(testBuffer);

// LCD hardware Reset, active low
void ST7796S_RESET(void) {
  // setting the reset pin to low to signal a reset
  HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_RESET);

  // small delay
  HAL_Delay(10);

  // setting the pin to high (default state)
  HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);

  HAL_Delay(100);
}

// LCD chip select signal, active low
void ST7796S_SELECT(void) {
  // setting the select pin to low
  HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
}

void ST7796S_DESELECT(void) {
  // setting the select pin to high
  HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
}

// sends cmd to lcd
void ST7796S_CMD(SPI_HandleTypeDef *spi, uint8_t cmd) {
  // setting DC pin to command mode (low)
  HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);

  // selecting SPI device
  ST7796S_SELECT();

  // using SPI to transmit data
  HAL_SPI_Transmit(spi, &cmd, 1, HAL_MAX_DELAY);

  // deselecting SPI device
  ST7796S_DESELECT();
}

// sends data to lcd
void ST7796S_DATA(SPI_HandleTypeDef *spi, uint8_t *data, uint16_t size) {
  // setting DC pin to command mode (high)
  HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

  // selecting SPI device
  ST7796S_SELECT();

  // using SPI to transmit data
  HAL_SPI_Transmit(spi, data, size, HAL_MAX_DELAY);

  // deselecting SPI device
  ST7796S_DESELECT();
}

void ST7796S_UNLOCK_EXTENDED(SPI_HandleTypeDef *spi) {
  // unlocking extended command set (command table 2)
  ST7796S_CMD(spi, 0xF0);
  uint8_t unlockKeys[2] = {0xC3, 0x96};
  ST7796S_DATA(spi, &unlockKeys[0], 1);
  ST7796S_CMD(spi, 0xF0);
  ST7796S_DATA(spi, &unlockKeys[1], 1);
}

void ST7796S_LOCK_EXTENDED(SPI_HandleTypeDef *spi) {
  // locking extended command set
  ST7796S_CMD(spi, 0xF0);
  uint8_t lockKeys[2] = {0xC3, 0x69};
  ST7796S_DATA(spi, &lockKeys[0], 1);
  ST7796S_CMD(spi, 0xF0);
  ST7796S_DATA(spi, &lockKeys[1], 1);
}

// initializes the ST7796S
void ST7796S_INIT(SPI_HandleTypeDef *spi) {

  HAL_Delay(200);
  // hardware reset
  ST7796S_RESET();
  HAL_Delay(200);

  // software reset
  ST7796S_CMD(spi, 0x01);
  HAL_Delay(120);

  // exit sleep mode
  ST7796S_CMD(spi, 0x11);
  HAL_Delay(120);

  // backlight on
  HAL_GPIO_WritePin(DISPLAY_LED_GPIO_Port, DISPLAY_LED_Pin, GPIO_PIN_SET);

  // unlocking extended command set - table 2
  ST7796S_UNLOCK_EXTENDED(spi);

  // memory data access control - instruction 36h MADCTL
  ST7796S_CMD(spi, 0x36);
  uint8_t madctl = 0x20;
  ST7796S_DATA(spi, &madctl, 1);

  // Interface Pixel Format - instruction 3Ah COLMOD
  ST7796S_CMD(spi, 0x3A);
  // Lowest available is 16bit/pixel
  // 01010101
  uint8_t colmod = 0x55;
  ST7796S_DATA(spi, &colmod, 1);

  // Column inversion for display longevity
  ST7796S_CMD(spi, 0xB4);
  uint8_t inversion = 0x01;
  ST7796S_DATA(spi, &inversion, 1);

  // locking extended command set
  ST7796S_LOCK_EXTENDED(spi);

  // display on
  ST7796S_CMD(spi, 0x29);
}

// testing function to see fill direction and measure min refresh speed
void ST7796S_FILL(SPI_HandleTypeDef *spi) {
  // set column address command
  ST7796S_CMD(spi, 0x2A);
  // parameters: starting col MSB, starting col LSB, ending col MSB, ending col
  // LSB
  uint8_t caset[] = {

      (uint8_t)(0),

      (uint8_t)(0),

      (uint8_t)(ST7796S_WIDTH >> 8),

      (uint8_t)(ST7796S_WIDTH & 0xFF)};

  ST7796S_DATA(spi, caset, 4);

  // set row address command
  ST7796S_CMD(spi, 0x2B);
  // parameters: starting row MSB, starting row LSB, ending row MSB, ending row
  // LSB
  uint8_t raset[] = {

      (uint8_t)(0),

      (uint8_t)(0),

      (uint8_t)(ST7796S_HEIGHT >> 8),

      (uint8_t)(ST7796S_HEIGHT & 0xFF)};

  ST7796S_DATA(spi, raset, 4);

  // write data command
  ST7796S_CMD(spi, 0x2C);

  // setting to data mode
  HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

  // selecting spi device
  ST7796S_SELECT();

  uint8_t data[2] = {0xFF, 0xFF};
  for (uint32_t r = 0; r < 320 * 480; r++) {
    HAL_SPI_Transmit(spi, data, 2, HAL_MAX_DELAY);
  }

  //	HAL_SPI_Transmit_DMA(spi, testBuffer, sizeof(testBuffer));
  //	testCount--;
  ST7796S_DESELECT();
}

// speed testing function for ST7796S_FILL
// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
//	if (hspi->Instance == SPI1) {
//
//		if (testCount < 0) {
//			ST7796S_DESELECT();
//			return;
//		} else if (testCount == 0) {
//			HAL_SPI_Transmit_DMA(hspi, testBuffer,
//					(320 * 480 * 2) % sizeof(testBuffer));
//		} else {
//			HAL_SPI_Transmit_DMA(hspi, testBuffer,
// sizeof(testBuffer));
//		}
//		testCount--;
//	}
// }

// displays what's on the cloneBuffer
bool ST7796S_REFRESH(SPI_HandleTypeDef *spi) {
  if (!state.currentlyDrawing) {
    // setting status to busy

    state.currentlyDrawing = true;

    // set column address command
    ST7796S_CMD(spi, 0x2A);
    // parameters: starting col MSB, starting col LSB, ending col MSB, ending
    // col LSB
    uint8_t caset[] = {

        (uint8_t)(0),

        (uint8_t)(0),

        (uint8_t)((ST7796S_WIDTH - 1) >> 8),

        (uint8_t)((ST7796S_WIDTH - 1) & 0xFF)};

    ST7796S_DATA(spi, caset, 4);

    // set row address command
    ST7796S_CMD(spi, 0x2B);
    // parameters: starting row MSB, starting row LSB, ending row MSB, ending
    // row LSB
    uint8_t raset[] = {

        (uint8_t)(0),

        (uint8_t)(0),

        (uint8_t)((ST7796S_HEIGHT - 1) >> 8),

        (uint8_t)((ST7796S_HEIGHT - 1) & 0xFF)};

    ST7796S_DATA(spi, raset, 4);

    // write data command
    ST7796S_CMD(spi, 0x2C);

    // setting to data mode
    HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

    // selecting spi device
    ST7796S_SELECT();
    // sending image data. chunking data for DMA and memory saving purposes

    for (uint32_t i = 0; i < (480 * 320); i++) {
      uint8_t data[2] = {GET_PIXEL(state.screenCopy, i) ? ON_COLOR : OFF_COLOR,
                         GET_PIXEL(state.screenCopy, i) ? ON_COLOR : OFF_COLOR};
      HAL_SPI_Transmit(spi, data, 2, HAL_MAX_DELAY);
    }
    ST7796S_DESELECT();
    state.currentlyDrawing = false;
    return true;
  } else {
    return false;
  }
}

bool ST7796S_WRITE(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y,
                   Image_t *image, bool overWrite) {
  if (!state.currentlyDrawing) {
    // setting status to busy

    state.currentlyDrawing = true;
    // set column address command
    ST7796S_CMD(spi, 0x2A);
    // parameters: starting col MSB, starting col LSB, ending col MSB, ending
    // col LSB
    uint8_t caset[] = {

        (uint8_t)(x >> 8),

        (uint8_t)(x & 0xFF),

        (uint8_t)((x + image->width - 1 > ST7796S_WIDTH - 1
                       ? ST7796S_WIDTH - 1
                       : x + image->width - 1) >>
                  8),

        (uint8_t)((x + image->width - 1 > ST7796S_WIDTH - 1
                       ? ST7796S_WIDTH - 1
                       : x + image->width - 1) &
                  0xFF)

    };

    ST7796S_DATA(spi, caset, 4);

    // set row address command
    ST7796S_CMD(spi, 0x2B);
    // parameters: starting row MSB, starting row LSB, ending row MSB, ending
    // row LSB
    uint8_t raset[] = {

        (uint8_t)(y >> 8),

        (uint8_t)(y & 0xFF),

        (uint8_t)((y + image->height - 1 > ST7796S_HEIGHT - 1
                       ? ST7796S_HEIGHT - 1
                       : y + image->height - 1) >>
                  8),

        (uint8_t)((y + image->height - 1 > ST7796S_HEIGHT - 1
                       ? ST7796S_HEIGHT - 1
                       : y + image->height - 1) &
                  0xFF)};

    ST7796S_DATA(spi, raset, 4);

    // decompressing image to cloned buffer

    // an index throughout the loop
    state.dcompImage_SIZE = 0;
    uint32_t count = 0;
    // iterating through compressed image
    for (uint32_t i = 0; i < image->size; i++) {
      // iterating through the current RLE value
      for (uint8_t j = 0; j < image->data[i]; j++) {
        if ((count % image->width) + x < ST7796S_WIDTH &&
            (count / image->width) + y < ST7796S_HEIGHT) {
          // calculating screen pixel index from current position within image
          uint32_t globalpos = ST7796S_WIDTH * (y + (count / image->width)) +
                               x + (count % image->width);

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
    if (x + image->width > ST7796S_WIDTH) {
      state.width = ST7796S_WIDTH - x;
    } else {
      state.width = image->width;
    }

    if (y + image->height > ST7796S_HEIGHT) {
      state.height = ST7796S_HEIGHT - y;
    } else {
      state.height = image->height;
    }

    // write data command
    ST7796S_CMD(spi, 0x2C);

    // setting to data mode
    HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);

    // selecting spi device
    ST7796S_SELECT();

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
            ST7796S_WIDTH * (y + (i / state.width)) + x + (i % state.width);

        // expanding bit to 2 bytes as per the color specification
        state.buf[state.activeBuf][i * 2] =
            GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
        state.buf[state.activeBuf][i * 2 + 1] =
            GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
      }

      HAL_SPI_Transmit_DMA(spi, state.buf[state.activeBuf], state.imageTarget);
      state.imageProgress = state.imageTarget;
    } else {
      // required

      // expanding to buffer
      for (uint32_t i = 0; i < CHUNK / 2; i++) {
        // converting image space index to screen space index
        uint32_t globalpos =
            (ST7796S_WIDTH * (y + (i / state.width))) + x + (i % state.width);

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
      for (uint32_t i = 0; i < (state.imageTarget - state.imageProgress > CHUNK
                                    ? CHUNK
                                    : state.imageTarget - state.imageProgress) /
                                   2;
           i++) {

        uint32_t globalpos =
            ST7796S_WIDTH * (state.y + imgRow) + state.x + (imgCol);

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
//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
//  if (hspi->Instance == SPI1) {
//    if (state.imageProgress >= state.imageTarget) {
//      // done drawing
//      state.finalTime = HAL_GetTick() - state.startTime;
//      ST7796S_DESELECT();
//      state.currentlyDrawing = 0;
//      return;
//    } else if (state.imageTarget - state.imageProgress < CHUNK) {
//      // partial chunk
//      HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf],
//                           state.imageTarget - state.imageProgress);
//      // done drawing criteria
//      state.imageProgress += CHUNK;
//    } else {
//      // full chunk
//      state.imageProgress += CHUNK;
//      HAL_SPI_Transmit_DMA(hspi, state.buf[state.activeBuf], CHUNK);
//      uint32_t shift = state.imageProgress / 2;
//      uint8_t *current = state.buf[state.activeBuf];
//      uint16_t imgRow = shift / state.width;
//      uint16_t imgCol = shift % state.width;
//      // loading the next chunk of the image into ram for faster transfer, and
//      // swapping buffers
//      state.activeBuf = !state.activeBuf;
//      for (uint32_t i = 0; i < CHUNK / 2; i++) {
//        // checking if image is in bounds
//
//        uint32_t globalpos =
//            ST7796S_WIDTH * (state.y + imgRow) + state.x + (imgCol);
//
//        // expanding bit to 2 bytes as per the color specification
//        uint8_t color =
//            GET_PIXEL(state.screenCopy, globalpos) ? ON_COLOR : OFF_COLOR;
//
//        *current++ = color;
//        *current++ = color;
//
//        if (++imgCol == state.width) {
//          imgCol = 0;
//          ++imgRow;
//        }
//      }
//    }
//  }
//}
