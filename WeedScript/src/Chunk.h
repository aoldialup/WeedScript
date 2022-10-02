#pragma once
#include <vector>
#include "Value.h"

namespace WeedScript {

	// Each bytecode instruction has a one byte operation code, which indicates the type of operation it is
	enum OpCode : uint8_t
	{
		RETURN,
		CONSTANT,
		CONSTANT_LONG,
		NEGATE,
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		FALSE,
		TRUE,
		NIL,

		NOT,
		EQUAL,
		GREATER,
		LESS
	};

	// Bytecode vs tree walk interpreters vs native compilers and commonalities
	// AST interpreters are easier to write, whereas bytecode interpreters are harder
	// Both bytecode and AST are cross-platform, native compilers are not
	// Native compilers are implemented per architecture, so they are difficult to write
	// Bytecode is much faster than AST, because its a linear stream of instructions, whereas an AST is a series of nodes
	// that are all scattered in memory, which is slow as shit
	// With an AST, every piece of syntax is a node, which creates a massive overhead (32 or 64 bits of overhead) to each piece of syntax
	// AST interpreters are good for interpreters that are simple (e.g. calculator) and they have an advantage there because they 
	// are easier to write than a bytecode interpreter. For any programming language, AST interpreters are incredibly slow


	// In our format, each instruction has a one byte operation code (shortened to opcode)
	// The number controls which kind of instruction we are working with
	class Chunk
	{
	public:
		void Write(uint8_t byte, int line);
		int AddConstant(Value value);
		void WriteConstant(Value value, int line);
#ifdef DEBUG_PRINT_CODE
		void Disassemble(const char* chunkName);
		int DisassembleInstruction(int offset);
		int DisassembleBasicInstruction(const char* name, int offset);
		int DisassembleConstant(const char* name, int offset);
#endif
		friend class VM;
	private:
		// Dynamic array of instructions
		std::vector<uint8_t> code;
		std::vector<Value> constants;
		std::vector<int> lines;
	};

}

static WeedScript::Chunk g_Chunk;
