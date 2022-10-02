#pragma once
#include "Value.h"
#include "Common.h"
#include <string.h>
#include <utility>

namespace WeedScript {

	enum class ObjectType
	{
		STRING
	};

	class Object
	{
	public:
		Object(ObjectType type);
		virtual ~Object();

		// The type of object (e.g. STRING)
		ObjectType GetType() const;

		virtual const char* ToString() const;
		virtual void DebugPrint() const;

		static bool IsObjectType(Value value, ObjectType type);
	private:
		friend class VM;

		Object* p_Next;
		ObjectType m_Type;
		// Create a linked list that stores every object. This is an 'intrusive list' because the object itself will be the linked list node
		// Each object gets a pointer to the next object in the chain. The head is stored in the VM


	};


	template <class T, class... Args> T* AllocateObject(Args &&... args)
	{
		T* ptr = new T(std::forward<Args>(args)...);


		return ptr;
	}
}