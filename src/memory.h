#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <vector>

uintptr_t GetModuleBaseAddress(DWORD dwProcID, const char* szModuleName)
{
  uintptr_t ModuleBaseAddress = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
  if (hSnapshot != INVALID_HANDLE_VALUE)
  {
    MODULEENTRY32 ModuleEntry32 { };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &ModuleEntry32))
    {
      do
      {
        if (strcmp((char*) ModuleEntry32.szModule, szModuleName) == 0)
        {
          ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
          break;
        }
      } while (Module32Next(hSnapshot, &ModuleEntry32));
    }
    CloseHandle(hSnapshot);
  }
  return ModuleBaseAddress;
}

// Gets dynamic memory allocation address (DMAA) with offsets
uintptr_t findDMAA(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
  uintptr_t addr = ptr;
  for (unsigned int i = 0; i < offsets.size(); ++i) {
    ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
    addr += offsets[i];
  }
  return addr;
}
