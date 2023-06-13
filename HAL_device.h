#ifndef HAL_DEVICE_H
#define HAL_DEVICE_H

#include <tchar.h>
#include <strsafe.h>
#include <windows.h>
#include <winusb.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <Usbiodef.h>
#include "stdint.h"
#include "types.h"
#include "usb_requests.h"


BOOL SetUartLineCoding(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, LineCoding_s* UartLineCoding);
BOOL GetUartLineCoding(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, LineCoding_s* UartLineCoding);
void PrintUartLineCoding(LineCoding_s UartLineCoding);

BOOL SetSIUMode(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, UINT8 SIUMode);
BOOL GetSIUMode(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, UINT8* SIUMode);

#endif // HAL_DEVICE_H