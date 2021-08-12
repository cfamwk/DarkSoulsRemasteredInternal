#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "LDasm.h"

#include <iostream>
#define msg(x) std::cout << x << std::endl

namespace Plugin {
	namespace Utility {
		void SpawnConsole();
		void CloseConsole();
		unsigned char * FindPattern(const char * module, const char * signature);
		size_t GetInstructionSize(void* pInstruction);
	}
}
