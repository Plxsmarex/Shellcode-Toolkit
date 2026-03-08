#include "Shellcode-Toolkit.c"

// createstring will put the string in the .text section
#define createstring __attribute__((section(".text.data"), used))

// Strings
createstring char User32[] = "USER32";
createstring char Title[] = "Shellcode Test";
createstring char Message[] = "Hello, world!";

__attribute__((section(".text.entrypoint"), used))
int EntryPoint() {
	// Get PEB and Kernel32 base
	void *PEBBase = GetPEBBase();
	void *Kernel32BaseAddress = GetDLLBase(PEBBase, 0x6DDB9555); // "KERNEL32.DLL"

	// Get LoadLibraryA function
	typedef void *(*LoadLibraryAType)(const char *lpLibFileName);
	LoadLibraryAType LoadLibraryA = (LoadLibraryAType)GetExportAddress(Kernel32BaseAddress, 0x5FBFF0FB); // "LoadLibraryA"

	// Load USER32.DLL
	void *User32BaseAddress = LoadLibraryA(User32);

	// Get MessageBoxA function
	typedef int (*MessageBoxAType)(void *hWnd, char *lpText, char *lpCaption, unsigned int uType);
	MessageBoxAType MessageBoxA = (MessageBoxAType)GetExportAddress(User32BaseAddress, 0x384F14B4); // "MessageBoxA"

	// Call the messagebox
	MessageBoxA(0, Message, Title, 0);
}
