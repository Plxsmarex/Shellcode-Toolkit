#include "Shellcode-Toolkit.c"

// createstring will put the string in the .text section
#define createstring __attribute__((section(".text.data"), used))

// Strings
createstring char User32[] = "USER32";
createstring char Title[] = "Shellcode Test";
createstring char Message[] = "Hello, world!";

__attribute__((section(".text.entrypoint"), used))
int EntryPoint() {
	// Function and module hashes
	unsigned int Kernel32Hash = 0x6DDB9555; // "KERNEL32.DLL"
	unsigned int User32Hash = 0x2208CF13; // "USER32.DLL"
	unsigned int LoadLibraryAHash = 0x5FBFF0FB; // "LoadLibraryA"
	unsigned int MessageBoxAHash = 0x384F14B4; // "MessageBoxA"
	//unsigned int ExitProcessHash = 0xB769339E; // "ExitProcess"

	// Get PEB and Kernel32 base
	void *PEBBase = GetPEBBase();
	void *Kernel32BaseAddress = GetDLLBase(PEBBase, Kernel32Hash);

	// Get LoadLibraryA function
	void *LoadLibraryAPointer = GetExportAddress(Kernel32BaseAddress, LoadLibraryAHash);
	typedef void (*LoadLibraryAType)(const char *lpLibFileName);
	LoadLibraryAType LoadLibraryA = (LoadLibraryAType)LoadLibraryAPointer;

	// Load USER32.DLL (Loading modules from shellcode like this is very suspicious to EDR, only included for this example)
	LoadLibraryA(User32);
	void *User32BaseAddress = GetDLLBase(PEBBase, User32Hash);

	// Get MessageBoxA function
	void *MessageBoxAPointer = GetExportAddress(User32BaseAddress, MessageBoxAHash);
	typedef int (*MessageBoxAType)(void *hWnd, char *lpText, char *lpCaption, unsigned int uType);
	MessageBoxAType MessageBoxA = (MessageBoxAType)MessageBoxAPointer;

	// Get ExitProcess function
	//void *ExitProcessPointer = GetExportAddress(Kernel32BaseAddress, ExitProcessHash);
	//typedef void (*ExitProcessType)(unsigned int uExitCode);
	//ExitProcessType ExitProcess = (ExitProcessType)ExitProcessPointer;

	// Call the messagebox
	MessageBoxA(0, Message, Title, 0);

	// Exit the process
	//ExitProcess(0);
}
