#include "Utility.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>
#include <string>
#include "LDasm.h"


namespace Plugin
{

	namespace Utility {

		void SpawnConsole()
		{
			AllocConsole();
			SetConsoleTitle("Debug");
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}

		void CloseConsole()
		{
			fclose(stdin);
			fclose(stdout);
			fclose(stderr);
			FreeConsole();
		}


		unsigned char * FindPattern(const char * module, const char * signature)
		{
			static auto pattern_to_byte = [](const char* pattern) {
				auto bytes = std::vector<int>{};
				auto start = const_cast<char*>(pattern);
				auto end = const_cast<char*>(pattern) + strlen(pattern);

				for (auto current = start; current < end; ++current) {
					if (*current == '?') {
						++current;
						if (*current == '?')
							++current;
						bytes.push_back(-1);
					}
					else {
						bytes.push_back(strtoul(current, &current, 16));
					}
				}
				return bytes;
			};

			PBYTE Module = (PBYTE)GetModuleHandleA(module);
			if (!Module) {
				return 0;
			}

			auto dosHeader = (PIMAGE_DOS_HEADER)Module;
			auto ntHeaders = (PIMAGE_NT_HEADERS)((unsigned char*)Module + dosHeader->e_lfanew);

			auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
			auto patternBytes = pattern_to_byte(signature);
			auto scanBytes = reinterpret_cast<uint8_t*>(Module);

			auto s = patternBytes.size();
			auto d = patternBytes.data();

			for (auto i = 0ul; i < sizeOfImage - s; ++i) {
				bool found = true;
				for (auto j = 0ul; j < s; ++j) {
					if (scanBytes[i + j] != d[j] && d[j] != -1) {
						found = false;
						break;
					}
				}
				if (found) {
					return &scanBytes[i];
				}
			}

			// Afterwards call server to stop dispatch of cheat and to alert us of update.
			return nullptr;
		}
		size_t GetInstructionSize(void * pInstruction)
		{
			ldasm_data ld = { 0 };
			return ldasm(pInstruction, &ld, true);
		}
	}
}