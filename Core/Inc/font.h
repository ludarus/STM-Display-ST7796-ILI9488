/*
 * font.h
 *
 *  Created on: Jun 26, 2026
 *      Author: Luke Fadel
 */

#ifndef INC_FONT_H_

typedef struct {
  // ascii code that the character represents
  const uint8_t asciiCode;
  // bit packed array of data
  const uint8_t *data;
} Character_t;

#define INC_FONT_H_

#endif /* INC_FONT_H_ */
