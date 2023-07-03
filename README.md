# Assault Cube Cheat

> 🥽 I am currently learning about game hacking, windows api, memory and to test some functions and techniques i choose Assault cube game.

### Features

- It can modify the health value.
- It has a namespace containing the offsets that i used.
- Written with x86 and c++ 20.
- If you need a memory handler, i wrote one, look [memory.h](src/memory.h).

## Getting Started

1. Download the .exe
2. Open the Assault Cube and select a map (empty)
3. Double click the .exe
4. Done! Press numpad 1 to assign you new health.

### Build

1. Clone the repository
2. Open the solution file in your Visual Studio
3. Build for x86 (release)
4. Find the .exe at `bin/Win32/Release/assault-cube-cheat.exe`

```bash
# (1)
git clone https://github.com/kauefraga/assault-cube-cheat.git
cd assault-cube-cheat

# (4)
# Take the .exe and move it closer (root)
mv bin/Win32/Release/assault-cube-cheat.exe .
```

You are welcome to open issues and pull requests!

## 🧻 Resources

These are the contents that i consume to develop this cheat:

- [Cheat Engine](https://cheatengine.org) - It is a memory scanner and it helped me a lot when i was looking for the base pointer address and the offsets.
    - [How to get offsets with Cheat Engine](https://www.youtube.com/watch?v=8oC0w6WhZ1E).
- [@mattlau1](https://github.com/mattlau1) - [Assault Cube Cheat](https://github.com/mattlau1/assault-cube-cheats/blob/main/external_cheat/main.cpp).
- [Read Memory of Process](https://stackoverflow.com/questions/48208782/read-memory-of-process-c).
- [Can't Get Process Id (windows)](https://stackoverflow.com/questions/31147690/c-cant-get-process-id-windows).

## 📝 License

This project is licensed under the MIT License - See the [LICENSE](https://github.com/kauefraga/assault-cube-cheat/blob/main/LICENSE) for more information.
