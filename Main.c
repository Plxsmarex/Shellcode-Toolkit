#include "Shellcode-Toolkit.h"

// Function types
typedef void *(*Type_LoadLibraryA)(const char *lpLibFileName);
typedef int   (*Type_MessageBoxA)(void *hWnd, char *lpText, char *lpCaption, unsigned int uType);

// Strings
createstring char User32[]  = "USER32";
createstring char Title[]   = "Shellcode Test";
createstring char Message[] = "Hello, world!";

// Entry point
int main(void)
{
	// Get address of LoadLibraryA in KERNEL32.DLL, we will use this to load USER32.DLL, which contains the MessageBoxA function
	void *Address_PEB          = GetPEBAddress();
	void *Address_KERNEL32     = GetModuleAddress(Address_PEB, 0x76918253); // "KERNEL32.DLL"
	void *Address_LoadLibraryA = GetExportAddress(Address_KERNEL32, 0x139A2F01); // "LoadLibraryA"

	// Get ready to call LoadLibraryA
	Type_LoadLibraryA LoadLibraryA = (Type_LoadLibraryA)Address_LoadLibraryA;

	// Call it and load USER32.DLL, LoadLibraryA should return its address
	void *Address_USER32 = LoadLibraryA(User32);

	// Get ready to call MessageBoxA
	Type_MessageBoxA MessageBoxA = (Type_MessageBoxA)GetExportAddress(Address_USER32, 0xDDF06316); // "MessageBoxA"

	// Call the messagebox
	MessageBoxA(0, Message, Title, 0);
	return 0;
}
