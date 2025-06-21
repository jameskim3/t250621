#include <iostream>
#include <string>
#include <sstream>

#include <Windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iphlpapi.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <fstream>
#include <string>
#include <iostream>
#include <intrin.h>
void WriteLog(const std::string& message) {
    std::ofstream log("C:\\temp\\hwid_log123.txt", std::ios::app);
    if (log.is_open()) {
        log << message << std::endl;
    }
}
std::string TryWmiProperty(const wchar_t* className, const wchar_t* propName) {
    std::string result;
    WriteLog("Start TryWmiProperty: " + std::string(_bstr_t(className)) + "." + std::string(_bstr_t(propName)));

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return result;

    CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);

    IWbemLocator* locator = nullptr;
    IWbemServices* services = nullptr;
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&locator);
    if (FAILED(hr)) return result;

    hr = locator->ConnectServer(BSTR(L"ROOT\\CIMV2"),
        NULL, NULL, 0, NULL, 0, 0, &services);
    locator->Release();
    if (FAILED(hr)) return result;

    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE);

    IEnumWbemClassObject* enumObj = NULL;
    std::wstring query = L"SELECT ";
    query += propName;
    query += L" FROM ";
    query += className;

    hr = services->ExecQuery(BSTR(L"WQL"), BSTR(query.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumObj);

    if (SUCCEEDED(hr) && enumObj) {
        IWbemClassObject* obj = NULL;
        ULONG ret = 0;
        hr = enumObj->Next(WBEM_INFINITE, 1, &obj, &ret);
        if (ret) {
            VARIANT vt;
            obj->Get(propName, 0, &vt, 0, 0);
            result = _bstr_t(vt.bstrVal);
            VariantClear(&vt);
            obj->Release();
            WriteLog("WMI RESULT: " + result);
        }
        else {
            WriteLog("WMI Next() returned 0 items");
        }
        enumObj->Release();
    }
    else {
        WriteLog("ExecQuery failed");
    }

    services->Release();
    CoUninitialize();

    return result;
}

std::string GetCPUID() {
    int cpuInfo[4] = { 0 };

#ifdef _WIN32
    __cpuid(cpuInfo, 0);
#else
    __cpuid(0, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif

    std::stringstream ss;
    for (int i = 0; i < 4; ++i) {
        ss << std::hex << cpuInfo[i];
    }

    return ss.str();
}
std::string GetProcessorId() {
    std::string uuid = TryWmiProperty(L"Win32_ComputerSystemProduct", L"UUID");

    if (uuid.empty() || uuid == "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF")
        uuid = "UNKNOWN_CPU";

    WriteLog("GetProcessorId: " + uuid);
    return uuid;
}
std::string GetBoardId() {
    std::string serial = TryWmiProperty(L"Win32_BIOS", L"SerialNumber");

    if (serial.empty() || serial == "Default string" || serial == "To be filled by O.E.M.") {
        serial = TryWmiProperty(L"Win32_BaseBoard", L"SerialNumber");
    }

    if (serial.empty() || serial == "Default string")
        serial = "UNKNOWN_BOARD";

    WriteLog("GetBoardId: " + serial);
    return serial;
}
std::string GetMacAddress() {
    std::string result = "UNKNOWN_MAC";

    IP_ADAPTER_INFO adapterInfo[16]; // 최대 16개 어댑터
    DWORD bufLen = sizeof(adapterInfo);

    if (GetAdaptersInfo(adapterInfo, &bufLen) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = adapterInfo;

        while (pAdapter) {
            if (pAdapter->Type == MIB_IF_TYPE_ETHERNET &&
                pAdapter->AddressLength == 6) {
                char macStr[18];
                sprintf_s(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    pAdapter->Address[0], pAdapter->Address[1],
                    pAdapter->Address[2], pAdapter->Address[3],
                    pAdapter->Address[4], pAdapter->Address[5]);

                result = macStr;
                break; // 첫 번째 유효한 MAC만 사용
            }
            pAdapter = pAdapter->Next;
        }
    }

    return result;
}
std::string GetDiskSerialAndFileSystem()
{
    char volumeName[MAX_PATH] = { 0 };
    char fileSystemName[MAX_PATH] = { 0 };
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;

    // C:\ 드라이브를 기준으로 정보 조회
    if (GetVolumeInformationA(
        "C:\\",           // 드라이브 이름
        volumeName,       // 볼륨 이름 (보통 드라이브 레터)
        MAX_PATH,         // 볼륨 이름 최대 길이
        &serialNumber,    // 시리얼 번호
        &maxComponentLen, // 최대 파일 이름 길이
        &fileSystemFlags, // 파일 시스템 플래그
        fileSystemName,   // 파일 시스템 이름 (NTFS, FAT32, etc.)
        MAX_PATH          // 파일 시스템 이름 최대 길이
    )) {
        //// 파일 시스템 및 시리얼 번호 출력
        //std::cout << "Volume Serial Number: " << serialNumber << std::endl;
        //std::cout << "File System: " << fileSystemName << std::endl;

        // 반환된 시리얼 번호를 문자열로 변환하여 반환
        std::stringstream ss;
        ss << std::hex << serialNumber;
        return ss.str();
    }

    return "UNKNOWN_DISK"; // 실패 시 기본값 반환
}
// CPU 정보를 가져오는 함수
std::string GetProcessorId2()
{
    int cpuInfo[4] = { -1 };

    // CPUID 호출, 0은 기본 정보 요청
    __cpuid(cpuInfo, 0);

    // CPU 제조사 문자열 추출
    char manufacturer[13];
    *((int*)&manufacturer[0]) = cpuInfo[1];
    *((int*)&manufacturer[4]) = cpuInfo[3];
    *((int*)&manufacturer[8]) = cpuInfo[2];
    manufacturer[12] = '\0';  // 문자열 종료

    // 제조사 정보
    std::string cpuManufacturer = std::string(manufacturer);

    // CPU 모델 정보 가져오기
    __cpuid(cpuInfo, 0x80000002); // CPUID 호출, 0x80000002로 모델 정보 시작
    char modelInfo[16];
    *((int*)&modelInfo[0]) = cpuInfo[0];
    *((int*)&modelInfo[4]) = cpuInfo[1];
    *((int*)&modelInfo[8]) = cpuInfo[2];
    *((int*)&modelInfo[12]) = cpuInfo[3];
    modelInfo[15] = '\0';  // Null terminate

    std::string cpuModel = std::string(modelInfo);

    // 0x80000003 ~ 0x80000004까지 추가 호출하여 모델 정보 나누어 추출
    __cpuid(cpuInfo, 0x80000003);
    *((int*)&modelInfo[0]) = cpuInfo[0];
    *((int*)&modelInfo[4]) = cpuInfo[1];
    *((int*)&modelInfo[8]) = cpuInfo[2];
    *((int*)&modelInfo[12]) = cpuInfo[3];
    modelInfo[15] = '\0';
    std::string cpuModel2 = std::string(modelInfo);

    __cpuid(cpuInfo, 0x80000004);
    *((int*)&modelInfo[0]) = cpuInfo[0];
    *((int*)&modelInfo[4]) = cpuInfo[1];
    *((int*)&modelInfo[8]) = cpuInfo[2];
    *((int*)&modelInfo[12]) = cpuInfo[3];
    modelInfo[15] = '\0';
    std::string cpuModel3 = std::string(modelInfo);

    // 최종 모델 정보 반환 (이 세 부분을 결합)
    return cpuManufacturer + " " + cpuModel + cpuModel2 + cpuModel3;
}

int main() {
    std::cout << "CPU   : " << GetCPUID() << std::endl;
    std::cout << "CPU   : " << GetProcessorId2() << std::endl;
    std::cout << "CPUID  : " << GetProcessorId() << std::endl;
    std::cout << "Board : " << GetBoardId() << std::endl;
    std::cout << "HDD   : " << GetDiskSerialAndFileSystem() << std::endl;
    std::cout << "MAC   : " << GetMacAddress() << std::endl;
    getchar();
    return 0;
}
