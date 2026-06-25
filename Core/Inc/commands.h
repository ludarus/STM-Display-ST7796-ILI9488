/*
 * commands.h
 *
 *  Created on: Jun 18, 2026
 *      Author: Luke Fadel
 */

#ifndef INC_COMMANDS_H_
#define INC_COMMANDS_H_

#include "image.h"

// public functions. just getters and setters
uint8_t *getCommandBuffer(void);
size_t getCommandSize(void);
void commandsInit(SPI_HandleTypeDef *spiInterface);

// byte array struct to easily encode the size of a string
typedef struct {
  uint8_t *data;
  uint16_t size;
} ByteArray_t;

// command struct
typedef struct {
  // the keyword that must be inputted for the command
  const uint8_t *keyword;
  // the size of the keyword in bytes
  const uint16_t keyword_size;
  // a function pointer to a handle that executes when the command is called
  const ByteArray_t (*action)(void);
} Command_t;

#endif /* INC_COMMANDS_H_ */
