#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <vector>

/// @brief Gets a process identifier by its name.
/// @param process_name - The process to be find.
/// @returns A process identifier.
DWORD get_process_id_by_name(const LPCTSTR process_name) {
  DWORD process_id = 0;

  // Get a snapshot of all processes currently running
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  // Check if invalid snapshot
  if (snapshot == INVALID_HANDLE_VALUE) return process_id;

  PROCESSENTRY32 process_entry{ };
  process_entry.dwSize = sizeof(process_entry);

  if (Process32First(snapshot, &process_entry)) {
    // Loop through processes in snapshot
    do {
      // unicode
      if (!lstrcmpi(process_entry.szExeFile, process_name)) {
        CloseHandle(snapshot);
        return process_entry.th32ProcessID;
      }
    } while (Process32Next(snapshot, &process_entry));
  }

  CloseHandle(snapshot);
  return 0;
}

/// @brief Open a process (all access) with a given process identifier.
/// @param process_id - The identifier of the process to be opened.
/// @returns A process handle.
HANDLE open_process(DWORD process_id) {
  HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);

  return process;
}

/// @brief Gets a module base address by taking a snapshot of the given process and searching for its module name.
/// @param process_id - The identifier of the process to be included in the snapshot. This parameter can be zero to indicate the current process.
/// @param module_name - The module name to be searched. 
/// @returns An module base address.
uintptr_t get_module_base_address(DWORD process_id, const LPCTSTR module_name) {
  uintptr_t module_base_address = 0;

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);

  if (snapshot != INVALID_HANDLE_VALUE) {
    MODULEENTRY32 ModuleEntry32{ };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &ModuleEntry32)) {
      do {
        if (strcmp(ModuleEntry32.szModule, module_name) == 0) {
          module_base_address = (uintptr_t)ModuleEntry32.modBaseAddr;
          break;
        }
      } while (Module32Next(snapshot, &ModuleEntry32));
    }

    CloseHandle(snapshot);
  }

  return module_base_address;
}

/// @brief Finds a dynamic memory allocation address (DMAA) with offsets.
/// @param process - A handle to the process with memory that is being read.
/// @param p_base_address - A pointer to the base address in the specified process from which to read.
/// @param offsets - A list of offsets.
/// @returns An address.
uintptr_t findDMAA(
  HANDLE process,
  uintptr_t p_base_address,
  std::vector<uintptr_t> offsets
) {
  uintptr_t address = p_base_address;

  for (uintptr_t i = 0; i < offsets.size(); ++i) {
    ReadProcessMemory(process, (BYTE*) address, &address, sizeof(address), nullptr);
    address += offsets[i];
  }

  return address;
}
