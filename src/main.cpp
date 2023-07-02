#include <iostream>
#include <thread>
#include "memory.h"

namespace offset {
  const int base_address = 0x18AC00;
  const int health = 0xEC;
}

int main() {
  const char* process_name = "ac_client.exe";

  DWORD pid = get_process_id_by_name(TEXT(process_name));

  if (pid == 0) {
    std::cout << "Process id could not be retrieved" << '\n';
    return 1;
  }

  std::cout << "Process id: " << pid << '\n';

  HANDLE handle_process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

  if (!handle_process) {
    std::cout << "Process could not be opened" << '\n';
    return 1;
  }

  // Module base address
  uintptr_t ac_client = get_module_base_address(pid, process_name);

  if (ac_client == 0) {
    std::cout << "Base address could not be retrieved" << '\n';
    return 1;
  }

  // get base address of pointer chain (entity object pointer)
  // ac_client.exe+0018AC00
  uintptr_t dynamic_pointer_base_address = ac_client + offset::base_address;
  std::cout << "Dynamic Pointer Base Address: 0x" << std::hex << dynamic_pointer_base_address << '\n';

  uintptr_t health_address = findDMAA(handle_process, dynamic_pointer_base_address, { offset::health });
  std::cout << "health address = " << "0x" << std::hex << health_address << std::endl;

  // Get the current health
  int health_value = 0;
  ReadProcessMemory(handle_process, (BYTE*) health_address, &health_value, sizeof(health_value), nullptr);
  std::cout << "Health value: " << std::dec << health_value << '\n';

  while (!GetAsyncKeyState(VK_END)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (GetAsyncKeyState(VK_NUMPAD1)) {
      // Write new value to health
      int new_health_value = 300;
      WriteProcessMemory(handle_process, (BYTE*)health_address, &new_health_value, sizeof(new_health_value), nullptr);
    }
  }

  return 0;
}
