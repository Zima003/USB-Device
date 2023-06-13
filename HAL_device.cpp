#include "HAL_device.h"

BOOL SetUartLineCoding(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, LineCoding_s* UartLineCoding)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    ULONG cbSent = 0;
	
	SetupPacket.RequestType = (REQUEST_TYPE_SET_LINE_CODING & 0xFF00) >> 8;
    SetupPacket.Request = REQUEST_TYPE_SET_LINE_CODING & 0x00FF;
    SetupPacket.Value = SET_LINE_CODING_VALUE;
    SetupPacket.Index = InterfaceNumber;
    SetupPacket.Length = SET_LINE_CODING_LENGTH;
	
	bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, (PUCHAR)UartLineCoding, SetupPacket.Length, &cbSent, 0);
	
	return bResult;
}

BOOL GetUartLineCoding(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, LineCoding_s* UartLineCoding)
{
		if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    ULONG cbSent = 0;
	
	SetupPacket.RequestType = (REQUEST_TYPE_GET_LINE_CODING & 0xFF00) >> 8;
    SetupPacket.Request = REQUEST_TYPE_GET_LINE_CODING & 0x00FF;
    SetupPacket.Value = GET_LINE_CODING_VALUE;
    SetupPacket.Index = InterfaceNumber;
    SetupPacket.Length = GET_LINE_CODING_LENGTH;
	
	bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, (PUCHAR)UartLineCoding, SetupPacket.Length, &cbSent, 0);
	
	return bResult;
}

void PrintUartLineCoding(LineCoding_s UartLineCoding)
{
	printf("\nBaudrate %d", UartLineCoding.DTERate);
	switch(UartLineCoding.CharFormat)
	{
		case 0:
		{
			printf("\n1 stop bit");
			break;
		}
		case 1:
		{
			printf("\n1,5 stop bit");
			break;
		}
		case 2:
		{
			printf("\n2 stop bits");
			break;
		}
		
	}
	switch(UartLineCoding.ParityType)
	{
		case 0:
		{
			printf("\nParity type: non parity bit");
			break;
		}
		case 1:
		{
			printf("\nParity type: odd parity bit");
			break;
		}
		case 2:
		{
			printf("\nParity type: even parity bit");
			break;
		}
		case 3:
		{
			printf("\nParity type: mark parity bit");
			break;
		}
		case 4:
		{
			printf("\nParity type: space parity bit");
			break;
		}
	}
	printf("\nData bits quantity %d", UartLineCoding.DataBits);
}

BOOL SetSIUMode(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, UINT8 SIUMode)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

    //Create the setup packet
    SetupPacket.RequestType = (REQUEST_TYPE_SET_INTERFACES_CONFIG & 0xFF00) >> 8;
    SetupPacket.Request = (REQUEST_TYPE_SET_INTERFACES_CONFIG & 0x00FF);
    SetupPacket.Value = SIUMode;
    SetupPacket.Index = InterfaceNumber;
    SetupPacket.Length = SET_INTERFACES_CONFIG_LENGTH;

    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, NULL, 0, NULL, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Num of Data to receive: %d \nActual data transferred: %d.\n", sizeof(*SIUMode), cbSent);


done:
    return bResult;
}

BOOL GetSIUMode(WINUSB_INTERFACE_HANDLE hDeviceHandle, SIUInterfaces_e InterfaceNumber, UINT8* SIUMode)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
	ULONG cbSent = 0;
	
    //Create the setup packet
    SetupPacket.RequestType = (REQUEST_TYPE_GET_INTERFACES_CONFIG & 0xFF00) >> 8;
    SetupPacket.Request = (REQUEST_TYPE_GET_INTERFACES_CONFIG & 0x00FF);
    SetupPacket.Value = GET_INTERFACES_CONFIG_VALUE;
    SetupPacket.Index = InterfaceNumber;
    SetupPacket.Length = GET_INTERFACES_CONFIG_LENGTH;

    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, NULL, 0, NULL, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Num of Data to receive: %d \nActual data transferred: %d.\n", sizeof(*SIUMode), cbSent);


done:
    return bResult;
}