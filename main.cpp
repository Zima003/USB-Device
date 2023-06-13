/*#include <tchar.h>
#include <strsafe.h>
#include <windows.h>
#include <winusb.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <Usbiodef.h>

#include "OLED_Driver.h"
#include "ASCII_Font.h"
*/

#include "HAL_device.h"


//#define GUID_DEVINTERFACE_USBApplication1 GUID_CLASS_USB_DEVICE
#define GUID_DEVINTERFACE_USBApplication1 GUID_DEVINTERFACE_USB_DEVICE
//DEFINE_GUID(GUID_NULL, 0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
DEFINE_GUID(GUID_DEVCLASS_WINUSB, 0x88BAE032L, 0x5A81, 0x49f0, 0xBC, 0x3D, 0xA4, 0xFF, 0x13, 0x82, 0x16, 0xD6);

#if 0
// {C9163760-FEB6-4015-BDA8-FD85C493B4F7}
static const GUID GUID_DEVINTERFACE_USBApplication1 =
{ 0xc9163760, 0xfeb6, 0x4015, { 0xbd, 0xa8, 0xfd, 0x85, 0xc4, 0x93, 0xb4, 0xf7 } };
//{65580F4E-3970-44C2-91C3-4FBF1A9EAE75}
static const GUID GUID_DEVINTERFACE_USBApplication2 =
{ 0x65580F4E, 0x3970, 0x44C2, { 0x91, 0xc3, 0x4f, 0xbf, 0x1a, 0x9e, 0xae, 0x75 } };
#endif

#if 1
//{64080FBB - E548 - 4706 - ABB1 - DB94A3E21C1D}
static const GUID GUID_DEVINTERFACE_USB2USJG =
{ 0x64080FBB, 0xE548, 0x4706, { 0xAB, 0xB1, 0xDB, 0x94, 0xA3, 0xE2, 0x1C, 0x1D } };
//{64080FBB - E548 - 4706 - ABB1 - DB94A3E2C1D1}
static const GUID GUID_DEVINTERFACE_USBApplication1_IF1 =
{ 0x64080FBB, 0xE548, 0x4706, { 0xAB, 0xB1, 0xDB, 0x94, 0xA3, 0xE2, 0xC1, 0xD1 } };
//{B8A9B049 - B4C6 - 46B0 - B536 - 299B39152B6A}
static const GUID GUID_DEVINTERFACE_USBApplication1_IF2 =
{ 0xB8A9B049, 0xB4C6, 0x46B0, { 0xB5, 0x36, 0x29, 0x9B, 0x39, 0x15, 0x2B, 0x6A } };
#endif

//PCWSTR eee = _T("USB\\VID_1D50&PID_60A9");

typedef struct _DEVICE_DATA {
    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];
} DEVICE_DATA, * PDEVICE_DATA;

HRESULT
RetrieveDevicePath(
    _Out_bytecap_(BufLen) LPTSTR DevicePath,
    _In_                  ULONG  BufLen,
    _Out_opt_             PBOOL  FailureDeviceNotFound
)
/*++

Routine description:

    Retrieve the device path that can be used to open the WinUSB-based device.

    If multiple devices have the same device interface GUID, there is no
    guarantee of which one will be returned.

Arguments:

    DevicePath - On successful return, the path of the device (use with CreateFile).

    BufLen - The size of DevicePath's buffer, in bytes

    FailureDeviceNotFound - TRUE when failure is returned due to no devices
        found with the correct device interface (device not connected, driver
        not installed, or device is disabled in Device Manager); FALSE
        otherwise.

Return value:

    HRESULT

--*/
{
    BOOL                             bResult = FALSE;
    HDEVINFO                         deviceInfo;
    SP_DEVICE_INTERFACE_DATA         interfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
    SP_DEVINFO_DATA                  deviceData;
    PSP_DEVINFO_LIST_DETAIL_DATA     devDetailData = NULL;
    ULONG                            length;
    ULONG                            requiredLength = 0;
    HRESULT                          hr;

    if (NULL != FailureDeviceNotFound) {

        *FailureDeviceNotFound = FALSE;
    }

#define FFF_DEVICEINTERFACE_

#ifdef FFF_DEVICEINTERFACE
    //
    // Enumerate all devices exposing the interface
    //
    deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB2USJG,
        NULL,
        //_T("USB\\COMPOSITE"),
        //_T("USB\\VID_1D50&PID_60A9\\6&14D6642B&0&3"),
        //_T("USB\\VID_1D50&PID_60A9&MI_00\\7&74286E4&0&0002"),
        //_T("USB\\VID_1D50&PID_60A9&MI_00"),
        //_T("USB\\VID_FFFF&PID_FFFF&MI_00"),
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
#else
    //
    // Enumerate all devices exposing the class
    //
    deviceInfo = SetupDiGetClassDevs(/*&GUID_DEVINTERFACE_USBApplication1*/&GUID_DEVCLASS_WINUSB/*&GUID_DEVCLASS_USB*/,
        //NULL,
        //_T("USB\\COMPOSITE"),
        //_T("USB\\VID_1D50&PID_60A9\\6&14D6642B&0&3"),
        //_T("USB\\VID_1D50&PID_60A9&MI_00\\7&74286E4&0&0002"),
        //_T("USB\\VID_1D50&PID_60A9&MI_00"),
        _T("USB\\VID_FFFF&PID_FFFF&MI_02"),
        NULL,
        DIGCF_PRESENT);
#endif

    if (deviceInfo == INVALID_HANDLE_VALUE) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

#ifdef FFF_DEVICEINTERFACE
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //
    // Get the first interface (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInterfaces(deviceInfo,
        NULL,
        &GUID_DEVINTERFACE_USB2USJG,
        0,
        &interfaceData);

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }
#else
    deviceData.cbSize = sizeof(SP_DEVINFO_DATA);

    //
    // Get the first device (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInfo(deviceInfo, 0, &deviceData);

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    TCHAR* ddd;

    bResult = SetupDiGetDeviceInstanceId(deviceInfo, &deviceData, NULL, 0, &requiredLength);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    ddd = (PTCHAR)LocalAlloc(LMEM_FIXED, requiredLength * sizeof(*ddd));
    length = requiredLength;

    if (NULL == ddd)
    {
        hr = E_OUTOFMEMORY;
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    bResult = SetupDiGetDeviceInstanceId(deviceInfo, &deviceData, ddd, length, &requiredLength);

#define MAX_GUID_STRING_LENGTH	40

    DWORD size, reg_type;
    HKEY key;
    TCHAR guid_string[MAX_GUID_STRING_LENGTH];
    LONG s;

    key = SetupDiOpenDevRegKey(deviceInfo, &deviceData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
    if (key == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }
    // Look for both DeviceInterfaceGUIDs *and* DeviceInterfaceGUID, in that order
    size = sizeof(guid_string);
    s = RegQueryValueEx(key, _T("DeviceInterfaceGUIDs"), NULL, &reg_type,
        (LPBYTE)guid_string, &size);
    if (s == ERROR_FILE_NOT_FOUND)
        s = RegQueryValueEx(key, _T("DeviceInterfaceGUID"), NULL, &reg_type,
            (LPBYTE)guid_string, &size);
    RegCloseKey(key);

    GUID if_guid;

    if (IIDFromString(guid_string, &if_guid) != 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    SetupDiDestroyDeviceInfoList(deviceInfo);

    //
    // Enumerate all devices exposing the interface
    //
    deviceInfo = SetupDiGetClassDevs(&if_guid,
        ddd,
        //_T("USB\\COMPOSITE"),
        //_T("USB\\VID_1D50&PID_60A9\\6&14D6642B&0&3"),
        //_T("USB\\VID_1D50&PID_60A9&MI_00\\7&74286E4&0&0002"),
        //_T("USB\\VID_1D50&PID_60A9&MI_00"),
        //_T("USB\\VID_FFFF&PID_FFFF&MI_00"),
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    LocalFree(ddd);

    if (deviceInfo == INVALID_HANDLE_VALUE) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    deviceData.cbSize = sizeof(SP_DEVINFO_DATA);

    //
    // Get the first device (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInfo(deviceInfo, 0, &deviceData);

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //
    // Get the first interface (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInterfaces(deviceInfo,
        &deviceData,
        &if_guid /*&GUID_DEVINTERFACE_USB2USJG*/,
        0,
        &interfaceData); // Добавить выбор интерфейса для этой функции на уровне выше

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

#if 0

    TCHAR *ddd;

    bResult = SetupDiGetDeviceInstanceId(deviceInfo, &deviceData, NULL, 0, &requiredLength);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    ddd = (PTCHAR) LocalAlloc(LMEM_FIXED, requiredLength*sizeof(*ddd));
    length = requiredLength;

    if (NULL == ddd)
    {
        hr = E_OUTOFMEMORY;
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    bResult = SetupDiGetDeviceInstanceId(deviceInfo, &deviceData, ddd, length, &requiredLength);

    //
    // Give path to the caller. SetupDiGetDeviceInstanceId ensured
    // DevicePath is NULL-terminated.
    //
    hr = StringCbCopy(DevicePath, BufLen, ddd);

    LocalFree(ddd);

#endif


#endif

    //
    // Get the size of the path string
    // We expect to get a failure with insufficient buffer
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
        &interfaceData,
        NULL,
        0,
        &requiredLength,
        NULL);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
        LocalAlloc(LMEM_FIXED, requiredLength);

    if (NULL == detailData)
    {
        hr = E_OUTOFMEMORY;
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    length = requiredLength;

    //
    // Get the interface's path string
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
        &interfaceData,
        detailData,
        length,
        &requiredLength,
        NULL);

    if (FALSE == bResult)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        LocalFree(detailData);
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Give path to the caller. SetupDiGetDeviceInterfaceDetail ensured
    // DevicePath is NULL-terminated.
    //
    hr = StringCbCopy(DevicePath,
        BufLen,
        detailData->DevicePath);

    LocalFree(detailData);

    SetupDiDestroyDeviceInfoList(deviceInfo);

    return hr;
}

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
)
/*++

Routine description:

    Open all needed handles to interact with the device.

    If the device has multiple USB interfaces, this function grants access to
    only the first interface.

    If multiple devices have the same device interface GUID, there is no
    guarantee of which one will be returned.

Arguments:

    DeviceData - Struct filled in by this function. The caller should use the
        WinusbHandle to interact with the device, and must pass the struct to
        CloseDevice when finished.

    FailureDeviceNotFound - TRUE when failure is returned due to no devices
        found with the correct device interface (device not connected, driver
        not installed, or device is disabled in Device Manager); FALSE
        otherwise.

Return value:

    HRESULT

--*/
{
    HRESULT hr = S_OK;
    BOOL    bResult;

    DeviceData->HandlesOpen = FALSE;

    hr = RetrieveDevicePath(DeviceData->DevicePath,
        sizeof(DeviceData->DevicePath),
        FailureDeviceNotFound);

    if (FAILED(hr)) {

        return hr;
    }

    DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);

    if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    bResult = WinUsb_Initialize(DeviceData->DeviceHandle, &DeviceData->WinusbHandle);

    if (FALSE == bResult) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(DeviceData->DeviceHandle);
        return hr;
    }

    DeviceData->HandlesOpen = TRUE;
    return hr;
}

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
)
/*++

Routine description:

    Perform required cleanup when the device is no longer needed.

    If OpenDevice failed, do nothing.

Arguments:

    DeviceData - Struct filled in by OpenDevice

Return value:

    None

--*/
{
    if (FALSE == DeviceData->HandlesOpen) {

        //
        // Called on an uninitialized DeviceData
        //
        return;
    }

    WinUsb_Free(DeviceData->WinusbHandle);
    CloseHandle(DeviceData->DeviceHandle);
    DeviceData->HandlesOpen = FALSE;

    return;
}

BOOL SendDatatoDefaultEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;


    UCHAR bars = 0;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    ULONG cbSent = 0;

    //Set bits to light alternate bars
    for (short i = 0; i < 7; i += 2)
    {
        bars += 1 << i;
    }

    //Create the setup packet
    SetupPacket.RequestType = 0;
    SetupPacket.Request = 0xD8;
    SetupPacket.Value = 0;
    SetupPacket.Index = 0;
    SetupPacket.Length = sizeof(UCHAR);

    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, &bars, sizeof(UCHAR), &cbSent, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Data sent: %d \nActual data transferred: %d.\n", sizeof(bars), cbSent);


done:
    return bResult;

}

BOOL SetSIUMode(WINUSB_INTERFACE_HANDLE hDeviceHandle, UINT8 SIUMode)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

    //Create the setup packet
    SetupPacket.RequestType = 0x41;
    SetupPacket.Request = 0x00;
    SetupPacket.Value = SIUMode;
    SetupPacket.Index = 0;
    SetupPacket.Length = 0;

    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, NULL, 0, NULL, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Num of Data to receive: %d \nActual data transferred: %d.\n", sizeof(*SIUMode), cbSent);


done:
    return bResult;

}

BOOL GetSIUMode(WINUSB_INTERFACE_HANDLE hDeviceHandle, UINT8* SIUMode)
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
    SetupPacket.RequestType = 0xc1;
    SetupPacket.Request = 0x01;
    SetupPacket.Value = 0;
    SetupPacket.Index = 0;
    SetupPacket.Length = sizeof(UINT8);

    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, SIUMode, sizeof(UINT8), &cbSent, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Num of Data to receive: %d \nActual data transferred: %d.\n", sizeof(*SIUMode), cbSent);


done:
    return bResult;

}

BOOL SetSIUDIV(WINUSB_INTERFACE_HANDLE hDeviceHandle, UINT16 IDIV, UINT8 FDIV)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

    //Create the setup packet
    SetupPacket.RequestType = 0x41;
    SetupPacket.Request = 0x02;
    SetupPacket.Value = IDIV;
    SetupPacket.Index = FDIV<<13;
    SetupPacket.Length = 0;

    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, NULL, 0, NULL, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Num of Data to receive: %d \nActual data transferred: %d.\n", sizeof(*SIUMode), cbSent);


done:
    return bResult;

}

#define Write_Command1(c) 0x80, 0x20, 0x3b, 0x11, 0x00, 0x00, c
#define Write_Data1(d) 0x80, 0x30, 0x3b, 0x11, 0x00, 0x00, d

#define Write_CommandN(n) 0x80, 0x20, 0x3b, 0x11, (n-1)&0xff, ((n-1)>>8)&0xff
#define Write_DataN(n) 0x80, 0x30, 0x3b, 0x11, (n-1)&0xff, ((n-1)>>8)&0xff

#define ReadData1 0x21, 0x00, 0x00

BOOL InitSJG(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR pID, ULONG* pcbWritten)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE || !pcbWritten)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    static const UINT8 szBuffer[] = {
        0x86, 2, 10,
        0x80, 0x08, 0x3b,
        0x8f, 0x00, 0x10,
        0x80, 0x28, 0x3b,
        Write_CommandN(1),
        0xfd,    // command lock
        Write_DataN(1),
        0x12,
        Write_CommandN(1),
        0xfd,    // command lock
        Write_DataN(1),
        0xB1,

        Write_CommandN(2),
        0xae,    // display off
        0xa4,    // Normal Display mode

        Write_CommandN(1),
        0x15,    //set column address
        Write_DataN(2),
        0x00,    //column address start 00
        0x7f,    //column address end 95
        Write_CommandN(1),
        0x75,	//set row address
        Write_DataN(2),
        0x00,   //row address start 00
        0x7f,   //row address end 63	

        Write_Command1(0xB3),
        Write_Data1(0xF1),

        Write_Command1(0xCA),
        Write_Data1(0x7F),

        Write_Command1(0xa0),  //set re-map & data format
        Write_Data1(0x74),     //Horizontal address increment

        Write_Command1(0xa1),  //set display start line
        Write_Data1(0x00),     //start 00 line

        Write_Command1(0xa2),  //set display offset
        Write_Data1(0x00),

        Write_CommandN(2),
        0xAB,
        0x01,

        Write_Command1(0xB4),
        Write_DataN(3),
        0xA0,
        0xB5,
        0x55,

        Write_Command1(0xC1),
        Write_DataN(3),
        (0xC8),
        (0x80),
        (0xC0),

        Write_Command1(0xC7),
        Write_Data1(0x0F),

        Write_Command1(0xB1),
        Write_Data1(0x32),

        Write_Command1(0xB2),
        Write_DataN(3),
        (0xA4),
        (0x00),
        (0x00),

        Write_Command1(0xBB),
        Write_Data1(0x17),

        Write_Command1(0xB6),
        Write_Data1(0x01),

        Write_Command1(0xBE),
        Write_Data1(0x05),

        Write_Command1(0xA6),

        //Clear_Screen(),
        Write_Command1(0xaf),     //display on

        //Write_Command1(SSD1351_CMD_SETCOLUMN),
        //Write_DataN(2),
        //(16),	//X start 
        //(16),	//X end 
        //Write_Command1(SSD1351_CMD_SETROW),
        //Write_DataN(2),
        //(16),	//Y start 
        //(16 + 7),	//Y end 
        //Write_Command1(SSD1351_CMD_WRITERAM),
        //Write_DataN(2),
        //0xff, 0xff,

        0x80, 0x38, 0x3b
    };

    ULONG cbSize = sizeof(szBuffer);
    ULONG cbSent = 0;

    bResult = WinUsb_WritePipe(hDeviceHandle, pID, (PUCHAR)szBuffer, cbSize, &cbSent, 0);
    if (!bResult)
    {
        goto done;
    }

    _tprintf(_T("Wrote %d byte(s) to pipe %d.\nActual data transferred: %d.\n"), cbSize, pID, cbSent);
    *pcbWritten = cbSent;


done:
    return bResult;

}
BOOL SetLineCoding(WINUSB_INTERFACE_HANDLE hDeviceHandle,UCHAR IntefaceNum ,int BaudRate, UCHAR StopBit, UCHAR Parity, UCHAR NumDataBits)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;


    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

    //Create the setup packet
    SetupPacket.RequestType = 0x21;
    SetupPacket.Request = 0x20;
    SetupPacket.Value = 0;
    SetupPacket.Index = IntefaceNum;
    SetupPacket.Length = 7;
    UCHAR DATA[7];
    ULONG cbSent_UART = 0;
    DATA[0] = (UCHAR)((BaudRate & 0xFF));
    DATA[1] = (UCHAR)((BaudRate >> 8) & 0xFF);
    DATA[2] = (UCHAR)((BaudRate >> 16) & 0xFF);
    DATA[3] = (UCHAR)((BaudRate >> 24) & 0xFF);
    DATA[4] = (StopBit);
    DATA[5] = (Parity);
    DATA[6] = (NumDataBits);
    bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, &DATA[0], 7, &cbSent_UART, 0);
    if (!bResult)
    {
        goto done;
    }

    //printf("Num of Data to receive: %d \nActual data transferred: %d.\n", sizeof(*SIUMode), cbSent);


done:
    return bResult;

}
void Demo_SPI(WINUSB_INTERFACE_HANDLE hInterface) {
    BOOL bError, bResult;
    UINT8 SIUMode;
    WINUSB_PIPE_INFORMATION pipeO;
    WINUSB_INTERFACE_HANDLE hDataInterface;
    ULONG cbWritten;

    SetSIUMode(hInterface, 0);
    SetSIUMode(hInterface, 3);
    if (GetSIUMode(hInterface, &SIUMode)) {
        printf("Current SIU mode: %d \n", SIUMode);
    }

    if (!WinUsb_GetAssociatedInterface(hInterface, 0, &hDataInterface)) return;

    bResult = WinUsb_QueryPipe(hDataInterface, 0, 1, &pipeO);
    if (bResult) {
        while(true)
        {
            InitSJG(hDataInterface, pipeO.PipeId, &cbWritten);
        }
       // ClearDisplayRAM(hDataInterface, pipeO.PipeId, &cbWritten);
        WinUsb_Free(hDataInterface);
    }
}

void Demo_GPIO(WINUSB_INTERFACE_HANDLE hInterface) {
    BOOL bError, bResult;
    UINT8 SIUMode;
    WINUSB_PIPE_INFORMATION pipeO, pipeI;
    WINUSB_INTERFACE_HANDLE hDataInterface;
    ULONG cbWritten;

    SetSIUMode(hInterface, 0);
    SetSIUMode(hInterface, 3);
    if (GetSIUMode(hInterface, &SIUMode)) {
        printf("Current SIU mode: %d \n", SIUMode);
    }

    if (!WinUsb_GetAssociatedInterface(hInterface, 0, &hDataInterface)) return;

    static const UINT8 szBuffer[] = {
        0x86, 2, 0,
        0x80, 0x00, 0xFF,
        0x82, 0x05, 0x0F
    };
 
    UINT8 ToSJG[10], FromSJG[10];

    bResult = WinUsb_QueryPipe(hDataInterface, 0, 1, &pipeO);
    bResult &= WinUsb_QueryPipe(hDataInterface, 0, 0, &pipeI);
    if (bResult) {

        ULONG cbSize = sizeof(szBuffer);
        ULONG cbSent = 0;
        UCHAR pID;

        pID = pipeO.PipeId;
        bResult = WinUsb_WritePipe(hDataInterface, pID, (PUCHAR)szBuffer, cbSize, &cbSent, 0);


        _tprintf(_T("Wrote %d byte(s) to pipe %d.\nActual data transferred: %d.\n"), cbSize, pID, cbSent);

        ToSJG[0] = 0x80; ToSJG[1] = 0xAA; ToSJG[2] = 0xFF;
        ToSJG[3] = 0x82; ToSJG[4] = 0x09; ToSJG[5] = 0x00;
        ToSJG[6] = 0x81; ToSJG[7] = 0x83; ToSJG[8] = 0x87;
        while (true)
        {
            bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, &ToSJG[0], 3, &cbSent, 0);
            Sleep(200);
            printf("Ok");
        }
    }
        
}

void test_SPI(WINUSB_INTERFACE_HANDLE hInterface)
{
    BOOL bError, bResult;
    UINT8 SIUMode;
    WINUSB_PIPE_INFORMATION pipeO, pipeI;
    WINUSB_INTERFACE_HANDLE hDataInterface;
    ULONG cbWritten;

    SetSIUMode(hInterface, 0);
    SetSIUMode(hInterface, 3);
    if (GetSIUMode(hInterface, &SIUMode)) {
        printf("Current SIU mode: %d \n", SIUMode);
    }

    if (!WinUsb_GetAssociatedInterface(hInterface, 0, &hDataInterface)) return;

    static const UINT8 szBuffer[] = {
    0x86, 2, 10, 
    };

    bResult = WinUsb_QueryPipe(hDataInterface, 0, 1, &pipeO);
    bResult &= WinUsb_QueryPipe(hDataInterface, 0, 0, &pipeI);
    _tprintf(_T("Input to pipe ID %d.\n"), pipeI.PipeId);
    _tprintf(_T("Output to pipe ID %d.\n"), pipeO.PipeId);

    ULONG cbSize = sizeof(szBuffer);
    ULONG cbSent = 0;
    ULONG cbRead = 0;

    bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, (PUCHAR)szBuffer, cbSize, &cbSent, 0);
    if (bResult)
    {
        uint8_t cmd_buf[] = { 
        0x8f, 0xff, 0x00,

        /*Write_Command1(0x1),
        Write_Command1(0x2),
        Write_Command1(0x3),
        Write_Command1(0x4),
        Write_Command1(0x5),
        Write_Command1(0x6),
        Write_Command1(0x7),*/
        Write_Command1(0x1),
        Write_Command1(0x2),
        Write_Command1(0x3),
        Write_Command1(0x4),
        Write_Command1(0x5),

       

        0x80, 0x38, 0x3b}; 
        ULONG cmd_size = sizeof(cmd_buf);

        uint8_t read_buf[100];
        while (true)
        {
            bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, (PUCHAR)cmd_buf, cmd_size, &cbSent, 0);
            Sleep(200);
            _tprintf(_T("Wrote %d byte(s) to pipe %d.\nActual data transferred: %d.\n___________"), cmd_size, pipeO.PipeId, cbSent);
            
        }
    }
}

void Test_UART(WINUSB_INTERFACE_HANDLE hInterface, uint32_t DTERate, uint8_t  CharFormat, uint8_t  ParityType, uint8_t  DataBits, ULONG data)
{
    BOOL bError, bResult = true;
    UINT8 SIUMode;
    WINUSB_PIPE_INFORMATION pipeO, pipeI;
    WINUSB_INTERFACE_HANDLE hDataInterface = nullptr;
    ULONG cbWritten;
    ULONG cbSent = 0;
    LineCoding_s UartLineCoding;
    UartLineCoding.DTERate = DTERate;
    UartLineCoding.CharFormat = CharFormat;
    UartLineCoding.ParityType = ParityType;
    UartLineCoding.DataBits = DataBits;


    SIUInterfaces_e Interface = SIU_INTERFACE_0;
    
#if 0
    printf("Enter UART parameters: \n");
    printf("Baudrate: \n");
    scanf_s("%d", &(UartLineCoding.DTERate));
    printf("Stop bits qty (0 - 1stop bit, 1 - 1.5 stop bit, 2 - 2 stop bits): \n");
    scanf_s("%d", &(UartLineCoding.CharFormat));
    printf("Parity type (0 - non parity bit, 1 - odd, 2 - even, 3 - mark, 4 - space): \n");
    scanf_s("%d", &(UartLineCoding.ParityType));
    printf("Data bits (5,6,7,8,16): \n");
    scanf_s("%d", &(UartLineCoding.DataBits));
#endif

#if 0
    printf("Enter Interface (0, 2): \n");
    scanf_s("%d", &(Interface));
#endif
    /// Настройки для UART не выставляются если книверсальный блок интерфейсов отключён
    //bResult = SetSIUMode(hInterface, SIU_INTERFACE_0, SET_INTERFACES_CONFIG_VALUE_DISABLE);
    if (bResult)
    {
        printf("\n OK");
    }

    bResult = SetSIUMode(hInterface, Interface, SET_INTERFACES_CONFIG_VALUE_UART);
    if (GetSIUMode(hInterface, Interface, &SIUMode)) {
        printf("Current SIU mode: %d \n", SIUMode);
    }
    PrintUartLineCoding(UartLineCoding);
    bResult = SetUartLineCoding(hInterface, Interface, &UartLineCoding);
    if (bResult)
    {
        printf("\n Set UART Line Coding OK");
    }
    memset(&UartLineCoding, 0x00, sizeof(UartLineCoding));

    bResult = GetUartLineCoding(hInterface, Interface, &UartLineCoding);
    if (bResult)
    {
        printf("\nGet UART Line Coding OK");
    }

    PrintUartLineCoding(UartLineCoding);

    if(Interface == SIU_INTERFACE_0)
    {
        if (!WinUsb_GetAssociatedInterface(hInterface, 0, &hDataInterface)) return;
    }
    if(Interface == SIU_INTERFACE_1)
    {
        if (!WinUsb_GetAssociatedInterface(hInterface, 0, &hDataInterface)) return;
    }

    bResult = WinUsb_QueryPipe(hDataInterface, 0, 1, &pipeO);
    bResult &= WinUsb_QueryPipe(hDataInterface, 0, 0, &pipeI);
#if 0    
    printf("\nEnter data to write: \n");
    scanf_s("%u", &(data));
#endif
    while (true)
    {
        bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, (PUCHAR)&data, 1, &cbSent, 0);
        
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    DEVICE_DATA mydd;
    HRESULT hr;
    BOOL bError, bResult;
    LineCoding_s UartLineCoding;
    UartLineCoding.DTERate = 150000;
    UartLineCoding.CharFormat = 0;
    UartLineCoding.ParityType = 1;
    UartLineCoding.DataBits = 8;
    //system("pause");
    hr = OpenDevice(&mydd, &bError);
    if (FAILED(hr)) return -1;
    if (bError) {
        return -2;
    }
    //Test_UART(mydd.WinusbHandle,24000000,0 , 1, 8, 131);
    //Demo_GPIO(mydd.WinusbHandle);
    test_SPI(mydd.WinusbHandle);
    //Demo_SPI(mydd.WinusbHandle);
    CloseDevice(&mydd);
    return 0;
}
/*
BOOL GPIO_Set( WINUSB_INTERFACE_HANDLE InterfaceHandle, UCHAR PipeID, int pin, int state)
{
    BOOL bResult;
    ULONG cbSent = 0;
    bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, &ToSJG[6], 3, &cbSent, 0);
    bResult = WinUsb_ReadPipe(hDataInterface, pipeI.PipeId, &FromSJG[0], 2, &cbSent, 0);

    if (pin < 8)
    {
        if (state)
            ToSJG[1] = FromSJG[0] | (1 << pin);//Set Pin
        else
            ToSJG[1] = FromSJG[0] & (~(1 << pin));//Reset Pin

        bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, &ToSJG[0], 3, &cbSent, 0);
        if (!bResult)
        {
            printf("fail to write Low Byte\n");
        }
    }
    else
    {
        if (state)
            ToSJG[4] = FromSJG[1] | (1 << pin);//Set Pin
        else
            ToSJG[4] = FromSJG[1] & (~(1 << pin));//Reset Pin

        bResult = WinUsb_WritePipe(hDataInterface, pipeO.PipeId, &ToSJG[3], 3, &cbSent, 0);
        if (!bResult)
        {
            printf("fail to write High Byte\n");
        }
    }



    return bResult;

}
*/


