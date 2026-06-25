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

// width and height of display in pixels
#define ILI9488_WIDTH 480
#define ILI9488_HEIGHT 320

#define ILI9488_SCALED_WIDTH (ILI9488_WIDTH / 8)

// the size of the buffers in bytes that will store the expanded image data
// #define CHUNK 5360
// #define CHUNK 64
#define CHUNK 4096

// macros to set bits in a bit packed array
// sets pixel/bit to 1
#define SET_PIXEL(array, bit)                                                  \
  ((array)[(bit) / 8] |= (1u << ((bit) % 8))) // returns void
// sets pixel/bit to 0
#define CLR_PIXEL(arr, bit) ((arr)[(bit) / 8] &= ~(1u << ((bit) % 8)))
// shifting byte to desired bit and masking off the rest of the bit

// TODO: optimize struct alignment for better memory usage
// struct to store the current state of image rendering,
// as drawing happens between functions and callbacks so shared state is needed
typedef struct {
  // a boolean that determines if the display is currently being written to
  volatile bool currentlyDrawing;

  // same thing as currentlyDrawing but for loading
  volatile bool currentlyLoading;

  // a double buffer to expand bit packed image data into
  // one side is transferring over SPI while the other side is being filled with
  // new image data aligned for casting from uint32_t to uint8_t
  uint8_t buf[2][CHUNK] __attribute__((aligned(4)));
  // represents the active buffer
  volatile uint8_t activeBuf;

  // parameters from the requested image to be displayed
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;

  // current amount of bytes transferred
  volatile uint32_t imageProgress;
  // total amount of bytes to be transferred
  uint32_t imageTarget;

  // DEBUG
  uint32_t startTime;
  uint32_t finalTime;
  volatile uint8_t THING;
  // DEBUG

  // bit packed array of the screen. bitpacked to save memory
  uint8_t screenCopy[((480 * 320) + 7) / 8];
  // size of the decoded image
  uint32_t dcompImage_SIZE;
} ImageTransferState_t;

// public functions
void ILI9488_INIT(SPI_HandleTypeDef *spi);
bool ILI9488_REFRESH(SPI_HandleTypeDef *spi);
void ILI9488_FILL(SPI_HandleTypeDef *spi);
bool ILI9488_LOAD(SPI_HandleTypeDef *spi, uint16_t x, uint16_t y,
                  Image_t *image, bool overWrite);
bool ILI9488_DRAW(SPI_HandleTypeDef *spi);

#endif /* INC_DISPLAY_ILI9488_H_ */
