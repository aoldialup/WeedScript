#pragma once
#include "Scanner.h"
#include "Common.h"
#include <functional>
#include <array>
#include "Parser.h"
#include "Chunk.h"

namespace WeedScript {

	enum class Precedence
	{
		NONE = 0,
		ASSIGNMENT = 1,  // =
		OR = 2,          // or
		AND = 3,         // and
		EQUALITY = 4,    // == !=
		COMPARISON = 5,  // < > <= >=
		TERM = 6,        // + -
		FACTOR = 7,      // * /
		UNARY = 8,       // ! -
		CALL = 9,        // . ()
		PRIMARY = 10
	};

	class Compiler;
	typedef void(Compiler::* ParseFn)();

	struct ParseRule
	{
		ParseFn Prefix;
		ParseFn Infix;
		Precedence Precedence;
	};

	class Compiler
	{
	public:
		bool Compile(const char* source, Chunk& chunk);
		Compiler();

		friend class VM;
	private:
		void EmitByte(uint8_t byte);
		void EmitBytes(uint8_t byte1, uint8_t byte2);
		void EmitConstant(Value value);

		void End();
		void EmitReturn();
		void ParseNumber();

		void String();

		uint8_t CreateConstant(Value value);

		void Grouping();
		void Unary();

		void ParseExpression();
		void ParsePrecedence(Precedence precedence);
		void Binary();

		void ParseLiteral();

		ParseRule rules[43] =
		{
			/* TOKEN_LEFT_PAREN */		{ &Compiler::Grouping, NULL,  Precedence::NONE },
			/* TOKEN_RIGHT_PAREN */		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_LEFT_BRACE */		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_RIGHT_BRACE */		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_COMMA */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_DOT */				{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_MINUS */			{ &Compiler::Unary, &Compiler::Binary, Precedence::TERM },
			/* TOKEN_PLUS */			{ NULL,     &Compiler::Binary, Precedence::TERM },
			/* TOKEN_SEMICOLON */		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_SLASH */			{ NULL,     &Compiler::Binary, Precedence::FACTOR },
			/* TOKEN_STAR */			{ NULL,     &Compiler::Binary, Precedence::FACTOR },
			/* TOKEN_BANG */			{ &Compiler::Unary,     NULL,   Precedence::NONE },
			/* TOKEN_BANG_EQUAL */		{ NULL,     &Compiler::Binary,   Precedence::NONE },
			/* TOKEN_EQUAL*/			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_EQUAL_EQUAL */		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_GREATER */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_GREATER_EQUAL */	{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_LESS] */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_LESS_EQUAL]*/		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_IDENTIFIER*/		{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_STRING */			{ &Compiler::String,   NULL,   Precedence::NONE },
			/* TOKEN_STRING*/			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_NUMBER*/ 			{ &Compiler::ParseNumber,   NULL,   Precedence::NONE },
			/* TOKEN_AND */				{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_CLASS */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_ELSE */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_FALSE */			{ &Compiler::ParseLiteral, NULL,	Precedence::NONE },
			/* TOKEN_TRRUEW */			{ &Compiler::ParseLiteral, NULL,	Precedence::NONE },
			/* TOKEN_FALSE */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_FOR */				{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_FUN */				{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_IF */				{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_NIL */				{ &Compiler::ParseLiteral,     NULL,   Precedence::NONE },
			/* TOKEN_OR */				{ &Compiler::ParseLiteral, NULL,   Precedence::NONE },
			/* TOKEN_PRINT */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_RETURN */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_SUPER */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_THIS */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_TRUE */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_VAR */				{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_WHILE */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_ERROR */			{ NULL,     NULL,   Precedence::NONE },
			/* TOKEN_EOF */				{ NULL,     NULL,   Precedence::NONE }
		};

		ParseRule& GetRule(TokenType type)
		{
			// Bounds checking shouldn't be needed
			//WS_ASSERT((int)type >= 0 && (int)type < 41);

			return rules[(int)type];
		}

		Parser parser;
		Scanner scanner;
	};
}