#pragma once
#include <string.h>
#include "Common.h"
#include <memory.h>

namespace WeedScript {

	enum class TokenType
	{
		LEFT_PAREN, RIGHT_PAREN,
		LEFT_BRACE, RIGHT_BRACE,
		COMMA, DOT, MINUS, PLUS,
		SEMICOLON, SLASH, STAR,
		BANG, BANG_EQUAL,
		EQUAL, EQUAL_EQUAL,
		GREATER, GREATER_EQUAL,
		LESS, LESS_EQUAL,
		IDENTIFIER, STRING, NUMBER,
		AND, CLASS, ELSE, FALSE,
		FOR, FUN, IF, NIL, OR,
		PRINT, RETURN, SUPER, THIS,
		TRUE, VAR, WHILE,
		ERROR, _EOF
	};

	struct Token
	{
		TokenType Type;
		const char* Start;
		int Length;
		int Line;
	};

	class Scanner
	{
	public:
		void SetSource(const char* source);

		Token ScanToken();
		char Advance();
	private:
		const char* start;
		const char* current;

		int line;

		bool IsAtEnd();
		bool IsMatch(char expected);

		void SkipWhitespace();
		char PeekNext();
		char Peek();

		Token CreateStringToken();
		Token CreateToken(TokenType type);
		Token CreateErrorToken(const char* message);
		Token CreateNumberToken();
		Token CreateIdentifierToken();

		TokenType GetIdentifierType();
		TokenType CheckKeyword(int start, int length, const char* rest, TokenType type);
	};
}