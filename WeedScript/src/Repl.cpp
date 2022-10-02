#include "Repl.h"
#include "Common.h"
#include <string>

namespace WeedScript {

	void Repl::Start(int argc, const char** argv)
	{
		switch(argc)
		{
			case 1:
			RunLoop();
			break;

			case 2:
			RunFile(argv[1]);
			break;

			default:
			fprintf(stderr, "Usage: ws [path]\n");
			exit(64);
			break;
		}
	}

	void Repl::RunLoop()
	{
		/*
		char line[1024];

		while(true)
		{
			printf("> ");

			if(!fgets(line, sizeof(line), stdin))
			{
				printf("\n");
				break;
			}

			vm.Interpret(line);
		}
		*/

		vm.Interpret("1+1");
	}

	void Repl::RunFile(const char* filepath)
	{
		char* source = ReadFile(filepath);
		InterpretResult result = vm.Interpret(source);

		free(source);

		if(result == InterpretResult::COMPILE_ERROR)
		{
			exit(65);
		}

		if(result == InterpretResult::RUNTIME_ERROR)
		{

			exit(70);
		}
	}

	char* Repl::ReadFile(const char* filepath)
	{
		FILE* file = fopen(filepath, "rb");
		if(file == NULL)
		{
			fprintf(stderr, "Could not open file \"%s\".\n", filepath);
			exit(74);
		}

		fseek(file, 0L, SEEK_END);
		size_t fileSize = ftell(file);

		rewind(file);

		char* buffer = (char*)malloc(fileSize + 1);
		if(buffer == NULL)
		{
			fprintf(stderr, "Not enough memory to read \"%s\".\n", filepath);
			exit(74);
		}

		size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

		if(bytesRead < fileSize)
		{
			fprintf(stderr, "Could not read file \"%s\".\n", filepath);
			exit(74);
		}

		buffer[bytesRead] = '\0';
		fclose(file);

		return buffer;
	}
}