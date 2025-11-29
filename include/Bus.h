#pragma once

#ifndef BUS_H
#define BUS_H

#include <cstdint>

#include "NES6502.h"

class Bus {
	NES6502 cpu;
	uint8_t *ram;

public:
	Bus();
	~Bus() { delete[] ram; }

	uint8_t ReadRam(uint16_t addr, bool bReadOnly = false) const;
	void WriteRam(uint16_t addr, uint8_t data) const;
};

#endif // !BUS_H
