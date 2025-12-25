#pragma once

#ifndef NES6502_H
#define NES6502_H

#include <cstdint>
#include <string>
#include <vector>

class Bus;

class NES6502 {
public:
    NES6502(Bus *_bus);

private:      /* Memory access */
    Bus *bus; // The bus the CPU is connected to
    uint8_t ReadRam(uint16_t addr) const;
    void WriteRam(uint16_t addr, uint8_t data) const;

private: /* CPU flags */
    enum FLAGS {
        C = (1 << 0), // Carry bit flag
        Z = (1 << 1), // Zero flag
        I = (1 << 2), // Disable interrupts flag
        D = (1 << 3), // Decimal mode flag (unused in this emulator implementation)
        B = (1 << 4), // Break flag
        U = (1 << 5), // Unused flag
        V = (1 << 6), // Overflow flag
        N = (1 << 7), // Negative flag
    };

private:            /* CPU registers */
    uint8_t a;      // Accumulator register
    uint8_t x;      // X register
    uint8_t y;      // Y register
    uint8_t stkp;   // Stack pointer (points to location on bus)
    uint16_t pc;    // Program counter
    uint8_t status; // Status register

private: /* Status register access */
    uint8_t GetFlag(FLAGS flag);
    void SetFlag(FLAGS flag, bool value);

public: /* Address modes */
    // Implied address mode
    uint8_t IMP();

    // Immediate address mode
    uint8_t IMM();

    // Zero page address mode
    uint8_t ZP0();

    // Zero page address mode with X offset
    uint8_t ZPX();

    // Zero page address mode with Y offset
    uint8_t ZPY();

    // Relative address mode
    uint8_t REL();

    // Absolute address mode
    uint8_t ABS();

    // Absolute address mode with X offset
    uint8_t ABX();

    // Absolute address mode with Y offset
    uint8_t ABY();

    // Indirect address mode
    uint8_t IND();

    // Indexed address mode with X offset
    uint8_t IZX();

    // Indexed address mode with Y offset
    uint8_t IZY();

public: /* Instruction set */
    // Add memory to accumulator with carry
    uint8_t ADC();

    // AND memory with accumulator
    uint8_t AND();

    // Shift left one bit (memory or accumulator)
    uint8_t ASL();

    // Branch on carry clear
    uint8_t BCC();

    // Branch on carry set
    uint8_t BCS();

    // Branch on result zero (i.e. if equal)
    uint8_t BEQ();

    // Test bits in memory with accumulator
    uint8_t BIT();

    // Branch on result minus
    uint8_t BMI();

    // Branch on result not zero (i.e. if not equal)
    uint8_t BNE();

    // Branch on result plus
    uint8_t BPL();

    // Force break
    uint8_t BRK();

    // Branch on overflow clear
    uint8_t BVC();

    // Branch on overflow set
    uint8_t BVS();

    // Clear carry flag
    uint8_t CLC();

    // Clear decimal mode
    uint8_t CLD();

    // Clear interrupt disable bit
    uint8_t CLI();

    // Clear overflow flag
    uint8_t CLV();

    // Compare memory with accumulator
    uint8_t CMP();

    // Compare memory and index X
    uint8_t CPX();

    // Compare memory and index Y
    uint8_t CPY();

    // Decrement memory by one
    uint8_t DEC();

    // Decrement index X by one
    uint8_t DEX();

    // Decrement index Y by one
    uint8_t DEY();

    // Exclusive-OR memory with accumulator
    uint8_t EOR();

    // Increment memory by one
    uint8_t INC();

    // Increment index X by one
    uint8_t INX();

    // Increment index Y by one
    uint8_t INY();

    // Jump to new location
    uint8_t JMP();

    // Jump to new location saving return address
    uint8_t JSR();

    // Load accumulator with memory
    uint8_t LDA();

    // Load index X with memory
    uint8_t LDX();

    // Load index Y with memory
    uint8_t LDY();

    // Shift one bit right (memory or accumulator)
    uint8_t LSR();

    // No operation
    uint8_t NOP();

    // OR memory with accumulator
    uint8_t ORA();

    // Push accumulator on stack
    uint8_t PHA();

    // Push processor status on stack
    uint8_t PHP();

    // Pull accumulator from stack
    uint8_t PLA();

    // Pull processor status from stack
    uint8_t PLP();

    // Rotate one bit left (memory or accumulator)
    uint8_t ROL();

    // Rotate one bit right (memory or accumulator)
    uint8_t ROR();

    // Return from interrupt
    uint8_t RTI();

    // Return from subroutine
    uint8_t RTS();

    // Subtract memory from accumulator with borrow
    uint8_t SBC();

    // Set carry flag
    uint8_t SEC();

    // Set decimal flag
    uint8_t SED();

    // Set interrupt disable status
    uint8_t SEI();

    // Store accumulator in memory
    uint8_t STA();

    // Store index X in memory
    uint8_t STX();

    // Store index Y in memory
    uint8_t STY();

    // Transfer accumulator to index X
    uint8_t TAX();

    // Transfer accumulator to index Y
    uint8_t TAY();

    // Transfer stack pointer to index X
    uint8_t TSX();

    // Transfer index X to accumulator
    uint8_t TXA();

    // Transfer index X to stack register
    uint8_t TXS();

    // Transfer index Y to accumulator
    uint8_t TYA();

    // Capture unofficial instructions (NOP equivalent)
    // Unique to this emulator implementation
    uint8_t XXX();

    struct Instruction {
        // Instruction mnemonic
        std::string name;

        // Instruction function pointer
        uint8_t (NES6502::*instruction)(void);

        // Address mode function pointer
        uint8_t (NES6502::*addrMode)(void);

        // Clock cycles required for the instruction
        uint8_t cycles;
    };

    using is = NES6502;
    using am = NES6502;
    // Lookup table in which the index is the instruction's opcode (1 byte)
    const std::vector<Instruction> instructionSetLookup = {
        // 0x00 - 0x0F
        {"BRK", &is::BRK, &am::IMM, 7},
        {"ORA", &is::ORA, &am::IZX, 6},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 3},
        {"ORA", &is::ORA, &am::ZP0, 3},
        {"ASL", &is::ASL, &am::ZP0, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"PHP", &is::PHP, &am::IMP, 3},
        {"ORA", &is::ORA, &am::IMM, 2},
        {"ASL", &is::ASL, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::NOP, &am::IMP, 4},
        {"ORA", &is::ORA, &am::ABS, 4},
        {"ASL", &is::ASL, &am::ABS, 6},
        {"???", &is::XXX, &am::IMP, 6},

        // 0x10 - 0x1F
        {"BPL", &is::BPL, &am::REL, 2},
        {"ORA", &is::ORA, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 4},
        {"ORA", &is::ORA, &am::ZPX, 4},
        {"ASL", &is::ASL, &am::ZPX, 6},
        {"???", &is::XXX, &am::IMP, 6},
        {"CLC", &is::CLC, &am::IMP, 2},
        {"ORA", &is::ORA, &am::ABY, 4},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 7},
        {"???", &is::NOP, &am::IMP, 4},
        {"ORA", &is::ORA, &am::ABX, 4},
        {"ASL", &is::ASL, &am::ABX, 7},
        {"???", &is::XXX, &am::IMP, 7},

        // 0x20 - 0x2F
        {"JSR", &is::JSR, &am::ABS, 6},
        {"AND", &is::AND, &am::IZX, 6},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"BIT", &is::BIT, &am::ZP0, 3},
        {"AND", &is::AND, &am::ZP0, 3},
        {"ROL", &is::ROL, &am::ZP0, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"PLP", &is::PLP, &am::IMP, 4},
        {"AND", &is::AND, &am::IMM, 2},
        {"ROL", &is::ROL, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"BIT", &is::BIT, &am::ABS, 4},
        {"AND", &is::AND, &am::ABS, 4},
        {"ROL", &is::ROL, &am::ABS, 6},
        {"???", &is::XXX, &am::IMP, 6},

        // 0x30 - 0x3F
        {"BMI", &is::BMI, &am::REL, 2},
        {"AND", &is::AND, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 4},
        {"AND", &is::AND, &am::ZPX, 4},
        {"ROL", &is::ROL, &am::ZPX, 6},
        {"???", &is::XXX, &am::IMP, 6},
        {"SEC", &is::SEC, &am::IMP, 2},
        {"AND", &is::AND, &am::ABY, 4},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 7},
        {"???", &is::NOP, &am::IMP, 4},
        {"AND", &is::AND, &am::ABX, 4},
        {"ROL", &is::ROL, &am::ABX, 7},
        {"???", &is::XXX, &am::IMP, 7},

        // 0x40 - 0x4F
        {"RTI", &is::RTI, &am::IMP, 6},
        {"EOR", &is::EOR, &am::IZX, 6},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 3},
        {"EOR", &is::EOR, &am::ZP0, 3},
        {"LSR", &is::LSR, &am::ZP0, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"PHA", &is::PHA, &am::IMP, 3},
        {"EOR", &is::EOR, &am::IMM, 2},
        {"LSR", &is::LSR, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"JMP", &is::JMP, &am::ABS, 3},
        {"EOR", &is::EOR, &am::ABS, 4},
        {"LSR", &is::LSR, &am::ABS, 6},
        {"???", &is::XXX, &am::IMP, 6},

        // 0x50 - 0x5F
        {"BVC", &is::BVC, &am::REL, 2},
        {"EOR", &is::EOR, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 4},
        {"EOR", &is::EOR, &am::ZPX, 4},
        {"LSR", &is::LSR, &am::ZPX, 6},
        {"???", &is::XXX, &am::IMP, 6},
        {"CLI", &is::CLI, &am::IMP, 2},
        {"EOR", &is::EOR, &am::ABY, 4},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 7},
        {"???", &is::NOP, &am::IMP, 4},
        {"EOR", &is::EOR, &am::ABX, 4},
        {"LSR", &is::LSR, &am::ABX, 7},
        {"???", &is::XXX, &am::IMP, 7},

        // 0x60 - 0x6F
        {"RTS", &is::RTS, &am::IMP, 6},
        {"ADC", &is::ADC, &am::IZX, 6},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 3},
        {"ADC", &is::ADC, &am::ZP0, 3},
        {"ROR", &is::ROR, &am::ZP0, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"PLA", &is::PLA, &am::IMP, 4},
        {"ADC", &is::ADC, &am::IMM, 2},
        {"ROR", &is::ROR, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"JMP", &is::JMP, &am::IND, 5},
        {"ADC", &is::ADC, &am::ABS, 4},
        {"ROR", &is::ROR, &am::ABS, 6},
        {"???", &is::XXX, &am::IMP, 6},

        // 0x70 - 0x7F
        {"BVS", &is::BVS, &am::REL, 2},
        {"ADC", &is::ADC, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 4},
        {"ADC", &is::ADC, &am::ZPX, 4},
        {"ROR", &is::ROR, &am::ZPX, 6},
        {"???", &is::XXX, &am::IMP, 6},
        {"SEI", &is::SEI, &am::IMP, 2},
        {"ADC", &is::ADC, &am::ABY, 4},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 7},
        {"???", &is::NOP, &am::IMP, 4},
        {"ADC", &is::ADC, &am::ABX, 4},
        {"ROR", &is::ROR, &am::ABX, 7},
        {"???", &is::XXX, &am::IMP, 7},

        // 0x80 - 0x8F
        {"???", &is::NOP, &am::IMP, 2},
        {"STA", &is::STA, &am::IZX, 6},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 6},
        {"STY", &is::STY, &am::ZP0, 3},
        {"STA", &is::STA, &am::ZP0, 3},
        {"STX", &is::STX, &am::ZP0, 3},
        {"???", &is::XXX, &am::IMP, 3},
        {"DEY", &is::DEY, &am::IMP, 2},
        {"???", &is::NOP, &am::IMP, 2},
        {"TXA", &is::TXA, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"STY", &is::STY, &am::ABS, 4},
        {"STA", &is::STA, &am::ABS, 4},
        {"STX", &is::STX, &am::ABS, 4},
        {"???", &is::XXX, &am::IMP, 4},

        // 0x90 - 0x9F
        {"BCC", &is::BCC, &am::REL, 2},
        {"STA", &is::STA, &am::IZY, 6},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 6},
        {"STY", &is::STY, &am::ZPX, 4},
        {"STA", &is::STA, &am::ZPX, 4},
        {"STX", &is::STX, &am::ZPY, 4},
        {"???", &is::XXX, &am::IMP, 4},
        {"TYA", &is::TYA, &am::IMP, 2},
        {"STA", &is::STA, &am::ABY, 5},
        {"TXS", &is::TXS, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 5},
        {"???", &is::NOP, &am::IMP, 5},
        {"STA", &is::STA, &am::ABX, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"???", &is::XXX, &am::IMP, 5},

        // 0xA0 - 0xAF
        {"LDY", &is::LDY, &am::IMM, 2},
        {"LDA", &is::LDA, &am::IZX, 6},
        {"LDX", &is::LDX, &am::IMM, 2},
        {"???", &is::XXX, &am::IMP, 6},
        {"LDY", &is::LDY, &am::ZP0, 3},
        {"LDA", &is::LDA, &am::ZP0, 3},
        {"LDX", &is::LDX, &am::ZP0, 3},
        {"???", &is::XXX, &am::IMP, 3},
        {"TAY", &is::TAY, &am::IMP, 2},
        {"LDA", &is::LDA, &am::IMM, 2},
        {"TAX", &is::TAX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"LDY", &is::LDY, &am::ABS, 4},
        {"LDA", &is::LDA, &am::ABS, 4},
        {"LDX", &is::LDX, &am::ABS, 4},
        {"???", &is::XXX, &am::IMP, 4},

        // 0xB0 - 0xBF
        {"BCS", &is::BCS, &am::REL, 2},
        {"LDA", &is::LDA, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 5},
        {"LDY", &is::LDY, &am::ZPX, 4},
        {"LDA", &is::LDA, &am::ZPX, 4},
        {"LDX", &is::LDX, &am::ZPY, 4},
        {"???", &is::XXX, &am::IMP, 4},
        {"CLV", &is::CLV, &am::IMP, 2},
        {"LDA", &is::LDA, &am::ABY, 4},
        {"TSX", &is::TSX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 4},
        {"LDY", &is::LDY, &am::ABX, 4},
        {"LDA", &is::LDA, &am::ABX, 4},
        {"LDX", &is::LDX, &am::ABY, 4},
        {"???", &is::XXX, &am::IMP, 4},

        // 0xC0 - 0xCF
        {"CPY", &is::CPY, &am::IMM, 2},
        {"CMP", &is::CMP, &am::IZX, 6},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"CPY", &is::CPY, &am::ZP0, 3},
        {"CMP", &is::CMP, &am::ZP0, 3},
        {"DEC", &is::DEC, &am::ZP0, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"INY", &is::INY, &am::IMP, 2},
        {"CMP", &is::CMP, &am::IMM, 2},
        {"DEX", &is::DEX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 2},
        {"CPY", &is::CPY, &am::ABS, 4},
        {"CMP", &is::CMP, &am::ABS, 4},
        {"DEC", &is::DEC, &am::ABS, 6},
        {"???", &is::XXX, &am::IMP, 6},

        // 0xD0 - 0xDF
        {"BNE", &is::BNE, &am::REL, 2},
        {"CMP", &is::CMP, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 4},
        {"CMP", &is::CMP, &am::ZPX, 4},
        {"DEC", &is::DEC, &am::ZPX, 6},
        {"???", &is::XXX, &am::IMP, 6},
        {"CLD", &is::CLD, &am::IMP, 2},
        {"CMP", &is::CMP, &am::ABY, 4},
        {"NOP", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 7},
        {"???", &is::NOP, &am::IMP, 4},
        {"CMP", &is::CMP, &am::ABX, 4},
        {"DEC", &is::DEC, &am::ABX, 7},
        {"???", &is::XXX, &am::IMP, 7},

        // 0xE0 - 0xEF
        {"CPX", &is::CPX, &am::IMM, 2},
        {"SBC", &is::SBC, &am::IZX, 6},
        {"???", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"CPX", &is::CPX, &am::ZP0, 3},
        {"SBC", &is::SBC, &am::ZP0, 3},
        {"INC", &is::INC, &am::ZP0, 5},
        {"???", &is::XXX, &am::IMP, 5},
        {"INX", &is::INX, &am::IMP, 2},
        {"SBC", &is::SBC, &am::IMM, 2},
        {"NOP", &is::NOP, &am::IMP, 2},
        {"???", &is::SBC, &am::IMP, 2},
        {"CPX", &is::CPX, &am::ABS, 4},
        {"SBC", &is::SBC, &am::ABS, 4},
        {"INC", &is::INC, &am::ABS, 6},
        {"???", &is::XXX, &am::IMP, 6},

        // 0xF0 - 0xFF
        {"BEQ", &is::BEQ, &am::REL, 2},
        {"SBC", &is::SBC, &am::IZY, 5},
        {"???", &is::XXX, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 8},
        {"???", &is::NOP, &am::IMP, 4},
        {"SBC", &is::SBC, &am::ZPX, 4},
        {"INC", &is::INC, &am::ZPX, 6},
        {"???", &is::XXX, &am::IMP, 6},
        {"SED", &is::SED, &am::IMP, 2},
        {"SBC", &is::SBC, &am::ABY, 4},
        {"NOP", &is::NOP, &am::IMP, 2},
        {"???", &is::XXX, &am::IMP, 7},
        {"???", &is::NOP, &am::IMP, 4},
        {"SBC", &is::SBC, &am::ABX, 4},
        {"INC", &is::INC, &am::ABX, 7},
        {"???", &is::XXX, &am::IMP, 7}

    };

public: /* CPU signals */
    // Clock signal (synchronous)
    void Clock();

    // Reset signal (asynchronous)
    void Reset();

    // Interrupt request signal (asynchronous)
    void IRQ();

    // Non-maskable interrupt request signal (asynchronous)
    void NMI();

private: /* Internal emulation helpers */
    // Data fetching according to address mode, populates the fetched data variable
    uint8_t FetchData();

    uint8_t fetchedData; // Working input value to the ALU
    uint16_t addr_abs;   // Current absolute memory address
    uint16_t addr_rel;   // Jump-relative memory address
    uint8_t opcode;      // Current instruction's opcode
    uint8_t cycles;      // Current instruction's duration in clock cycles
};

#endif // !NES6502_H
