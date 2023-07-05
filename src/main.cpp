#include <iostream>
#include <thread>
#include "memory.h"
#include "offsets.h"

int main() {
  const LPCTSTR process_name = (LPCTSTR) "ac_client.exe";

  DWORD pid = get_process_id_by_name(process_name);

  if (pid == 0) {
    std::cout << "Process id could not be retrieved" << '\n';
    return 1;
  }

  std::cout << "Process id: " << pid << '\n';

  HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

  if (!process) {
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
  uintptr_t dynamic_pointer_base_address = ac_client + offset::base_address;
  std::cout << "Dynamic Pointer Base Address: 0x" << std::hex << dynamic_pointer_base_address << '\n';

  uintptr_t health_address = findDMAA(process, dynamic_pointer_base_address, { offset::health });
  std::cout << "health address = " << "0x" << std::hex << health_address << '\n';

  // Get the current health
  int health_value = 0;
  ReadProcessMemory(process, (BYTE*) health_address, &health_value, sizeof(health_value), nullptr);
  std::cout << "Health value: " << std::dec << health_value << '\n';

  while (!GetAsyncKeyState(VK_END)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (GetAsyncKeyState(VK_NUMPAD1)) {
      // Write new value to health
      health_value += 300;
      WriteProcessMemory(process, (BYTE*) health_address, &health_value, sizeof(health_value), nullptr);
    }
  }

  return 0;
}
