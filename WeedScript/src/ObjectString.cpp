#include "ObjectString.h"

namespace WeedScript {

	ObjectString::ObjectString(const char* data, int length)
		: Object(ObjectType::STRING)
	{
		// Copy over the characters from the lexeme. The string must maintain its own data and cannot refer to the lexeme's
		// data, because it wouldn't allow concatenations or modifications to the string
		// Plus, if it referred to the lexeme's string and we freed it, bad things would happen. So yes,
		// each instance must maintain its own buffer, indepdent of the lexeme
		char* heapChars = new char[length + 1];
		WS_ASSERT(heapChars);

		memcpy(heapChars, m_Data, length);
		m_Length = length;

		heapChars[length] = '\0';
	}

	ObjectString::~ObjectString()
	{
		delete[] m_Data;
	}

	char* ObjectString::GetData() const
	{
		return m_Data;
	}

	int ObjectString::GetLength() const
	{
		return m_Length;
	}

	const char* ObjectString::ToString() const
	{
		return m_Data;
	}

	void ObjectString::DebugPrint() const
	{
		std::cout << m_Data;
	}

	ObjectString* ObjectString::ToLower()
	{
		char* chars = new char[m_Length + 1];
		WS_ASSERT(chars);
		memcpy(chars, m_Data, m_Length);
		chars[m_Length + 1] = '\0';

		//strlwr(chars);

		return new ObjectString(chars, m_Length);
	}

}