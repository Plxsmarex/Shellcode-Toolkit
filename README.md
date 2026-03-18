# Shellcode-Toolkit
A simple, easy to use toolkit for generating highly optimized shellcode and tiny executables using Windows GCC.

# How to use
Simply write your code in Main.c or include the Shellcode-Toolkit header file in a new project. Put all your strings at the start using the "createstring" macro, you can use types and structs from standard header files if you want, but do not use any standard C library functions or Windows imports.

Once finished, run "Build.bat" and it will compile to a tiny executable file using GCC with a custom linker script, then it will use objcopy to extract the .text section to create the shellcode binary file. The filepaths to GCC and objcopy must be set as environment variables for this to work.

Shellcode-Toolkit.c contains a few functions which will useful in development, GetPEBAddress will get the address of the Process Environment Block (PEB), there are many important things that can be found in the PEB and related areas of memory, but for now I will just include functions for finding modules and Windows APIs, additional toolkit functions can be added as an exercise for the reader.

Main.c contains some example code for a messagebox shellcode, this compiles to a 336 byte shellcode, and a 1024 byte PE file.

# GetModuleAddress
GetModuleAddress is a function that can be used to locate modules (DLLs) in the PEB, it takes parameters for the address of the PEB (Use GetPEBAddress to get it) and a hash for the target module (More information on the hashing later). It will then locate the address of the target module by comparing its hash against the hash of every module in the PEB Ldr InMemoryOrderModuleList until there is a match. It will return the address of the target module, or 0 if not found.

GetModuleAddress's search system also bypasses PEB hooking (Fake modules in the list). GetModuleAddress can be used as a replacement for the Windows API GetModuleHandle.

# GetExportAddress
GetExportAddress functions similar to GetModuleAddress, but it is for library exports (Functions/APIs) instead of modules, this will take the address of a target module (Obtained using GetModuleAddress) and a hash for an export, it will then locate the address of the target export by comparing its hash against the hash of every export in the export directory of the selected module until there is a match. It will return the address of the found export, or 0 if not found.

GetExportAddress can be used as a replacement for the Windows API GetProcAddress.

# Hash system
Shellcode-Toolkit uses optimized 32 bit hashes for modules and exports instead of plaintext strings, this makes it harder to reverse engineer, and reduces the filesize of the compiled shellcode.

Hasher.ps1 can be used to hash strings.

# Issues
Calling a Windows API in generated shellcode executed by the C# version of my shellcode loader JITLoader causes an AccessViolationException.

The PE executable files compiled only contain a .text section which could seem suspicious, this also can make the callstack look weird.

# Credits
https://print3m.github.io/blog/from-c-to-shellcode - Using C to compile shellcode

https://redops.at/en/blog/edr-analysis-leveraging-fake-dlls-guard-pages-and-veh-for-enhanced-detection - Information about PEB Hooking.
