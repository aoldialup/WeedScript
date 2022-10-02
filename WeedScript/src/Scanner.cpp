#include "Scanner.h"
#define _CRT_SECURE_NO_WARNINGS

namespace WeedScript {

	void Scanner::SetSource(const char* source)
	{
		start = source;
		current = source;
		line = 1;
	}

	Token Scanner::ScanToken()
	{
		SkipWhitespace();

		start = current;

		if(IsAtEnd())
		{
			return CreateToken(TokenType::_EOF);
		}

		char c = Advance();

		if(IsAlpha(c))
		{
			return CreateIdentifierToken();
		}

		if(IsDigit(c))
		{
			return CreateNumberToken();
		}

		switch(c)
		{
			case '(': return CreateToken(TokenType::LEFT_PAREN);
			case ')': return CreateToken(TokenType::RIGHT_PAREN);
			case '{': return CreateToken(TokenType::LEFT_BRACE);
			case '}': return CreateToken(TokenType::RIGHT_BRACE);
			case ';': return CreateToken(TokenType::SEMICOLON);
			case ',': return CreateToken(TokenType::COMMA);
			case '.': return CreateToken(TokenType::DOT);
			case '-': return CreateToken(TokenType::MINUS);
			case '+': return CreateToken(TokenType::PLUS);
			case '/': return CreateToken(TokenType::SLASH);
			case '*': return CreateToken(TokenType::STAR);

			case '!':
			return CreateToken(IsMatch('=') ? TokenType::BANG_EQUAL : TokenType::BANG);

			case '=':
			return CreateToken(IsMatch('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);

			case '<':
			return CreateToken(IsMatch('=') ? TokenType::LESS_EQUAL : TokenType::LESS);

			case '>':
			return CreateToken(IsMatch('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);

			case '"':
			return CreateStringToken();

			default:
			break;
		}

		return CreateErrorToken("Bullshit character.");
	}

	char Scanner::Peek()
	{
		return *current;
	}

	Token Scanner::CreateStringToken()
	{
		while(Peek() != '"' && !IsAtEnd())
		{
			if(Peek() == '\n')
			{
				line++;
			}

			Advance();
		}

		if(IsAtEnd())
		{
			return CreateErrorToken("Bullshit code: Unterminated string");
		}

		Advance();

		return CreateToken(TokenType::STRING);
	}

	Token Scanner::CreateToken(TokenType type)
	{
		Token token;
		token.Type = type;
		token.Start = start;
		token.Length = (int)(current - start);
		token.Line = line;

		return token;
	}

	Token Scanner::CreateErrorToken(const char* message)
	{
		Token token;
		token.Type = TokenType::ERROR;
		token.Start = message;
		token.Length = (int)strlen(message);
		token.Line = line;

		return token;
	}

	Token Scanner::CreateNumberToken()
	{
		while(IsDigit(Peek()))
		{
			Advance();
		}

		if(Peek() == '.' && IsDigit(PeekNext()))
		{
			Advance();

			while(IsDigit(Peek()))
			{
				Advance();
			}
		}

		return CreateToken(TokenType::NUMBER);
	}

	Token Scanner::CreateIdentifierToken()
	{
		while(IsAlphaNumeric(Peek()))
		{
			Advance();
		}

		return CreateToken(GetIdentifierType());
	}

	TokenType Scanner::CheckKeyword(int start, int length, const char* rest, TokenType type)
	{
		if(this->current - this->start == start + length &&
			memcmp(this->start + start, rest, length) == 0)
		{
			return type;
		}

		return TokenType::IDENTIFIER;
	}

	TokenType Scanner::GetIdentifierType()
	{
		switch(start[0])
		{
			case 'a': return CheckKeyword(1, 2, "nd", TokenType::AND);
			case 'c': return CheckKeyword(1, 4, "lass", TokenType::CLASS);
			case 'e': return CheckKeyword(1, 3, "lse", TokenType::ELSE);
			case 'i': return CheckKeyword(1, 1, "f", TokenType::IF);
			case 'n': return CheckKeyword(1, 2, "il", TokenType::NIL);
			case 'o': return CheckKeyword(1, 1, "r", TokenType::OR);
			case 'p': return CheckKeyword(1, 4, "rint", TokenType::PRINT);
			case 'r': return CheckKeyword(1, 5, "eturn", TokenType::RETURN);
			case 's': return CheckKeyword(1, 4, "uper", TokenType::SUPER);
			case 'v': return CheckKeyword(1, 2, "ar", TokenType::VAR);
			case 'w': return CheckKeyword(1, 4, "hile", TokenType::WHILE);

			case 'f':
			if(current - start > 1)
			{
				switch(start[1])
				{
					case 'a': return CheckKeyword(2, 3, "lse", TokenType::FALSE);
					case 'o': return CheckKeyword(2, 1, "r", TokenType::FOR);
					case 'u': return CheckKeyword(2, 1, "n", TokenType::FUN);
				}
			}
			break;

			case 't':
			if(current - start > 1)
			{
				switch(start[1])
				{
					case 'h': return CheckKeyword(2, 2, "is", TokenType::THIS);
					case 'r': return CheckKeyword(2, 2, "ue", TokenType::TRUE);
				}
			}
			break;

			default:
			return TokenType::ERROR;
		}
	}

	char Scanner::Advance()
	{
		current++;
		return current[-1];
	}

	bool Scanner::IsAtEnd()
	{
		return *current == '\0';
	}

	bool Scanner::IsMatch(char expected)
	{
		if(IsAtEnd())
		{
			return false;
		}

		if(*current != expected)
		{
			return false;
		}

		current++;

		return true;
	}

	void Scanner::SkipWhitespace()
	{
		while(true)
		{
			char c = Peek();

			switch(c)
			{
				case ' ':
				case '\r':
				case '\t':
				Advance();
				break;

				case '\n':
				line++;
				Advance();
				break;

				case '/':
				if(PeekNext() == '/')
				{
					while(Peek() != '\n' && !IsAtEnd())
					{
						Advance();
					}
				}
				else
				{
					return;
				}

				default:
				return;
			}
		}
	}

	char Scanner::PeekNext()
	{
		if(IsAtEnd())
		{
			return '\0';
		}

		return current[1];
	}
}