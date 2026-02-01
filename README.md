# Shellcode-Toolkit
A simple, easy to use toolkit for generating highly optimized shellcode and tiny executables using Windows GCC.

# How to use
Simply write your code in Main.c. Put all your strings at the start using the "createstring" macro, the EntryPoint function will be the entry point of the program, you can use standard header files if you want, but do not use any standard C library functions.

Once done, run "Build.bat" and it will compile it to an executable file using GCC, and then use objcopy to extract the .text section to create the shellcode binary file. GCC and objcopy must be installed and on the PATH environment variable for this to work.

Shellcode-Toolkit.c contains a few functions which will be very useful in development, GetPEBBase will get the base address of the Process Environment Block (PEB), there are many useful things that can be found in the PEB and related areas of memory, but for now I will just include functions for loading Windows APIs, additional toolkit functions can be added as an exercise for the reader.

Main.c contains some example code for a messagebox shellcode, this compiles to a 416 byte shellcode.

# GetDLLBase
GetDLLBase is a function that can be used to locate modules (DLLs), it takes the base address of the PEB (Use GetPEBBase to get it) and a hash for a module (More information on the hashing later), it will then locate the base address of the inputted module by comparing its hash against the hash of every module in the PEB LDR module list. It will return the base address of the found module, or 0 if not found.

GetDLLBase's search system also bypasses PEB hooking (Fake DLLs in the module list). GetDLLBase can be used as a replacement for the Windows API GetModuleHandle.

# GetExportAddress
GetExportAddress functions similar to GetDLLBase, but it is for exports (Functions/APIs) instead of modules, this will take the base address of a target module (Obtained using GetDLLBase) and a hash for an export, it will then locate the base address of the inputted export by comparing its hash against the hash of every export in the export directory of the selected module. It will return the base address of the found export, or 0 if not found.

GetExportAddress can be used as a complete replacement for the Windows API GetProcAddress.

# Hash system
Shellcode-Toolkit uses optimized hashes for modules and exports instead of plaintext strings, this makes it harder to reverse engineer and reduces the filesize of the compiled shellcode.

Hasher.ps1 can be used to hash strings.

# Issues
The only issue I had with the Shellcode-Toolkit was that generated shellcode would not work with the C# version of my shellcode loader JITLoader.

The structure of the generated shellcode is also slightly weird and doesn't look like most other shellcode, probably because of the way this tool generates the shellcode. I believe there is a much easier and maybe better way to do it using the Linux version of the GCC C compiler, but I don't use Linux so I can't test it.

# Credits
https://print3m.github.io/blog/from-c-to-shellcode - Using C to compile shellcode

https://redops.at/en/blog/edr-analysis-leveraging-fake-dlls-guard-pages-and-veh-for-enhanced-detection - Information about PEB Hooking.
