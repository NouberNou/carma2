#include <windows.h>
#include <stdio.h>
#include "compiler\process.hpp"
#include <fstream>
#include <sstream>

extern "C"
{
	__declspec(dllexport) void __stdcall RVExtension(char *output, int outputSize, const char *function);
};

uint32_t file_id;

enum {
	COMMAND_ADD_RESERVED_WORD,
	COMMAND_COMPILE
};

void __stdcall RVExtension(char *output, int outputSize, const char *function)
{
	std::string input_str = std::string(function);
	std::string command = input_str.substr(0, 1);
	std::string value = input_str.substr(1, input_str.length() - 1);
	uint32_t command_code = std::stoi(command);

	switch (command_code) {
	case COMMAND_COMPILE: {
		std::string output_str = carma::process::process_input(value);

		if (file_id > 0) {
			std::stringstream old_filename;
			old_filename << "userconfig\\carma2_tmp_";
			old_filename << file_id;
			old_filename << ".sqf";
			std::remove(old_filename.str().c_str());
		}

		std::stringstream filename;
		filename << "userconfig\\carma2_tmp_";
		filename << file_id++;
		filename << ".sqf";

		std::ofstream compile_file(filename.str());
		compile_file << output_str;
		strncpy(output, filename.str().c_str(), outputSize);
	}
	case COMMAND_ADD_RESERVED_WORD: {
		carma::tokenizer::add_reserved_word(value);
		strncpy(output, "", outputSize);
	}

	}
}

void Init(void) {
	file_id = 0;
}

void Cleanup(void) {

}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		Cleanup();
		break;
	}
	return TRUE;
}
