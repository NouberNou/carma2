#include <windows.h>
#include <stdio.h>
#include "compiler\compiler.hpp"
#include <fstream>
#include <sstream>

extern "C"
{
	__declspec(dllexport) void __stdcall RVExtension(char *output, int outputSize, const char *function);
};

uint32_t file_id;

void __stdcall RVExtension(char *output, int outputSize, const char *function)
{
	std::string input_str = std::string(function);

	carma::token_list tokens = carma::tokenize(input_str);
	
	carma::process_accessors(tokens);
	uint32_t dummy_block_counter = 0;
	carma::process_simple_assigments(tokens, tokens.begin(), dummy_block_counter);
	carma::process_method_calls(tokens, tokens.begin());
	carma::process_new_keyword(tokens, tokens.begin());
	carma::process_del_keyword(tokens, tokens.begin());
	std::stringstream filename;
	filename << "userconfig\\carma2_tmp_";
	filename << file_id++;
	filename << ".sqf";
	std::ofstream compile_file(filename.str());
	compile_file << carma::build_string(tokens);
	strncpy(output, filename.str().c_str(), outputSize);
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
