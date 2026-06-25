/*
 * commands.c
 *
 *  Created on: Jun 19, 2026
 *      Author: Luke Fadel
 */
#include "commands.h"
#include "main.h"
#include <display-ili9488.h>
#include <stdbool.h>

// includes for every image
#include "File_002_ObjNum_001_NEW_6_17_26.h"
#include "File_005_ObjNum_004_480x320_6_18_26.h"
#include "File_006_ObjNum_005_480x320_6_18_26_C.h"
#include "File_007_ObjNum_006_480x320_6_18_26.h"
#include "File_008_ObjNum_007_96x190_6_18_26.h"
#include "File_009_ObjNum_008_96x190_6_18_26.h"
#include "File_010_ObjNum_009_96x190_6_18_26.h"
#include "File_011_ObjNum_010_96x190_6_18_26.h"
#include "File_012_ObjNum_011_96x190_6_18_26.h"
#include "File_013_ObjNum_012_96x190_6_18_26.h"
#include "File_014_ObjNum_013_96x190_6_18_26.h"
#include "File_015_ObjNum_014_96x190_6_18_26.h"
#include "File_016_ObjNum_015_96x190_6_18_26.h"
#include "File_017_ObjNum_016_96x190_6_18_26.h"
#include "File_018_ObjNum_017_96x190_6_16_26.h"
#include "File_019_ObjNum_018_48x208_6_19_26.h"
#include "File_020_ObjNum_019_48x208_6_19_26.h"
#include "File_021_ObjNum_020_48x208_6_19_26.h"
#include "File_022_ObjNum_021_48x208_6_19_26.h"
#include "File_023_ObjNum_022_48x208_6_19_26.h"
#include "File_024_ObjNum_023_48x208_6_19_26.h"
#include "File_025_ObjNum_024_48x208_6_19_26.h"
#include "File_026_ObjNum_025_48x208_6_19_26.h"
#include "File_027_ObjNum_026_48x208_6_19_26.h"
#include "File_028_ObjNum_027_48x208_6_19_26.h"
#include "File_029_ObjNum_028_48x208_6_19_26.h"
#include "File_030_ObjNum_029_64x25_6_19_26.h"
#include "File_031_ObjNum_030_64x25_6_19_26.h"
#include "File_032_ObjNum_031_96x190_6_19_26.h"
#include "File_033_ObjNum_032_96x190_6_19_26.h"
#include "File_034_ObjNum_033_96x190_6_19_26.h"
#include "File_035_ObjNum_034_96x190_6_19_26.h"
#include "File_036_ObjNum_035_96x190_6_19_26.h"
#include "File_037_ObjNum_036_96x190_6_19_26.h"
#include "File_038_ObjNum_037_96x190_6_19_26.h"
#include "File_039_ObjNum_038_96x190_6_19_26.h"
#include "File_040_ObjNum_039_96x190_6_19_26.h"
#include "File_041_ObjNum_040_96x190_6_19_26.h"
#include "File_042_ObjNum_041_96x190_6_19_26.h"
#include "File_043_ObjNum_042_144x208_6_19_26.h"
#include "File_044_ObjNum_043_144x208_6_19_26.h"
#include "File_045_ObjNum_044_144x208_6_19_26.h"
#include "File_046_ObjNum_045_144x208_6_19_26.h"
#include "File_047_ObjNum_046_144x208_6_19_26.h"
#include "File_048_ObjNum_052_144x183_6_19_26.h"
#include "File_049_ObjNum_053_144x183_6_19_26.h"
#include "File_050_ObjNum_054_144x183_6_19_26.h"
#include "File_051_ObjNum_055_144x183_6_19_26.h"
#include "File_052_ObjNum_057_144x30_6_19_26.h"
#include "File_053_ObjNum_077_48x30_6_19_26.h"
#include "File_054_ObjNum_087_48x255_6_19_26.h"
#include "File_055_ObjNum_090_480x320_6_18_26.h"
#include "File_056_ObjNum_091_480x320_6_18_26.h"
#include "File_057_ObjNum_092_480x320_6_18_26.h"
#include "File_058_ObjNum_093_480x320_6_18_26.h"
#include "File_059_ObjNum_094_480x320_6_18_26.h"
#include "File_062_ObjNum_097_480x40_6_19_26.h"
#include "File_063_ObjNum_098_176x33_6_19_26.h"
#include "File_064_ObjNum_099_48x63_6_19_26.h"
#include "File_065_ObjNum_104_16x30_6_19_26.h"
#include "File_066_ObjNum_109_96x80_6_19_26.h"
#include "File_067_ObjNum_114_64x103_6_19_26.h"
#include "File_068_ObjNum_119_64x103_6_19_26.h"
#include "File_069_ObjNum_124_64x103_6_19_26.h"
#include "File_070_ObjNum_129_96x80_6_19_26.h"
#include "File_071_ObjNum_134_176x188_6_19_26.h"
#include "File_072_ObjNum_135_480x320_6_18_26.h"
#include "File_073_ObjNum_136_480x320_6_18_26.h"
#include "File_074_ObjNum_138_48x143_6_19_26.h"
#include "File_075_ObjNum_140_368x40_6_19_26.h"
#include "File_076_ObjNum_143_368x40_6_19_26.h"
#include "File_077_ObjNum_147_480x320_6_18_26.h"
#include "File_078_ObjNum_148_480x320_6_18_26.h"
#include "File_079_ObjNum_149_480x320_6_17_26.h"

// array containing every image
const Image_t *images[74] = {
    &File_074_ObjNum_138_48x143_6_19_26,
    &File_054_ObjNum_087_48x255_6_19_26,
    &File_045_ObjNum_044_144x208_6_19_26,
    &File_018_ObjNum_017_96x190_6_16_26,
    &File_034_ObjNum_033_96x190_6_19_26,
    &File_028_ObjNum_027_48x208_6_19_26,
    &File_011_ObjNum_010_96x190_6_18_26,
    &File_029_ObjNum_028_48x208_6_19_26,
    &File_079_ObjNum_149_480x320_6_17_26,
    &File_078_ObjNum_148_480x320_6_18_26,
    &File_063_ObjNum_098_176x33_6_19_26,
    &File_068_ObjNum_119_64x103_6_19_26,
    &File_062_ObjNum_097_480x40_6_19_26,
    &File_027_ObjNum_026_48x208_6_19_26,
    &File_008_ObjNum_007_96x190_6_18_26,
    &File_076_ObjNum_143_368x40_6_19_26,
    &File_064_ObjNum_099_48x63_6_19_26,
    &File_020_ObjNum_019_48x208_6_19_26,
    &File_041_ObjNum_040_96x190_6_19_26,
    &File_043_ObjNum_042_144x208_6_19_26,
    &File_052_ObjNum_057_144x30_6_19_26,
    &File_007_ObjNum_006_480x320_6_18_26,
    &File_023_ObjNum_022_48x208_6_19_26,
    &File_017_ObjNum_016_96x190_6_18_26,
    &File_026_ObjNum_025_48x208_6_19_26,
    &File_053_ObjNum_077_48x30_6_19_26,
    &File_021_ObjNum_020_48x208_6_19_26,
    &File_056_ObjNum_091_480x320_6_18_26,
    &File_019_ObjNum_018_48x208_6_19_26,
    &File_039_ObjNum_038_96x190_6_19_26,
    &File_032_ObjNum_031_96x190_6_19_26,
    &File_058_ObjNum_093_480x320_6_18_26,
    &File_066_ObjNum_109_96x80_6_19_26,
    &File_038_ObjNum_037_96x190_6_19_26,
    &File_002_ObjNum_001_NEW_6_17_26,
    &File_005_ObjNum_004_480x320_6_18_26,
    &File_044_ObjNum_043_144x208_6_19_26,
    &File_050_ObjNum_054_144x183_6_19_26,
    &File_022_ObjNum_021_48x208_6_19_26,
    &File_046_ObjNum_045_144x208_6_19_26,
    &File_033_ObjNum_032_96x190_6_19_26,
    &File_049_ObjNum_053_144x183_6_19_26,
    &File_075_ObjNum_140_368x40_6_19_26,
    &File_025_ObjNum_024_48x208_6_19_26,
    &File_031_ObjNum_030_64x25_6_19_26,
    &File_048_ObjNum_052_144x183_6_19_26,
    &File_024_ObjNum_023_48x208_6_19_26,
    &File_042_ObjNum_041_96x190_6_19_26,
    &File_077_ObjNum_147_480x320_6_18_26,
    &File_055_ObjNum_090_480x320_6_18_26,
    &File_040_ObjNum_039_96x190_6_19_26,
    &File_051_ObjNum_055_144x183_6_19_26,
    &File_009_ObjNum_008_96x190_6_18_26,
    &File_035_ObjNum_034_96x190_6_19_26,
    &File_071_ObjNum_134_176x188_6_19_26,
    &File_070_ObjNum_129_96x80_6_19_26,
    &File_057_ObjNum_092_480x320_6_18_26,
    &File_015_ObjNum_014_96x190_6_18_26,
    &File_010_ObjNum_009_96x190_6_18_26,
    &File_065_ObjNum_104_16x30_6_19_26,
    &File_072_ObjNum_135_480x320_6_18_26,
    &File_073_ObjNum_136_480x320_6_18_26,
    &File_036_ObjNum_035_96x190_6_19_26,
    &File_014_ObjNum_013_96x190_6_18_26,
    &File_067_ObjNum_114_64x103_6_19_26,
    &File_013_ObjNum_012_96x190_6_18_26,
    &File_069_ObjNum_124_64x103_6_19_26,
    &File_047_ObjNum_046_144x208_6_19_26,
    &File_037_ObjNum_036_96x190_6_19_26,
    &File_016_ObjNum_015_96x190_6_18_26,
    &File_012_ObjNum_011_96x190_6_18_26,
    &File_030_ObjNum_029_64x25_6_19_26,
    &File_006_ObjNum_005_480x320_6_18_26_C,
    &File_059_ObjNum_094_480x320_6_18_26,
};

//--------------------------------------------------------------------------------
// private variables

// standard error message when command is not found
const uint8_t errorMsg[] = "\n\rCommand not found\r\n";

// reference to spi interface. should be set upon initialization
static const SPI_HandleTypeDef *spi;

// state for testing commands
static uint8_t imageNum = 34;
static bool isOr;

// a buffer that stores the most recent serial input
uint8_t commandBuffer[255];

//--------------------------------------------------------------------------------
// public functions

// initialization sequence. currently just a setter that takes the serial input
void commandsInit(SPI_HandleTypeDef *spiInterface) { spi = spiInterface; }

uint8_t *getCommandBuffer(void) { return commandBuffer; }

size_t getCommandSize(void) { return sizeof(commandBuffer); }

//--------------------------------------------------------------------------------
// command handles

// TODO: improve or remove this command
const ByteArray_t helpCMD(void) {
  // simply returns a help message. TODO: make this modular when there's time
  static uint8_t msg[] =
      "HELP MENU FOR ST7796S DISPLAY\n COMMANDS ARE: \nHELP (show this menu), "
      "\nDISPLAY (display test)\n";
  return (ByteArray_t){.data = msg, .size = sizeof(msg) - 1};
}

const ByteArray_t displayImageCMD(void) {
  // checking if the display is currently being written to
  if (ILI9488_LOAD(spi, 0, 0, images[imageNum], !isOr)) {
    // cycling through the images
    imageNum++;
    imageNum %= 74;
    static uint8_t msg[] = "SUCESSFULLY DISPLAYED IMAGE\n";
    return (ByteArray_t){.data = msg, .size = sizeof(msg) - 1};
  } else {
    static uint8_t msg[] = "FAILED TO DISPLAY IMAGE : BUSY";
    return (ByteArray_t){.data = msg, .size = sizeof(msg) - 1};
  }
}

const ByteArray_t refreshCMD(void) {
  // checking if the display is currently being written to
  if (ILI9488_REFRESH(spi)) {
    imageNum++;
    static uint8_t msg[] = "SUCESSFULLY REFRESHED DISPLAY\n";
    return (ByteArray_t){.data = msg, .size = sizeof(msg) - 1};
  } else {
    static uint8_t msg[] = "FAILED TO REFRESH DISPLAY : BUSY";
    return (ByteArray_t){.data = msg, .size = sizeof(msg) - 1};
  }
}

const ByteArray_t orCMD(void) {
  // or toggle
  isOr = !isOr;
  static uint8_t msg[] = "TOGGLED OR MODE\n";
  return (ByteArray_t){.data = msg, .size = sizeof(msg) - 1};
}

//--------------------------------------------------------------------------------

// commands
// defining a keyword and handle for each command
const Command_t commands[] = {
    {.keyword = (const uint8_t *)"HELP\n",
     .keyword_size = 5,
     .action = helpCMD},
    {.keyword = (const uint8_t *)"DISPLAY\n",
     .keyword_size = 8,
     .action = displayImageCMD},
    {.keyword = (const uint8_t *)"REFRESH\n",
     .keyword_size = 8,
     .action = refreshCMD},
    {.keyword = (const uint8_t *)"OR\n", .keyword_size = 3, .action = orCMD},
};

//--------------------------------------------------------------------------------
// utility functions

bool stringsEqual(uint8_t *s1, uint8_t *s2, uint16_t len) {
  while (len--) {
    if (*s1++ != *s2++) {
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------------------
// private functions
void parseCommand(UART_HandleTypeDef *huart, uint16_t size) {
  // iterating through every command and testing if the keyword matches the
  // input
  for (uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    // if the received command equals a command in the command list
    if (size == commands[i].keyword_size &&
        stringsEqual(commands[i].keyword, &commandBuffer[0], size)) {
      const ByteArray_t response = commands[i].action();

      // sending response back to serial device
      HAL_UART_Transmit_IT(huart, response.data, response.size);
      return;
    }
  }

  // if no matching command was found
  HAL_UART_Transmit_IT(huart, errorMsg, sizeof(errorMsg) - 1);
}

//--------------------------------------------------------------------------------
// serial input callback
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  // restarting receive (async)
  HAL_UARTEx_ReceiveToIdle_DMA(huart, commandBuffer, sizeof(commandBuffer));

  // parsing command
  parseCommand(huart, Size);
}
