#pragma once
#include "Object.h"
#include <iostream>
#include <string.h>

namespace WeedScript {

	class ObjectString : public Object
	{
	public:
		ObjectString(const char* data, int length);
		~ObjectString();

		char* GetData() const;
		int GetLength() const;

		virtual const char* ToString() const override;
		virtual void DebugPrint() const override;

		static ObjectString* Combine(const ObjectString& a, const ObjectString& b)
		{
			int length = a.GetLength() + b.GetLength();
			char* chars = new char[length + 1];

			memcpy(chars, a.GetData(), a.GetLength());
			memcpy(chars + a.GetLength(), b.GetData(), b.GetLength());

			chars[length] = '\0';

			return AllocateObject<ObjectString>(chars, length);
		}

		ObjectString* ToLower();
	private:
		int m_Length;
		char* m_Data;
	};
}


