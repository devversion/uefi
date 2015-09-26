#include <efi.h>
#include <efilib.h>

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_INPUT_KEY Key;

	InitializeLib(ImageHandle, SystemTable);

	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

	Print(L"Hello from EFI\n");
	Print(L"Normally I'm developing Bootloader's in Legacy Assembly but here is my first EFI Bootloader\n");
	Print(L"The Advantages of EFI are definitely the unlimited size of the bootloader.\n");
	Print(L"You even not need to take care about the right size of the boot sectors. Its fantastic.\n\n");
	Print(L"Best Regards - Paul Gschwendtner!");

	while (TRUE) {
		//TODO KEEP OPEN
	}

	return EFI_SUCCESS;
}
