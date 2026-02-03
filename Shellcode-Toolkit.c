/*
 * GetPEBBase:
 *
 *  - Gets the base address of the x64 PEB using GS:[0x60].
 *
 *  - Returns pointer to PEB on success, 0 on failure.
 *
*/

static void *GetPEBBase() {
	void *PEB = 0;
	asm volatile("movq %%gs:0x60, %0" : "=r"(PEB));
	return PEB;
}



/*
 * GetDLLBase:
 *
 *  - Searches the Process Environment Block (PEB) loader list to find a loaded
 *    module whose DLL name hash exactly matches the selected module hash, this ensures
 *    the real module base will be returned, even if the PEB is hooked.
 *
 *  - Returns the module's base address on success, or 0 if not found.
 *
 * Parameters:
 *
 *  PEBBase = Pointer to the PEB.
 *
 *  ModuleHash = Hash for the exact DLL name to match.
 *
*/

static void *GetDLLBase(void *PEBBase, unsigned int ModuleHash) {
	unsigned char *LDRData = *(unsigned char**)((unsigned char*)PEBBase + 0x18);
	unsigned char *InLoadOrderList = LDRData + 0x20;
	unsigned char *CurrentListEntry = *(unsigned char**)(InLoadOrderList);
	for (; CurrentListEntry && CurrentListEntry != InLoadOrderList; CurrentListEntry = *(unsigned char**)(CurrentListEntry)) {
		unsigned char *ModuleEntry = CurrentListEntry - 0x10;
		void *ModuleBase = *(void**)(ModuleEntry + 0x30);
		unsigned short NameLengthBytes = *(unsigned short*)(ModuleEntry + 0x58);
		unsigned short *NameBuffer = *(unsigned short**)(ModuleEntry + 0x60);
		int NameLengthChars = (int)(NameLengthBytes >> 1);
		unsigned int ModuleHashCalc = 5381u;
		for (int NameIndex = 0; NameIndex < NameLengthChars; ++NameIndex) {
			unsigned char NameChar = (unsigned char)NameBuffer[NameIndex];
			ModuleHashCalc = ((ModuleHashCalc << 5) + ModuleHashCalc) + (unsigned int)NameChar;
		}
		if (ModuleHashCalc == ModuleHash) {
			return ModuleBase;
		}
	}
	return 0;
}



/*
 * GetExportAddress:
 *
 *  - Searches the export directory of the supplied module base to find the export address
 *    whose name hash matches ExportHash.
 *
 *  - Returns the export address on success, or 0 if not found.
 *
 * Parameters:
 *
 *  ModuleBase = Base address of the module to search.
 *
 *  ExportHash = Hash of the exact exported symbol name to locate.
 *
*/

static void *GetExportAddress(void *ModuleBase, unsigned int ExportHash) {
	unsigned int PEHeaderOffset = *(unsigned int*)(ModuleBase + 0x3C);
	unsigned char *OptionalHeader = ModuleBase + PEHeaderOffset + 4 + 20;
	unsigned short Magic = *(unsigned short*)OptionalHeader;
	unsigned char *DataDirectory = (Magic == 0x20B ? OptionalHeader + 0x70 : OptionalHeader + 0x60);
	unsigned char *ExportDirectory = ModuleBase + *(unsigned int*)DataDirectory;
	unsigned int NumberOfNames = *(unsigned int*)(ExportDirectory + 0x18);
	unsigned int AddressOfFunctionsRVA = *(unsigned int*)(ExportDirectory + 0x1C);
	unsigned int AddressOfNamesRVA = *(unsigned int*)(ExportDirectory + 0x20);
	unsigned int AddressOfNameOrdinalsRVA = *(unsigned int*)(ExportDirectory + 0x24);
	for (unsigned int Index = 0; Index < NumberOfNames; Index++) {
		const unsigned char *Name = (const unsigned char*)(ModuleBase + *(unsigned int*)(ModuleBase + AddressOfNamesRVA + Index * 4));
		unsigned int ExportHashCalc = 5381u;
		for (const unsigned char *Ptr = Name; *Ptr; ++Ptr) {
			ExportHashCalc = ((ExportHashCalc << 5) + ExportHashCalc) + (unsigned int)(*Ptr);
		}
		if (ExportHashCalc == ExportHash) {
			unsigned short Ordinal = *(unsigned short*)(ModuleBase + AddressOfNameOrdinalsRVA + Index * 2);
			void *Address = ModuleBase + *(unsigned int*)(ModuleBase + AddressOfFunctionsRVA + Ordinal * 4);
			return Address;
		}
	}
	return 0;
}
