#include "Parser.h"

namespace WeedScript {

	void Parser::Advance()
	{
		// Takes the old token and puts it in the previous field, so we could use the lexeme we want after matching a token
		previous = current;

		while(true)
		{
			// Scan the token
			current = p_Scanner->ScanToken();

			// We only want the parser to see valid tokens, so we continue to go through and report
			// errors until we hit one that isn't an error
			if(current.Type == TokenType::ERROR)
			{
				// The scanner doesn't report lexical errors. Instead it creates special error tokens and reports them here
				// RIGHT HERE IN THIS METHOD the lexical errors are reported in the parser class

				// Pulls the location out of the current token and forward it to ErrorAt
				ErrorAtCurrent(current.Start);
			}
			else
			{
				break;
			}
		}
	}

	void Parser::ErrorAt(const Token& token, const char* message)
	{
		// If there's a mistake in the user's source code, the parser will stop printing warnings,
		// because the warnings are likely a bunch of incomprehensible shit due to the parser being unable to make 
		// sense of the syntax
		if(isPanicing)
		{
			return;
		}

		isPanicing = true;

		// Print the line on which the error occurred
		fprintf(stderr, "[line %d] error", token.Line);

		if(token.Type == TokenType::_EOF)
		{
			fprintf(stderr, " at end");
		}
		else if(token.Type == TokenType::ERROR)
		{

		}
		else
		{
			// Show the lexeme because its human-readable (non-error)
			fprintf(stderr, " at '%.*s'", token.Length, token.Start);
		}

		fprintf(stderr, ": %s\n", message);
		hadError = true;
	}

	// If the scanner hands us an error token, we need to actually tell the user
	void Parser::ErrorAtCurrent(const char* message)
	{
		ErrorAt(current, message);
	}

	// Similar to advance, in the way that it reads the next token,
	// but it also validates that the token has an expected type. If not, it reports an error
	// This function is the foundation of most syntax errors in the compiler
	void Parser::Consume(TokenType type, const char* message)
	{
		if(current.Type == type)
		{
			Advance();
		}
		else
		{
			ErrorAtCurrent(message);
		}
	}

	void Parser::Error(const char* message)
	{
		// Prints the error at the previous token
		ErrorAt(previous, message);
	}
}