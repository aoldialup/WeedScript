#include "Compiler.h"
#include <stdio.h>
#include "Chunk.h"
#include "ObjectString.h"

#ifdef DEBUG_PRINT_CODE
//#include "Debug.h"
#endif

namespace WeedScript {


	// Single pass compilation
	// The compiler has roughly two jobs: it parses the user's source code to understand what it means.
	// Then it takes that knowledge and outputs low level instructions that produce those same semantics.
	// Many languages split that into two stages - two seperate passes in the implementation.
	// A parser produces an AST - just like jlox does - and then a code generator traverses the ast and outputs target code

	// In clox, we are taking an old school approach and merging these two passes into one. Back in the day language hackers,
	// did this because computers literally didn't have enough memory to store an entire source file's AST
	// We are doing it to keep our compiler simpler

	// Single-pass compilers are not always good, because the compiler has only a peephole view into the user's source code
	// You shouldn't need much surrounding context to understand a piece of syntax
	// 


	bool Compiler::Compile(const char* source, Chunk& chunk)
	{
		scanner.SetSource(source);

		parser.hadError = false;
		parser.isPanicing = false;

		scanner.Advance();
		ParseExpression();
		parser.Consume(TokenType::_EOF, "Expect end of expression");

		End();

		int line = -1;

		while(true)
		{
			Token token = scanner.ScanToken();

			if(token.Line != line)
			{
				printf("%4d ", token.Line);
				line = token.Line;
			}
			else
			{
				printf("   | ");
				printf("%2d '%.*s'\n", token.Type, token.Length, token.Start);
			}

			if(token.Type == TokenType::_EOF)
			{
				break;
			}
		}

		return !parser.hadError;
	}

	// After we parse and understand a piece of the user's program, the next step is to translate that to a series of bytecode
	// instructions
	// It starts with the easiest step: appending a single byte to a chunk

	// Writes the given byte to the chunk, which may be an opcode or operand to an instruction
	// It sends in the previous token's line information so that runtime errors are associated with that line
	void Compiler::EmitByte(uint8_t byte)
	{
		g_Chunk.Write(byte, parser.previous.Line);
	}

	void Compiler::End()
	{
		EmitReturn();
#ifdef DEBUG_PRINT_CODE
		if(!parser.hadError)
		{
			// We only do the print if the code was free of errors. After a syntax error the compiler continues,
			// but it will be in a weird state and might produce broken code. That's harmless because it won't get executed
			// but we'll confuse ourselves if we try to read it
			g_Chunk.Disassemble("Bullshit");
		}
#endif
	}

	// When you run Weed, it will parse, compile and execute an expression, then print the result
	// To print that value, we are temporarily using the OP_RETURN instruction. So we have the compiler add one 
	// of those to the end of the chunk
	void Compiler::EmitReturn()
	{
		EmitByte((uint8_t)OpCode::RETURN);
	}

	// Compiles number literals and corresponds to TOKEN_NUMBER in the enum
	void Compiler::ParseNumber()
	{
		// We assume the token for the number is already consumed and is stored in previous
		// We convert it to a double

		// No overflow/underflow check here
		double value = strtod(parser.previous.Start, NULL);
		EmitConstant(Value(value));



		//EmitConstant(NUMBER_VALUE(value));
	}

	void Compiler::String()
	{
		// Now, its the parser's turn to tokenize string literals, because the VM can already represent string values
		// This takes the string's characters directly from the lexeme. The + 1 and - 2 parts trim the leading and trailing 
		// quotation marks. Then it creates a string object, wraps it in a vlaue and puts it in the constant table

		EmitConstant(Value(new ObjectString(parser.previous.Start + 1,
			parser.previous.Length - 2)));
	}

	void Compiler::EmitConstant(Value value)
	{
		EmitBytes((uint8_t)OpCode::CONSTANT, CreateConstant(value));
	}

	// Add the value to the constant table then emit an OP_CONSTANT instruction that pushes
	// it onto the stack at runtime
	uint8_t Compiler::CreateConstant(Value value)
	{
		// Adds the given value to the end of the chunk's constant table and returns its index
		// This new function's job is to ensure we don't have too many constants. We can only have 256 constants in a chunk
		int constant = g_Chunk.AddConstant(value);

		if(constant > UINT8_MAX)
		{
			parser.Error("Too many constants in chunk");
			return 0;
		}

		return (uint8_t)constant;
	}

	// Not every expression is a single token long, most are longer
	// However, most expressions *start* with a particular token
	// We call these 'prefix' expressions
	// For example, when we parse an expression and the current token is '(', we know we must be looking
	// At a parenthesized grouping expression
	void Compiler::Grouping()
	{
		// We assume the initial '(' has already been consumed
		// We recusively call back into expression() to compile the expression between the parens, then parse the closing ')' at the end

		ParseExpression();
		parser.Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
	}

	void Compiler::Unary()
	{
		TokenType opType = parser.previous.Type;

		ParseExpression();

		switch(opType)
		{
			case TokenType::BANG:
			EmitByte(OpCode::NOT);
			break;


			case TokenType::MINUS:
			EmitByte(OpCode::NEGATE);
			break;
		}
	}

	// There will be many cases where we will need to write an opcode followed by a one-byte operand that it's worth defining this function
	void Compiler::EmitBytes(uint8_t byte1, uint8_t byte2)
	{
		EmitByte(byte1);
		EmitByte(byte2);
	}

	// Parsing tokens:
	// We map each token type to a different kind of expression. We define a function
	// for each expression that outputs the appropriate bytecode. Then we build an array of function pointers. The indexes
	// correspond to the TokenType enum and the function at each index is the code to compile an expression of that token type

	void Compiler::ParseExpression()
	{
		ParsePrecedence(Precedence::ASSIGNMENT);
	}

	void Compiler::ParsePrecedence(Precedence precedence)
	{
		parser.Advance();

		ParseFn prefixRule = GetRule(parser.previous.Type).Prefix;

		if(prefixRule == NULL)
		{
			parser.Error("Expect expression.");
		}
		else
		{
			(*this.*prefixRule)();

			while((int)precedence <= (int)GetRule(parser.current.Type).Precedence)
			{
				parser.Advance();
				ParseFn infixRule = GetRule(parser.previous.Type).Infix;

				(*this.*infixRule)();
			}
		}
	}

	void Compiler::Binary()
	{
		TokenType opType = parser.previous.Type;
		ParseRule& rule = GetRule(opType);

		ParsePrecedence((Precedence)((int)rule.Precedence + 1));

		switch(opType)
		{
			case TokenType::PLUS:
			EmitByte(OpCode::ADD);
			break;

			case TokenType::MINUS:
			EmitByte(OpCode::SUBTRACT);
			break;

			case TokenType::STAR:
			EmitByte(OpCode::MULTIPLY);
			break;

			case TokenType::SLASH:
			EmitByte(OpCode::DIVIDE);
			break;

			case TokenType::BANG_EQUAL:
			EmitBytes(OpCode::EQUAL, OpCode::NOT);
			break;

			case TokenType::EQUAL_EQUAL:
			EmitByte(OpCode::EQUAL);
			break;

			case TokenType::GREATER:
			EmitByte(OpCode::GREATER);
			break;

			case TokenType::GREATER_EQUAL:
			EmitBytes(OpCode::LESS, OpCode::NOT);
			break;

			case TokenType::LESS:
			EmitByte(OpCode::LESS);
			break;

			case TokenType::LESS_EQUAL:
			EmitBytes(OpCode::GREATER, OpCode::NOT);
			break;
		}
	}

	void Compiler::ParseLiteral()
	{
		switch(parser.previous.Type)
		{
			case TokenType::FALSE:
			EmitByte(OpCode::FALSE);
			break;

			case TokenType::NIL:
			EmitByte(OpCode::NIL);
			break;

			case TokenType::TRUE:
			EmitByte(OpCode::TRUE);
			break;
		}
	}

	Compiler::Compiler()
	{
		parser.SetScanner(&scanner);
	}
}