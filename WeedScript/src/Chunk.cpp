#include "Chunk.h"

namespace WeedScript {


	void Chunk::Write(uint8_t byte, int line)
	{
		code.push_back(byte);
		lines.push_back(line);
	}

	int Chunk::AddConstant(Value value)
	{
		// Add the constant and return the index where the constant was appended so we could locate it sometime later
		constants.push_back(value);

		return (int)constants.size() - 1;
	}

	void Chunk::WriteConstant(Value value, int line)
	{
		constants.push_back(value);

		code.push_back(OpCode::ADD);
		code.push_back(constants.size() - 1);
		lines.push_back(line);
	}
#ifdef DEBUG_PRINT_CODE
	void Chunk::Disassemble(const char* chunkName)
	{
		printf("== %s ==\n", chunkName);

		for(int offset = 0; offset < code.size();)
		{
			offset = DisassembleInstruction(offset);
		}
	}

	int Chunk::DisassembleInstruction(int offset)
	{
		printf("%04d ", offset);

		if(offset > 0 && lines[offset] == lines[offset - 1])
		{
			printf("   | ");
		}
		else
		{
			printf("%4d ", lines[offset]);
		}

		uint8_t instruction = code[offset];

		switch(instruction)
		{
			case OpCode::RETURN:
			return DisassembleBasicInstruction("OP_RETURN", offset);

			case OpCode::CONSTANT:
			return DisassembleConstant("OP_CONSTANT", offset);

			case OpCode::NEGATE:
			return DisassembleBasicInstruction("OP_NEGATE", offset);

			case OpCode::ADD:
			return DisassembleBasicInstruction("OP_ADD", offset);

			case OpCode::SUBTRACT:
			return DisassembleBasicInstruction("OP_SUBTRACT", offset);

			case OpCode::MULTIPLY:
			return DisassembleBasicInstruction("OP_MULTIPLY", offset);

			case OpCode::DIVIDE:
			return DisassembleBasicInstruction("OP_DIVIDE", offset);

			case OpCode::TRUE:
			return DisassembleBasicInstruction("OP_TRUE", offset);

			case OpCode::FALSE:
			return DisassembleBasicInstruction("OP_FALSE", offset);

			case OpCode::NIL:
			return DisassembleBasicInstruction("OP_NIL", offset);

			case OpCode::NOT:
			return DisassembleBasicInstruction("OP_NOT", offset);

			default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
		}
	}

	int Chunk::DisassembleConstant(const char* name, int offset)
	{
		uint8_t constant = code[offset + 1];
		printf("%-16s %4d '", name, constant);
		constants[constant].Print();
		printf("'\n");

		// Return the address two bytes later - to skip over the instruction and its operand
		return offset + 2;
	}

	int Chunk::DisassembleBasicInstruction(const char* name, int offset)
	{
		printf("%s\n", name);
		return offset + 1;
	}
#endif
}