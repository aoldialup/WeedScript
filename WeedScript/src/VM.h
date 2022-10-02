#pragma once
#include "Chunk.h"
#include "Common.h"
#include <stdlib.h>
#include "Compiler.h"
#include "ObjectString.h"
#include "Object.h"
#include <format>

namespace WeedScript {

	enum class InterpretResult
	{
		OK,
		COMPILE_ERROR,
		RUNTIME_ERROR
	};

	class VM
	{
	public:
		InterpretResult Interpret(const char* source);
		InterpretResult Run();

		void ResetStack();

		void Push(Value value);
		Value Pop();
		Value Peek(int distance);

		void SetChunk(Chunk& chunk);
		int* GetInstructionAddr();
		void RuntimeError(const char* format, ...);

		VM();

		void SetHeadObject(Object* object);
	private:
		void Free();
		void FreeObject(Object* object);

		bool IsString(Value value);
		bool IsNumber(Value value);

		void ConcatenateStrings();
		bool IsFalsey(Value value);

		// Store the compiler and the stack of values
		Compiler m_Compiler;
		std::vector<Value> m_Stack;

		// Store instruction pointer
		uint8_t* p_IP;

		// Store the address of the top item of the stack
		Value* p_StackTop;
		
		// Linked list of objects
		Object* p_Objects;
	};
}