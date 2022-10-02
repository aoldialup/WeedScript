#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "VM.h"

namespace WeedScript {

	class Repl
	{
	public:
		void Start(int argc, const char** argv);
	private:
		void RunLoop();
		void RunFile(const char* filepath);
		char* ReadFile(const char* filepath);

		VM vm;
	};
}