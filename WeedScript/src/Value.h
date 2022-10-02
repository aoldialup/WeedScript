#pragma once
#include <stdio.h>
#include "Common.h"
#include <typeinfo>

// Should be refactored to an OO design whenever possible.

namespace WeedScript {

	class Object;
	class ObjectString;

	enum class ValueType
	{
		BOOL,
		NIL,
		NUMBER,
		OBJECT
	};

	class Value
	{
	public:
		Value(bool value);
		Value(double value);
		Value(Object* object);
		Value();

		static bool Equals(Value a, Value b);
		ValueType GetType() const;

		bool AsBool();
		double AsNumber();
		Object* AsObject();

		void Print();

		static bool AreValuesEqual(Value a, Value b);

		bool IsNumber() { return type == ValueType::NUMBER; }
		bool IsNil() { return type == ValueType::NIL; }
		bool IsBool() { return type == ValueType::BOOL; }
		bool IsObject() { return type == ValueType::OBJECT; }
	private:
		ValueType type;

		union
		{
			bool boolean;
			double number;
			Object* object;
		} as;
	};
}
