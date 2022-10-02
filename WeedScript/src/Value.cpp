#include "Value.h"
#include "ObjectString.h"

namespace WeedScript {

	Value::Value(bool value)
	{
		as.boolean = value;
		type = ValueType::BOOL;
	}

	Value::Value(double value)
	{
		as.number = value;
		type = ValueType::NUMBER;
	}

	Value::Value(Object* object)
	{
		as.object = object;
		type = ValueType::OBJECT;
	}

	Value::Value()
	{
		type = ValueType::NIL;
	}

	bool Value::Equals(Value a, Value b)
	{
		switch(a.type)
		{
			case ValueType::NUMBER:
			return a.AsNumber() == b.AsNumber();

			case ValueType::OBJECT:
			return a.AsObject() == b.AsObject();
		}
	}

	ValueType Value::GetType() const
	{
		return type;
	}

	bool Value::AsBool()
	{
		WS_ASSERT(IsBool());
		return as.boolean;
	}

	double Value::AsNumber()
	{
		WS_ASSERT(IsNumber());
		return as.number;
	}

	Object* Value::AsObject()
	{
		WS_ASSERT(IsObject());
		return as.object;
	}

	void Value::Print()
	{
		switch(type)
		{
			case ValueType::BOOL:
			printf(AsBool() ? "true" : "false");
			break;

			case ValueType::NIL:
			printf("NIL");
			break;

			case ValueType::NUMBER:
			printf("%g", AsNumber());
			break;
		}
	}

	bool Value::AreValuesEqual(Value a, Value b)
	{
		switch(a.GetType())
		{
			case ValueType::NUMBER:
			return a.AsNumber() == b.AsNumber();

			case ValueType::BOOL:
			return a.AsBool() == b.AsBool();

			case ValueType::OBJECT:
			{
				switch(a.AsObject()->GetType())
				{
					case ObjectType::STRING:
					{
						if(b.AsObject()->GetType() != ObjectType::STRING)
						{
							throw std::runtime_error("error handling is nonexistent in this code");
						}

						ObjectString* aStr = (ObjectString*)a.AsObject();
						ObjectString* bStr = (ObjectString*)b.AsObject();

						return aStr->GetLength() == bStr->GetLength() &&
							memcmp(aStr->GetData(), bStr->ToString(), aStr->GetLength()) == 0;
					}

				}

			}

			default:
			throw std::logic_error("Bad programming error");
		}
	}
}