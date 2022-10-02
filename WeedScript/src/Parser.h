#pragma once
#include "Scanner.h"

namespace WeedScript {

	class Parser
	{
	private:
		Token current;
		Token previous;

		bool hadError;
		bool isPanicing;

		void Advance();

		void Error(const char* message);
		void ErrorAt(const Token& token, const char* message);
		void ErrorAtCurrent(const char* message);

		void Consume(TokenType type, const char* message);

		void SetScanner(Scanner* scanner)
		{
			p_Scanner = scanner;
		}

		Scanner* p_Scanner;
	public:
		friend class Compiler;
	};
}