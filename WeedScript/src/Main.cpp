#include "Repl.h"

int main(int argc, const char** argv)
{
	WeedScript::Repl repl;
	repl.Start(argc, argv);

	return 0;
}
