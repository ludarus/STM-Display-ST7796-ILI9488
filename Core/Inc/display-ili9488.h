/*
 * display.h
 *
 *  Created on: Jun 23, 2026
 *      Author: Luke Fadel
 */
#include "image.h"
#include "main.h"
#include <stdbool.h>

#ifndef INC_DISPLAY_ILI9488_H_
#define INC_DISPLAY_ILI9488_H_

#define ON_COLOR 0xFF
#define OFF_COLOR 0x00

#define ILI9488_WIDTH 480
#define ILI9488_HEIGHT 320

#define CHUNK 4096

// macros to set and access bits in the array
// sets pixel to 1
#define SET_PIXEL(array, bit)                                                  \
  ((array)[(bit) / 8] |= (1u << ((bit) % 8))) // returns void
// sets pixel to 0
#define CLR_PIXEL(arr, bit) ((arr)[(bit) / 8] &= ~(1u << ((bit) % 8)))
// shifting byte to desired bit and masking off the rest of the bit
#define GET_PIXEL(array, bit)                                                  \
  (((array)[(bit) / 8] >> ((bit) % 8)) & 1u) // returns 0u or 1u

typedef struct {
  volatile bool currentlyDrawing;
  // creating a buffer to load into the RAM for faster image display
  uint8_t buf[2][CHUNK];
  volatile uint8_t activeBuf;

  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;

  volatile uint32_t imageProgress;
  uint32_t imageTarget;

  // DEBUG
  uint32_t startTime;
  uint32_t finalTime;
  volatile uint8_t THING;
  // DEBUG

  // bit packing to save memory
  uint8_t screenCopy[((480 * 320) + 7) / 8];
  uint32_t dcompImage_SIZE;
} ImageTransferState_t;

void ILI9488_INIT(SPI_HandleTypeDef *spi);
void ILI9488_RESET(void);
void ILI9488_SELECT(void);
void ILI9488_DESELECT(void);
bool ILI9488_REFRESH(SPI_HandleTypeDef *spi);
void ILI9488_CMD(SPI_HandleTypeDef *spi, uint8_t cmd);
void ILI9488_DATA(SPI_HandleTypeDef *spi, uint8_t *data, uint16_t size);
void ILI9488_FILL(SPI_HandleTypeDef *spi);
bool ILI9488_WRITE(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y,
                   Image_t *image, bool overWrite);



#endif /* INC_DISPLAY_ILI9488_H_ */
