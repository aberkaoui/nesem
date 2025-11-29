/*
 *
 * NES Emulator
 * From javidx9's excellent tutorial :
 * https://www.youtube.com/playlist?list=PLrOv9FMX8xJHqMvSGB_9G9nZZ_4IgteYf
 *
 * I purely reproduced it from his video tutorials to shape it my way from the
 * get go, understand it better and eventually improve it. I recommend you check
 * out his repository as well if you want more thorough and verbose comments.
 *
 * Datasheets used for reference (or at least visited):
 * - http://matthieu.benoit.free.fr/cross/data_sheets/R6502.pdf
 * -
 * https://web.archive.org/web/20221112220234if_/http://archive.6502.org/datasheets/rockwell_r65c00_microprocessors.pdf
 * -
 * https://web.archive.org/web/20220121022728if_/http://archive.6502.org/datasheets/rockwell_r65c00-21_r65c29.pdf
 *
 * Todo:
 * - Check OS portability of cstdint data types (uint8_t, ...)
 * - Proper global documentation
 * - Check pertinence of uint8_t for additional_cycleX, as well as return type
 * for address mode methods, because they return 0 or 1.
 * - Consider replacing FetchData with FetchOperand, and same for fetchedData
 * and any comments
 */

#include <iostream>

#include "../include/Bus.h"

int main() {
	Bus b;

	// Ridiculous test
	b.WriteRam(0xF000, 70);
	std::cout << b.ReadRam(0xF000);

	return 0;
}
