#include "pch.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>

/*
   exploit scanner uses 2 different systems.
   it uses lua_state and script_context.

   quick reminder: lua_state has not changed since november of 2021.
   scriptcontext can be found in class informer -> RBX::ScriptContext
   most likely the 3rd string from the bottom and split a x between 0.
   
   pls dont actually skid off this and cred asdf/clxyify if you use it.
   NOTE: not all 100% of functions will be run anything freezing is your fault.
*/
#define x(x) (x - 0x400000 + (DWORD)GetModuleHandleA(0)) /* aslr */
std::uintptr_t rL; /* used for using functions */
DWORD ScriptContext; /* scriptcontext holder */
DWORD ScriptContextVFT = x(0x343C92C + (DWORD)GetModuleHandleA(0)); /* replace with scriptcontextVFTable */
/* updated for version-cb81695a34b340de */

namespace Memory { /* scanning shit */
	bool Compare(const char* pData, const char* bMask, const char* szMask)
	{
		while (*szMask) {
			__try {
				if (*szMask != '?') { /* check for ques */
					if (*pData != *bMask) return 0; /* if found then return null */
				}
				++szMask, ++pData, ++bMask;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) { /* fuck exceptions */
				return 0;
			}
		}
		return 1;
	}

	DWORD ScanForContext() /* scans the scriptcontext shit */
	{
		MEMORY_BASIC_INFORMATION MBI = { 0 };
		SYSTEM_INFO SI = { 0 };
		GetSystemInfo(&SI);
		DWORD Start = (DWORD)SI.lpMinimumApplicationAddress;
		DWORD End = (DWORD)SI.lpMaximumApplicationAddress;
		do
		{
			while (VirtualQuery((void*)Start, &MBI, sizeof(MBI))) {
				if ((MBI.Protect & PAGE_READWRITE) && !(MBI.Protect & PAGE_GUARD) && !(MBI.Protect & 0x90))
				{
					for (DWORD i = (DWORD)(MBI.BaseAddress); i - (DWORD)(MBI.BaseAddress) < MBI.RegionSize; ++i)
					{
						if (Compare((const char*)i, (char*)&ScriptContextVFT, "xxxx"))/* returns */
							return i;
					}
				}
				Start += MBI.RegionSize;
			}
		} while (Start < End);
		return 0;
	}
}

int create_pipe() // not required i still did it pls create a thread dont use the actual function unless your a retard
{
	HANDLE pipe = CreateFileW(TEXT("\\\\.\\pipe\\Script"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	ConnectNamedPipe(pipe, NULL);
	std::string message = "Script";
	DWORD numWritten;
	WriteFile(pipe, message.c_str(), message.length(), &numWritten, NULL);
	return 0;
} 

std::uint32_t rbx_gettop(std::uintptr_t rl) //thanks fishy
{
	return *reinterpret_cast<std::uint32_t*>(rl + 0x14) - *reinterpret_cast<std::uint32_t*>(rl + 0x8) >> 4; //top/base offset and >>4 not changed since 2014?
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: /* create thread */ 
		MessageBoxA(NULL, "Scanning ScriptContext VFTable!!!\nInject twice if it too fast!", "Scanning", MB_TOPMOST);
		ScriptContext = Memory::ScanForContext(); /* scans VFTable */
		MessageBoxA(NULL, "Creating rL state thread.", "Threading", MB_TOPMOST);
                rL = *(std::uintptr_t*)(ScriptContext + 308) - (ScriptContext + 308); /* opperander to scan lua state */
		MessageBoxA(NULL, std::to_string(rL).c_str(), "rL", MB_TOPMOST); /* show rL */
		create_pipe(); /* create pipe just in case btw because no advanced shit */
		MessageBoxA(NULL, "Scanning complete! Try gettop?", "Completed", MB_TOPMOST);
		rbx_gettop(rL); //testing purposes
		MessageBoxA(NULL, "Completed everything, detached.", "Completed", MB_TOPMOST);
    case DLL_THREAD_ATTACH:
    if (!rbx_gettop) { exit(-1); }// gonna keep this here for the skids
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH: 
        break;
    } 
    return TRUE;
}
