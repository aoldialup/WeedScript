#include "VM.h"
#include <cstdarg>

namespace WeedScript {

	InterpretResult VM::Interpret(const char* source)
	{
		Chunk chunk;

		if(!m_Compiler.Compile(source, chunk))
		{
			return InterpretResult::COMPILE_ERROR;
		}

		SetChunk(chunk);

		return Run();
	}

	void VM::SetHeadObject(Object* object)
	{
		object->p_Next = p_Objects;
		p_Objects = object;
	}

	void VM::Free()
	{
		Object* object = p_Objects;

		while(object != nullptr)
		{
			Object* next = object->p_Next;
			FreeObject(object);
			object = next;
		}
	}

	void VM::FreeObject(Object* object)
	{
		delete object;

		/*
		switch(object->GetType())
		{
			case ObjectType::STRING:
			ObjectString* string = (ObjectString*)object;

			// Will delete the string object and call its destructor, which destroys its array
			delete string;
			break;
		}
		*/
	}

	bool VM::IsString(Value value)
	{
		return value.GetType() == ValueType::OBJECT &&
			value.AsObject()->GetType() == ObjectType::STRING;
	}



	bool VM::IsNumber(Value value)
	{
		return value.GetType() == ValueType::NUMBER;
	}

	InterpretResult VM::Run()
	{
		ResetStack();

#define READ_BYTE() (*p_IP++)
#define READ_CONSTANT() (g_Chunk.constants[READ_BYTE()])

#define BINARY_OP(op) \
	do { \
	if(!(Peek(0).IsNumber()) || !Peek(1).IsNumber()) { \
		RuntimeError("Bullshit operands"); \
		return InterpretResult::RUNTIME_ERROR; \
	} \
		double b = Pop().AsNumber(); \
		double a = Pop().AsNumber(); \
		Push({ a op b }); \
	} while(false);

		while(true)
		{
#ifdef DEBUG_TRACE_EXECUTION
			printf("     ");
			for(Value* slot = m_Stack.data(); slot < p_StackTop; slot++)
			{
				printf("[ ");
				slot->Print();
				printf(" ]");
			}
			printf("\n");
			g_Chunk.DisassembleInstruction(*GetInstructionAddr());
#endif
			uint8_t instruction;

			switch(instruction = READ_BYTE())
			{
				case OpCode::RETURN:
				Pop().Print();
				printf("\n");
				return InterpretResult::OK;

				case OpCode::ADD:
				{
					if(IsString(Peek(0)) && IsString(Peek(1)))
					{
						ConcatenateStrings();
					}
					else if(IsNumber(Peek(0)) && IsNumber(Peek(1)))
					{
						double b = Pop().AsNumber();
						double a = Pop().AsNumber();

						Push(Value(a + b));
					}
					else
					{
						RuntimeError("Operands must be two numbers or two strings");
						return InterpretResult::RUNTIME_ERROR;
					}

					break;
				}

				case OpCode::SUBTRACT:
				BINARY_OP(-);
				break;

				case OpCode::MULTIPLY:
				BINARY_OP(*);
				break;

				case OpCode::DIVIDE:
				BINARY_OP(/);
				break;

				case OpCode::NOT:
				Push(Value(IsFalsey(Pop())));
				break;

				case OpCode::LESS:
				BINARY_OP(<);
				break;

				case OpCode::NEGATE:
				// Check if the value on top of the stack is a number, because that must be the case
				// if we want to use the negate operator
				if(Peek(0).GetType() != ValueType::NUMBER)
				{
					RuntimeError("Operand must be a number");
					return InterpretResult::RUNTIME_ERROR;
				}
				Push(-Pop().AsNumber());
				//m_Stack.back() = -m_Stack.back();
				break;

				case OpCode::CONSTANT:
				{
					Value constant = READ_CONSTANT();
					Push(constant);
					(constant.Print());
					printf("\n");
				}
				break;


			}
		}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
	}

	void VM::ResetStack()
	{
		p_StackTop = m_Stack.data();
	}

	void VM::Push(Value value)
	{
		m_Stack.push_back(value);

		p_StackTop = &m_Stack.back();
		*p_StackTop = value;
	}

	Value VM::Pop()
	{
		WS_ASSERT(!m_Stack.empty());

		p_StackTop--;
		return *p_StackTop;
	}

	Value VM::Peek(int distance)
	{
		// Distance: how far down the stack to go. 0 is the top
		return p_StackTop[-1 - distance];
	}

	void VM::SetChunk(Chunk& chunk)
	{
		g_Chunk = chunk;
		p_IP = &chunk.code[0];
	}

	int* VM::GetInstructionAddr()
	{
		return (int*)(p_IP - g_Chunk.code.data());
	}


	// Replace with variadic template
	void VM::RuntimeError(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
		fputs("\n", stderr);

		size_t instruction = (size_t)GetInstructionAddr() - 1;

		int line = g_Chunk.lines[instruction];

		std::cerr << std::format("[line {}] in script\n", line) << std::endl;
		ResetStack();
	}

	VM::VM()
	{
		ResetStack();

		// When we first initialize the VM, there are no allocated objects
		p_Objects = nullptr;
	}

	bool VM::IsFalsey(Value value)
	{
		return value.IsNil() || (value.IsBool() && !value.AsBool());
	}

	void VM::ConcatenateStrings()
	{
		ObjectString* b = (ObjectString*)Pop().AsObject();
		ObjectString* a = (ObjectString*)Pop().AsObject();

		ObjectString* result = ObjectString::Combine(*b, *a);

		Push(Value(result));
	}
}
