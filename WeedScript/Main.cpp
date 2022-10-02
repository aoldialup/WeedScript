#include "Chunk.h"

int main(int argc, const char** argv)
{
	Chunk chunk;
	chunk.Write(OpCode::RETURN);
	chunk.Disassemble("Shit");

	return 0;
}