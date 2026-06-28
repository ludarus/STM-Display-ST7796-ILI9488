/*
 * image.h
 *
 *  Created on: Jun 18, 2026
 *      Author: Luke Fadel
 */

#ifndef INC_IMAGE_H_
#define INC_IMAGE_H_

#include "main.h"

typedef struct {
  const uint16_t width;
  const uint16_t height;
  // rle encoded string of data
  const uint8_t *data;
  const uint32_t size;
} Image_t;

#endif /* INC_IMAGE_H_ */
