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
	unsigned char *ModulesListEntry = *(unsigned char**)(*(unsigned char**)((unsigned char*)PEBBase + 0x18) + 0x20);
	for (;ModulesListEntry && ModulesListEntry != *(unsigned char**)((unsigned char*)PEBBase + 0x18) + 0x20; ModulesListEntry = *(unsigned char**)(ModulesListEntry)) {
		unsigned short *NameBuffer = *(unsigned short**)(ModulesListEntry + 0x50);
		int NameIndex, NameLength = *(unsigned short*)(ModulesListEntry + 0x48) >> 1;
		unsigned int ModuleHashCalc = 5381;
		for (NameIndex = 0; NameIndex < NameLength; ++NameIndex) ModuleHashCalc = (ModuleHashCalc * 33) + NameBuffer[NameIndex];
		if (ModuleHashCalc == ModuleHash) return *(void**)(ModulesListEntry + 0x20);
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
	unsigned char *OptionalHeader = ModuleBase + *(unsigned int*)(ModuleBase + 0x3C) + 24;
	unsigned char *ExportDirectory = ModuleBase + *(unsigned int*)(*(unsigned short*)OptionalHeader == 0x20B ? OptionalHeader + 0x70 : OptionalHeader + 0x60);
	for (unsigned int Index = 0; Index < *(unsigned int*)(ExportDirectory + 0x18); Index++) {
		unsigned int ExportHashCalc = 5381;
		for (const unsigned char *Ptr = (const unsigned char*)(ModuleBase + *(unsigned int*)(ModuleBase + *(unsigned int*)(ExportDirectory + 0x20) + Index * 4)); *Ptr; ++Ptr) ExportHashCalc = ((ExportHashCalc << 5) + ExportHashCalc) + (unsigned int)(*Ptr);
		if (ExportHashCalc == ExportHash) return ModuleBase + *(unsigned int*)(ModuleBase + *(unsigned int*)(ExportDirectory + 0x1C) + *(unsigned short*)(ModuleBase + *(unsigned int*)(ExportDirectory + 0x24) + Index * 2) * 4);
	}
	return 0;
}
