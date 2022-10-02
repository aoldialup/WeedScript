#include "Object.h"

namespace WeedScript {

	ObjectType Object::GetType() const
	{
		return m_Type;
	}

	const char* Object::ToString() const
	{
		return "Object";
	}

	void Object::DebugPrint() const
	{
		printf("Object");
	}

	bool Object::IsObjectType(Value value, ObjectType type)
	{
		return value.GetType() == ValueType::OBJECT &&
			value.AsObject()->GetType() == ObjectType::STRING;
	}

	Object::Object(ObjectType type)
		: m_Type(type), p_Next(nullptr)
	{

	}

	Object::~Object()
	{

	}
}
