#pragma once
#include <efi.h>
#include <efilib.h>

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
VOID initBootloader(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);