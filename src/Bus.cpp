#include "../include/Bus.h"

/*

Overview of the NES data bus

      ┌───────────┐ ┌──────────────────────────┐
      │┆┆┆┆┆┆┆┆┆┆┆│ │          64 KiB          │
      │┆┆┆6502┆┆┆┆│ │                          │
      │┆┆┆┆┆┆┆┆┆┆┆│ │           RAM            │
      │┆┆┆CPU┆┆┆┆┆│ │                          │
      │┆┆┆┆┆┆┆┆┆┆┆│ │                          │
      └──│────∧───┘ └────────────∧─────────────┘
        A│   D│                  │
         │    │                  │
      ┌──∨────∨──────────────────∨─────────────┐
      │==================Bus===================│
      └────────────────────────────────────────┘
      │                                        │
     0x0000                                   0xFFFF

*/

Bus::Bus() : cpu(this) {
    const unsigned int RAM_SIZE = 64 * 1024;
    ram = new uint8_t[RAM_SIZE];
    unsigned int i = 0;
    while (i < RAM_SIZE) {
        ram[i++] = 0;
    }
    cpu.ZP0();
}

uint8_t Bus::ReadRam(uint16_t addr, bool bReadOnly) const {
    // RAM address space
    if (addr >= 0x0000 && addr <= 0xFFFF)
        return ram[addr];

    // Out-of-bounds space
    return 0;
}

void Bus::WriteRam(uint16_t addr, uint8_t data) const {
    // RAM address space
    if (addr >= 0x0000 && addr <= 0xFFFF)
        ram[addr] = data;
}
