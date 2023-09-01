# RM_GB_Emu Project

## Useful Links

- [Game BoyTM CPU Manual](http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf)

- [Pandocs](https://gbdev.io/pandocs/Specifications.html)

- [Clang format](https://skillupwards.com/blog/formatting-c-cplusplus-code-using-clangformat-and-vscode)

- [8080 Datasheet](http://bitsavers.trailing-edge.com/components/intel/MCS80/98-153B_Intel_8080_Microcomputer_Systems_Users_Manual_197509.pdf)

## How to build

1. visit : https://vcpkg.io/en/getting-started
2. select platform Windows/Linux. Do step 1&2.
3. cmake -B [build directory] -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake

Or configure `settings.json` file

```json
    "cmake.configureArgs": [
        "-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake"
    ],
```

4. Clang format configuration

```json
    "clang-format.executable": "clang-format",
    "clang-format.style": "file",
    "clang-format.fallbackStyle": "Microsoft",
    "clang-format.formatOnSave": true,
    "clang-format.formatOnType": true,
    "clang-format.arguments": [
        "-assume-filename=${workspaceFolder}/.clang-format"
    ]
```


