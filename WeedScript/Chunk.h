#pragma once
#include <vector>

// p.g. 239

namespace OpCode {

	enum OpCode
	{
		// Return from the current function
		RETURN
	};
}

// Chunk: Sequence of bytecode. Each instruction has a one-byte operation code (opcode), which indicates which instruction it is
// Assembler: Takes a file containing human-readable mnemonic names for CPU instructions, like "ADD" and translates them to binary
// Disassembler: Given a blob of machine code, it spits out a textual listing of the instructions

class Chunk
{
public:
	void Write(uint8_t byte);

	void Disassemble(const char* name);
	int DisassembleInstruction(int offset);
	int DisassembleSimpleInstruction(const char* name, int offset);
private:
	std::vector<uint8_t> code;
};

