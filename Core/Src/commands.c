/*
 * commands.c
 *
 *  Created on: Jun 19, 2026
 *      Author: Luke Fadel
 */
#include "main.h"
#include "commands.h"
#include "display.h"
#include <stdbool.h>
#include "File_006_ObjNum_005_240x128_10_27_23_C.h"

const uint8_t errorMsg[] = "\n\rCommand not found\r\n";

static const SPI_HandleTypeDef *spi;

void commandsInit(SPI_HandleTypeDef *spiInterface){
	spi = spiInterface;
}

// command action functions

// TODO: improve or remove this command
const ByteArray_t helpCMD(void) {
	static const uint8_t msg[] = "HELP MENU FOR ST7796S DISPLAY\n COMMANDS ARE: \nHELP (show this menu), \nDISPLAY (display test)\n";
	return (ByteArray_t){
		.data = msg,
		.size = sizeof(msg) -1
	};
}

const ByteArray_t displayImageCMD(void) {
//	DISPLAY_WRITE(spi, 5, 6, &File_006_ObjNum_005_240x128_10_27_23_C, false);
	static const uint8_t msg[] = "SUCESSFULLY DISPLAYED IMAGE\n";
	return (ByteArray_t){
		.data = msg,
		.size = sizeof(msg) -1
	};
}

const Command_t commands[] = {
		{
				.keyword = (const uint8_t*)"HELP\n",
				.keyword_size = 5,
				.action = helpCMD
		},
		{
				.keyword = (const uint8_t*)"DISPLAY\n",
				.keyword_size = 8,
				.action = displayImageCMD
		},
};

uint8_t commandBuffer[255];

// helper functions
bool stringsEqual(uint8_t* s1, uint8_t* s2, uint16_t len){
	while (len--){
		if (*s1++ != *s2++){
			return false;
		}
		return true;
	}
}


//public getter functions
uint8_t* getCommandBuffer(void) {
	return commandBuffer;
}

size_t getCommandSize(void) {
	return sizeof(commandBuffer);
}

//private
void parseCommand(UART_HandleTypeDef *huart, uint16_t size) {
	for (uint8_t i = 0; i < sizeof(commands)/sizeof(commands[0]) ; i++){
		// if the received command equals a command in the command list
		if (size == commands[i].keyword_size && stringsEqual(commands[i].keyword, &commandBuffer[0], size))		{
		    const ByteArray_t response = commands[i].action();

		    //sending response
		    HAL_UART_Transmit_IT(huart, response.data, response.size);
		    return;
		}
	}

	// if no matching command was found
    HAL_UART_Transmit_IT(huart, errorMsg, sizeof(errorMsg) - 1);

}

//on receive
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	//restarting receive
	HAL_UARTEx_ReceiveToIdle_DMA(huart, commandBuffer, sizeof(commandBuffer));

	//parsing command
	parseCommand(huart, Size);
}

