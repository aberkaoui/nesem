#include "../include/NES6502.h"
#include "../include/Bus.h"

NES6502::NES6502(Bus *_bus) {
	bus = _bus;

	/* CPU registers */
	a = 0;
	x = 0;
	y = 0;
	stkp = 0;
	pc = 0;
	status = 0;

	/* Internal emulation helpers */
	fetchedData = 0;
	addr_abs = 0;
	addr_rel = 0;
	opcode = 0;
	cycles = 0;
}

// Memory access

uint8_t NES6502::ReadRam(uint16_t addr) const { return bus->ReadRam(addr); }

void NES6502::WriteRam(uint16_t addr, uint8_t data) const { return bus->WriteRam(addr, data); }

// Status register access

uint8_t NES6502::GetFlag(FLAGS flag) { return 0; }

void NES6502::SetFlag(FLAGS flag, bool value) {}

// Address modes

uint8_t NES6502::IMP() {
	// The operand's address is implicitly given in the instruction

	fetchedData = a; // May operate on the accumulator

	return 0;
}

uint8_t NES6502::IMM() {
	// The operand is directly supplied in the instruction

	addr_abs = pc++; // The operand (data) is located in the next byte

	return 0;
}

uint8_t NES6502::ZP0() {
	// Addresses' structure:
	//
	// --------------------------------
	//  0x     XX     XX  <-- Address
	//         ^^     ^^
	//         |/     \|
	//      Page       Offset
	//  (high byte)  (low byte)
	// --------------------------------
	//
	// The memory is therefore 256 pages of 256 bytes each.

	// Here, we "shortcut" the page byte reading (which takes time!) and directly assume page zero.

	addr_abs = ReadRam(pc++); // why increment here while already in clock?
	addr_abs &=
		0x00FF; // 0x00XX for page zero (page byte at 00)
				// 8b(returned by ReadRam) into 16b will always keep high byte at 0, useless?

	return 0;
}

uint8_t NES6502::ZPX() {
	// X register offsets the absolute memory address

	addr_abs = ReadRam(pc++) + x;
	addr_abs &= 0x00FF; // same remarks as in ZP0

	return 0;
}

uint8_t NES6502::ZPY() {
	// Y register offsets the absolute memory address

	addr_abs = ReadRam(pc++) + y;
	addr_abs &= 0x00FF; // same remarks as in ZP0

	return 0;
}

uint8_t NES6502::REL() {
	// Only used for branching instructions,
	// that can't jump anywhere in the addressable space,
	// only to the current address' vicinity (at most 127 meomry locations)

	addr_rel = ReadRam(pc++);

	// Since the address in question is relative,
	// the determination of whether it is ahead or behind the current address
	// is indicated by its sign

	if (addr_rel & 0x0080) // If the 7th bit is at 1, then the address is signed
		addr_rel |= 0xFF00;

	return 0;
}

uint8_t NES6502::ABS() {
	// The operand's absolute memory address is directly supplied in the instruction

	uint16_t lo = ReadRam(pc++);
	uint16_t hi = ReadRam(pc++);
	// uint16_t hi = ReadRam(pc++) << 8; better to bitshift here?

	addr_abs = (hi << 8) | lo;
	// addr_abs = hi | lo;

	return 0;
}

uint8_t NES6502::ABX() {
	uint16_t lo = ReadRam(pc++);
	uint16_t hi = ReadRam(pc++);

	addr_abs = (hi << 8) | lo + x;

	// If the page boundary has been crossed...
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1; // ...exit with an additional clock cycle requirement
	else
		return 0; // ...exit without an additional clock cycle requirement
}

uint8_t NES6502::ABY() {
	uint16_t lo = ReadRam(pc++);
	uint16_t hi = ReadRam(pc++);

	addr_abs = (hi << 8) | lo + y;

	// If the page boundary has been crossed...
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1; // ...exit with an additional clock cycle requirement
	else
		return 0; // ...exit without an additional clock cycle requirement
}

uint8_t NES6502::IND() {
	// Similar to the absolute address mode,
	// but its operand is a pointer to the address of the data.

	uint16_t p_lo = ReadRam(pc++);
	uint16_t p_hi = ReadRam(pc++);

	uint16_t p_addr_abs = (p_hi << 8) | p_lo;

	if (p_lo != 0x00FF) // Page boundary hardware bug simulation
		// see www.nesdev.org/6502bugs.txt "*An indirect JMP (xxFF) will fail because..."
		addr_abs = (ReadRam(p_addr_abs & 0xFF00) << 8) | ReadRam(p_addr_abs);
	else // Normal behaviour
		addr_abs = (ReadRam(p_addr_abs + 1) << 8) | ReadRam(p_addr_abs);

	return 0;
}

uint8_t NES6502::IZX() {
	uint16_t zp_addr = ReadRam(pc++); // Zero page assumed

	uint16_t lo = ReadRam((zp_addr + (uint16_t)x) & 0x00FF);
	uint16_t hi = ReadRam((zp_addr + (uint16_t)x + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;

	return 0;
}

uint8_t NES6502::IZY() {
	// Same as IZX but the offset is applied to the obtained absolute address

	uint16_t zp_addr = ReadRam(pc++); // Zero page assumed

	uint16_t lo = ReadRam(zp_addr & 0x00FF);
	uint16_t hi = ReadRam((zp_addr + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo + y;

	// If the page boundary has been crossed...
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1; // ...exit with an additional clock cycle requirement
	else
		return 0; // ...exit without an additional clock cycle requirement
}

// Instruction set

uint8_t NES6502::ADC() {
	FetchData();

	uint16_t temp = (uint16_t)(a + fetchedData + GetFlag(C));

	// Status registers update
	SetFlag(C, temp > 0x00FF);		  // If the operation result holds a carry bit
	SetFlag(Z, (temp & 0x00FF) == 0); // If the operation result is 0
	SetFlag(N, temp & 0x0080);		  // If bit 7 is equal to 1
	SetFlag(V, (~((uint16_t)a ^ (uint16_t)fetchedData) & ((uint16_t)a ^ temp)) & 0x0080);
	// See overflow logic equation

	a = temp & 0x00FF; // Result stored in the accumulator

	return 1; // May require an additional clock cycle
			  // (see R650X datasheet, "Instruction set summary" table)
			  // "May" because it is added in the clock method only if
			  // the address mode too may require it
}

uint8_t NES6502::AND() {
	FetchData();

	// AND logical operation
	a &= fetchedData;

	// Status registers update
	SetFlag(Z, a == 0);
	SetFlag(N, a & 0x0080);

	return 1;
}

uint8_t NES6502::ASL() { return 0; }

uint8_t NES6502::BCC() {
	if (GetFlag(C) == 0) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BCS() {
	if (GetFlag(C) == 1) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
					  // (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BEQ() {
	if (GetFlag(Z) == 1) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BIT() { return 0; }

uint8_t NES6502::BMI() {
	if (GetFlag(N) == 1) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BNE() {
	if (GetFlag(Z) == 0) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BPL() {
	if (GetFlag(N) == 0) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BRK() { return 0; }

uint8_t NES6502::BVC() {
	if (GetFlag(V) == 0) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::BVS() {
	if (GetFlag(V) == 1) {
		cycles++; // Necessary additional clock cycle

		addr_abs = pc + addr_rel; // Absolute address update

		// If the page boundary has been crossed...
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++; // Another additional clock cycle
		// (see R650X datasheet, "Instruction set summary" table)

		pc = addr_abs; // Program counter update
	}

	return 0;
}

uint8_t NES6502::CLC() {
	SetFlag(C, false);

	return 0;
}

uint8_t NES6502::CLD() {
	SetFlag(D, false);

	return 0;
}

uint8_t NES6502::CLI() {
	SetFlag(I, false);

	return 0;
}

uint8_t NES6502::CLV() {
	SetFlag(V, false);

	return 0;
}

uint8_t NES6502::CMP() { return 0; }

uint8_t NES6502::CPX() { return 0; }

uint8_t NES6502::CPY() { return 0; }

uint8_t NES6502::DEC() { return 0; }

uint8_t NES6502::DEX() { return 0; }

uint8_t NES6502::DEY() { return 0; }

uint8_t NES6502::EOR() { return 0; }

uint8_t NES6502::INC() { return 0; }

uint8_t NES6502::INX() { return 0; }

uint8_t NES6502::INY() { return 0; }

uint8_t NES6502::JMP() { return 0; }

uint8_t NES6502::JSR() { return 0; }

uint8_t NES6502::LDA() { return 0; }

uint8_t NES6502::LDX() { return 0; }

uint8_t NES6502::LDY() { return 0; }

uint8_t NES6502::LSR() { return 0; }

uint8_t NES6502::NOP() { return 0; }

uint8_t NES6502::ORA() { return 0; }

uint8_t NES6502::PHA() {
	WriteRam(0x0100 + stkp, a); // 0x0100 is the hard coded base stack address

	stkp--;

	return 0;
}

uint8_t NES6502::PHP() { return 0; }

uint8_t NES6502::PLA() {
	a = ReadRam(0x0100 + ++stkp); // 0x0100 is the hard coded base stack address

	SetFlag(Z, a == 0);
	SetFlag(N, a & 0x0080);

	return 0;
}

uint8_t NES6502::PLP() { return 0; }

uint8_t NES6502::ROL() { return 0; }

uint8_t NES6502::ROR() { return 0; }

uint8_t NES6502::RTI() {
	// Return when the program has serviced the interrupt
	// This instruction restores the CPU to its
	// previous state before the interrupt

	status = ReadRam(0x0100 + ++stkp);
	status &= ~B;
	status &= ~U;

	pc = (uint16_t)ReadRam(0x0100 + ++stkp);
	pc |= (uint16_t)ReadRam(0x0100 + ++stkp) << 8;

	return 0;
}

uint8_t NES6502::RTS() { return 0; }

uint8_t NES6502::SBC() {
	FetchData();

	uint16_t inv = (uint16_t)fetchedData ^ 0x00FF; // Inversion for two's complement

	uint16_t temp = (uint16_t)(a + inv + GetFlag(C));

	// Status registers update
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(N, temp & 0x0080);
	SetFlag(V, (temp ^ (uint16_t)a) & (temp ^ inv) & 0x0080);

	a = temp & 0x00FF;

	return 1;
}

uint8_t NES6502::SEC() { return 0; }

uint8_t NES6502::SED() { return 0; }

uint8_t NES6502::SEI() { return 0; }

uint8_t NES6502::STA() { return 0; }

uint8_t NES6502::STX() { return 0; }

uint8_t NES6502::STY() { return 0; }

uint8_t NES6502::TAX() { return 0; }

uint8_t NES6502::TAY() { return 0; }

uint8_t NES6502::TSX() { return 0; }

uint8_t NES6502::TXA() { return 0; }

uint8_t NES6502::TXS() { return 0; }

uint8_t NES6502::TYA() { return 0; }

uint8_t NES6502::XXX() { return 0; }

// CPU signals

void NES6502::Clock() {
	if (cycles == 0) // i.e. no running instructions' cycles left
	{
		// Reading next instruction and incrementing the program counter
		opcode = ReadRam(pc++);

		// Setting required cycles for the current instruction
		cycles = instructionSetLookup[opcode].cycles;

		// Address mode and instruction calls
		uint8_t additional_cycle1 = (this->*instructionSetLookup[opcode].addrMode)();
		uint8_t additional_cycle2 = (this->*instructionSetLookup[opcode].instruction)();

		// Additional cycle if addrMode and (&) instruction both return 1
		cycles += additional_cycle1 & additional_cycle2;
	}

	cycles--;
}

void NES6502::Reset() {
	// CPU reset to default known condition

	a = 0;
	x = 0;
	y = 0;
	stkp = 0xFD;	// 0xFD is ?
	status = 0 | U; // = U ?

	addr_abs = 0xFFFC; // 0xFFFC is the hard coded address containing the
					   // address to reset the program counter in this case

	uint16_t lo = ReadRam(addr_abs);
	uint16_t hi = ReadRam(addr_abs + 1);

	pc = (hi << 8) | lo;

	addr_rel = 0;
	addr_abs = 0;
	fetchedData = 0;

	cycles = 8; // Hard coded clock cycles for this reset signal
}

void NES6502::IRQ() {
	if (GetFlag(I) == 0) {
		WriteRam(0x0100 + stkp--,
				 (pc >> 8) & 0x00FF); // 0x0100 is the hard coded base stack address
		WriteRam(0x0100 + stkp--, pc & 0x00FF);

		SetFlag(B, false);
		SetFlag(U, true);
		SetFlag(I, true);

		WriteRam(0x0100 + stkp--, status);

		addr_abs = 0xFFFE; // 0xFFFE is the hard coded address containing the
						   // address to set the program counter in this case

		uint16_t lo = ReadRam(addr_abs);
		uint16_t hi = ReadRam(addr_abs + 1);

		pc = (hi << 8) | lo;

		cycles = 7; // Hard coded clock cycles for this interrupt request signal
	}
}

void NES6502::NMI() {
	WriteRam(0x0100 + stkp--, (pc >> 8) & 0x00FF); // 0x0100 is the hard coded base stack address
	WriteRam(0x0100 + stkp--, pc & 0x00FF);

	SetFlag(B, false);
	SetFlag(U, true);
	SetFlag(I, true);

	WriteRam(0x0100 + stkp--, status);

	addr_abs = 0xFFFA; // 0xFFFA is the hard coded address containing the
					   // address to set the program counter in this case

	uint16_t lo = ReadRam(addr_abs);
	uint16_t hi = ReadRam(addr_abs + 1);

	pc = (hi << 8) | lo;

	cycles = 8; // Hard coded clock cycles for this non-maskable interrupt request signal
}

// Internal emulation helpers

uint8_t NES6502::FetchData() {
	// Data fetching from all address mode instructions except implied address mode
	// (operand is implicit in the instruction, nothing to fetch)

	if (instructionSetLookup[opcode].addrMode != &NES6502::IMP)
		fetchedData = ReadRam(addr_abs);

	return fetchedData; // In case, for any other function's use as argument or variable as value
}
