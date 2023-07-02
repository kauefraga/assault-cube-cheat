#include <iostream>
#include "memory.h"

DWORD GetProcessIdByName(LPCTSTR processName) {
  DWORD processId = 0;

  // get a snapshot of all processes currently running
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  // check if invalid snapshot
  if (snapshot == INVALID_HANDLE_VALUE) return processId;

  PROCESSENTRY32 processEntry { };
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

int main() {
  const char* processName = "ac_client.exe";

  DWORD pid = GetProcessIdByName(TEXT(processName));

  if (pid == 0) {
    std::cout << "Process id could not be retrieved" << '\n';
    return 1;
  }

  std::cout << pid << '\n';

  HANDLE handle_process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

  if (!handle_process) {
    std::cout << "Process could not be opened" << '\n';
    return 1;
  }

  // Module base address
  uintptr_t ac_client = GetModuleBaseAddress(pid, processName);

  if (ac_client == 0) {
    std::cout << "Base address could not be retrieved" << '\n';
    return 1;
  }

  // EC

  // get base address of pointer chain (entity object pointer)
  // ac_client.exe+0018AC00
  uintptr_t dynamicPtrBaseAddr = ac_client + 0x18AC00;

  std::cout << "DynamicPtrBaseAddr: 0x" << std::hex << dynamicPtrBaseAddr << '\n';

  std::vector<unsigned int> health_offsets = { 0xEC };

  uintptr_t health_address = findDMAA(handle_process, dynamicPtrBaseAddr, health_offsets);
  std::cout << "health address = " << "0x" << std::hex << health_address << std::endl;

  // Get the current health
  int health_value = 0;
  ReadProcessMemory(handle_process, (BYTE*) health_address, &health_value, sizeof(health_value), nullptr);
  std::cout << "Health value: " << std::dec << health_value << '\n';

  // Write new value to health
  int new_health_value = 300;
  WriteProcessMemory(handle_process, (BYTE*) health_address, &new_health_value, sizeof(new_health_value), nullptr);

  // Read the health again
  ReadProcessMemory(handle_process, (BYTE*) health_address, &health_value, sizeof(health_value), nullptr);
  std::cout << "Health value: " << std::dec << health_value << '\n';

  return 0;
}
