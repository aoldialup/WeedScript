#include "Chunk.h"

void Chunk::Write(uint8_t byte)
{
	code.push_back(byte);
}

void Chunk::Disassemble(const char* name)
{
	// Print a header so we could see which chunk we're looking at
	printf("== %s ==\n", name);

	int offset = 0;

	while(offset < code.size())
	{
		// DisassembleInstruction disassembles and returns the offset of the next instruction
		offset = DisassembleInstruction(offset);
	}
}

int Chunk::DisassembleInstruction(int offset)
{
	printf("%04d ", offset);

	uint8_t instruction = code[offset];

	switch(instruction)
	{
		case OpCode::RETURN:
		return DisassembleSimpleInstruction("OP_RETURN", offset);

		default:
		printf("Unknown opcode %d\n", instruction);
		return offset + 1;
	}
}

// Simple instructions, such as return, that don't have much going on
int Chunk::DisassembleSimpleInstruction(const char* name, int offset)
{
	printf("%s\n", name);
	// Return the byte offset past the current instruction
	return offset + 1;
}

