#include "main.h"

void printInt(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, UINTN value) {
	CHAR16 out[32];
	CHAR16 *ptr = out;
	if (value == 0) {
		conOut->OutputString(conOut, L"0");
		return;
	}

	ptr += 31;
	*--ptr = 0;
	UINTN tmp = value;// >= 0 ? value : -value; 

	while (tmp) {
		*--ptr = '0' + tmp % 10;
		tmp /= 10;
	}
	if (value < 0) *--ptr = '-';
	conOut->OutputString(conOut, ptr);
}

void printLabeledInt(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, CHAR16 *label, UINTN value) {
	conOut->OutputString(conOut, label);
	printInt(conOut, value);
	conOut->OutputString(conOut, L"\r\n");
}

CHAR16 asChar(UINT8 nibble) {
	return nibble + (nibble < 10 ? '0' : '7');
}

void printUUID(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, UINT8 uuid[16]) {
	int charPos = 0;
	int i;
	CHAR16 *uuidStr = L"00000000-0000-0000-0000-000000000000";
	for (i = 3; i >= 0; i--) {
		uuidStr[charPos++] = asChar(uuid[i] >> 4);
		uuidStr[charPos++] = asChar(uuid[i] & 0xF);
	}
	charPos++;
	for (i = 5; i >= 4; i--) {
		uuidStr[charPos++] = asChar(uuid[i] >> 4);
		uuidStr[charPos++] = asChar(uuid[i] & 0xF);
	}
	charPos++;
	for (i = 7; i >= 6; i--) {
		uuidStr[charPos++] = asChar(uuid[i] >> 4);
		uuidStr[charPos++] = asChar(uuid[i] & 0xF);
	}
	charPos++;
	for (i = 8; i <= 9; i++) {
		uuidStr[charPos++] = asChar(uuid[i] >> 4);
		uuidStr[charPos++] = asChar(uuid[i] & 0xF);
	}

	for (i = 10; i < 16; i++) {
		if (i == 4 || i == 6 || i == 8 || i == 10) charPos++;
		uuidStr[charPos++] = asChar(uuid[i] >> 4);
		uuidStr[charPos++] = asChar(uuid[i] & 0xF);
	}
	conOut->OutputString(conOut, L"\r\n");
	conOut->OutputString(conOut, uuidStr);
}


void printDevicePath(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, EFI_DEVICE_PATH *devicePath) {
	EFI_DEVICE_PATH *node = devicePath;
	conOut->OutputString(conOut, L"Device type: ");
	conOut->OutputString(conOut, L"  ");

	for (; !IsDevicePathEnd(node); node = NextDevicePathNode(node)) {
		conOut->OutputString(conOut, L" \\ ");
		printInt(conOut, node->Type);
		conOut->OutputString(conOut, L".");
		printInt(conOut, node->SubType);

		if (node->Type == MEDIA_DEVICE_PATH && node->SubType == MEDIA_HARDDRIVE_DP) {
			conOut->OutputString(conOut, L"\r\n");

			HARDDRIVE_DEVICE_PATH *hdPath = (HARDDRIVE_DEVICE_PATH *)node;
			printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition nr: ", hdPath->PartitionNumber);
			printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition start: ", hdPath->PartitionStart);
			printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition size: ", hdPath->PartitionSize);
			printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] MBR type: ", hdPath->MBRType);
			printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Signature type: ", hdPath->SignatureType);

			printUUID(conOut, hdPath->Signature);
			conOut->OutputString(conOut, L"\r\n\r\n");
		}
	}
	conOut->OutputString(conOut, L"\r\n");
}

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);

	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

	Print(L"Hello from EFI\n");
	Print(L"Normally I'm developing Bootloader's in Legacy Assembly but here is my first EFI Bootloader\n");
	Print(L"The Advantages of EFI are definitely the unlimited size of the bootloader.\n");
	Print(L"You even not need to take care about the right size of the boot sectors. Its fantastic.\n\n");
	Print(L"Best Regards - Paul Gschwendtner!");

	Print(L"\n\nPress any key to start the Bootloader\n");
	EFI_EVENT event = SystemTable->ConIn->WaitForKey;
	SystemTable->BootServices->WaitForEvent(1, &event, NULL);

	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
	initBootloader(ImageHandle, SystemTable);

	while (TRUE) {
		//TODO KEEP OPEN
	}

	return EFI_SUCCESS;
}

VOID initBootloader(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_BOOT_SERVICES *bs = SystemTable->BootServices;

	EFI_HANDLE handles[100];
	EFI_BLOCK_IO *ioblock;
	EFI_DEVICE_PATH *devicePath;
	UINTN bufferSize = 100 * sizeof(EFI_HANDLE);

	EFI_STATUS status = bs->LocateHandle(ByProtocol, &BlockIoProtocol, NULL, &bufferSize, handles);

	UINTN handleCount = bufferSize == 0 ? 0 : bufferSize / sizeof(EFI_HANDLE);
	Print(L"Count of successfully read handles: ");
	printInt(SystemTable->ConOut, handleCount);
	Print(L"\n");

	if (EFI_ERROR(status)) {
		Print(L"Failed to locate Handles\n");
		return;
	}

	for (int i = 0; i < handleCount; i++) {
		status = bs->HandleProtocol(handles[i], &DevicePathProtocol, (void *)&devicePath);
		if (EFI_ERROR(status)) {
			Print(L"Error while reading device path\n");
			continue;
		}

		status = bs->HandleProtocol(handles[i], &BlockIoProtocol, (void *)&ioblock);
		if (EFI_ERROR(status)) {
			Print(L"Error while reading io protocol.\n");
			continue;
		}

		printDevicePath(SystemTable->ConOut, devicePath);
		
	}
}