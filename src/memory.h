#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <vector>

DWORD GetProcessIdByName(LPCTSTR processName) {
  DWORD processId = 0;

  // get a snapshot of all processes currently running
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  // check if invalid snapshot
  if (snapshot == INVALID_HANDLE_VALUE) return processId;

  PROCESSENTRY32 processEntry{ };
  processEntry.dwSize = sizeof(processEntry);

  if (Process32First(snapshot, &processEntry)) {
    // loop through processes in snapshot
    do {
      // wchar_t case insensitive string compare to find matching process name
      if (!lstrcmpi(processEntry.szExeFile, processName)) {
        CloseHandle(snapshot);
        return processEntry.th32ProcessID;
      }
    } while (Process32Next(snapshot, &processEntry));
  }

  CloseHandle(snapshot);
  return 0;
}

uintptr_t GetModuleBaseAddress(DWORD dwProcID, const char* szModuleName) {
  uintptr_t ModuleBaseAddress = 0;
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);

  if (snapshot != INVALID_HANDLE_VALUE) {
    MODULEENTRY32 ModuleEntry32 { };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &ModuleEntry32)) {
      do {
        if (strcmp((char*) ModuleEntry32.szModule, szModuleName) == 0) {
          ModuleBaseAddress = (uintptr_t) ModuleEntry32.modBaseAddr;
          break;
        }
      } while (Module32Next(snapshot, &ModuleEntry32));
    }

    CloseHandle(snapshot);
  }

  return ModuleBaseAddress;
}

/// @brief Gets dynamic memory allocation address (DMAA) with offsets
/// @param process - A handle to the process with memory that is being read.
/// @param p_base_address - A pointer to the base address in the specified process from which to read.
/// @param offsets - A list of offsets.
/// @returns An address 
uintptr_t findDMAA(
  HANDLE process,
  uintptr_t p_base_address,
  std::vector<unsigned int> offsets
) {
  uintptr_t address = p_base_address;

  for (unsigned int i = 0; i < offsets.size(); ++i) {
    ReadProcessMemory(process, (BYTE*) address, &address, sizeof(address), nullptr);
    address += offsets[i];
  }

  return address;
}
