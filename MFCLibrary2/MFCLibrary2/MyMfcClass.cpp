#include "pch.h"
#include "MyMfcClass.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iphlpapi.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <fstream>
// CMyMfcClass 멤버 함수 구현
CMyMfcClass::CMyMfcClass() : m_data(0.0) {}

CMyMfcClass::~CMyMfcClass() {}

void CMyMfcClass::SetData(double value) {
    m_data = value;
}

double CMyMfcClass::GetData() const {
    return m_data;
}

double CMyMfcClass::SquareData() {
    return m_data * m_data;
}
void WriteLog(const std::string& message) {
    std::ofstream log("C:\\temp\\hwid_log14456.txt", std::ios::app);
    if (log.is_open()) {
        log << message << std::endl;
    }
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
std::string TryWmiProperty(const wchar_t* className, const wchar_t* propName) {
    std::string result;
    WriteLog("Start TryWmiProperty: " + std::string(_bstr_t(className)) + "." + std::string(_bstr_t(propName)));

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        WriteLog("CoInitializeEx FAILED: " + std::to_string(hr));
        return result;
    }

    // 보안 초기화는 무시해도 OK
    CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);

    IWbemLocator* locator = nullptr;
    IWbemServices* services = nullptr;

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&locator);
    if (FAILED(hr)) {
        WriteLog("CoCreateInstance failed");
        return result;
    }

    hr = locator->ConnectServer(BSTR(L"ROOT\\CIMV2"),
        NULL, NULL, 0, NULL, 0, 0, &services);
    locator->Release();
    if (FAILED(hr)) {
        WriteLog("ConnectServer failed");
        return result;
    }

    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE);

    IEnumWbemClassObject* enumObj = NULL;
    std::wstring query = L"SELECT ";
    query += propName;
    query += L" FROM ";
    query += className;
    WriteLog("WQL: " + std::string(_bstr_t(query.c_str())));

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
std::string GetProcessorId() {
    std::string result = TryWmiProperty(L"Win32_ComputerSystemProduct", L"UUID");

    if (result.empty() || result == "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF")
        result = "UNKNOWN_CPU";

    return result;
}
std::string GetBoardId() {
    std::string result = TryWmiProperty(L"Win32_BaseBoard", L"SerialNumber");
    if (result.empty() || result == "To Be Filled By O.E.M.") {
        result = TryWmiProperty(L"Win32_BIOS", L"SerialNumber");
    }
    if (result.empty()) {
        result = TryWmiProperty(L"Win32_ComputerSystemProduct", L"IdentifyingNumber");
    }

    if (result.empty())
        result = "UNKNOWN_BOARD";

    return result;
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
bool IsHWIDAllowed(const std::string& hwid) {
    const std::vector<std::string> allowedHWIDs = {
        "bfebfbff000306a9",
        "bfebfbff000406e3",
        "bfebfbff000a0652",
        "d68747541444d416369746e65"
    };

    for (const auto& allowed : allowedHWIDs) {
        if (_stricmp(hwid.c_str(), allowed.c_str()) == 0)
            return true;
    }
    return false;
}
// C 스타일 함수 구현
extern "C" {

    void* CreateMyMfcClass() 
    {
        std::ofstream log("C:\\log\\hwid_log.txt", std::ios::app); // append mode
        if (log.is_open()) {
            log << "cpu1:"<<GetCPUID()<<"||cpu2:"<< GetProcessorId()<<"||board:"<< GetBoardId()<<"||mac:"<< GetMacAddress()<<std::endl;
        }
        std::string hwid = GetCPUID()+ GetProcessorId()+ GetBoardId()+ GetMacAddress();

        if (!IsHWIDAllowed(hwid)) {
            return false;
        }
        return new CMyMfcClass();
    }

    void DeleteMyMfcClass(void* pInstance) {
        if (pInstance) {
            delete static_cast<CMyMfcClass*>(pInstance);
        }
    }

    void SetMyMfcClassData(void* pInstance, double value) {
        if (pInstance) {
            static_cast<CMyMfcClass*>(pInstance)->SetData(value);
        }
    }

    double GetMyMfcClassData(void* pInstance) {
        if (pInstance) {
            return static_cast<CMyMfcClass*>(pInstance)->GetData();
        }
        return 0.0;
    }

    double SquareMyMfcClassData(void* pInstance) {
        if (pInstance) {
            return static_cast<CMyMfcClass*>(pInstance)->SquareData();
        }
        return 0.0;
    }

}
typedef void(__stdcall* UiLogCallback)(const char*);
UiLogCallback g_uiLogger = nullptr;

extern "C" __declspec(dllexport)
void RegisterUiLogger(UiLogCallback cb) {
    g_uiLogger = cb;
}

void UiLog(const std::string& msg) {
    if (g_uiLogger) g_uiLogger(msg.c_str());
}

extern "C" __declspec(dllexport)
void ReportNanovisionTime() {
    time_t now = time(0);
    struct tm t;
    localtime_s(&t, &now);

    char buffer[16];
    strftime(buffer, sizeof(buffer), "%m/%d %H:%M:%S", &t);

    std::stringstream ss;
    ss << "Nanovision - [" << buffer << "]";

    UiLog(ss.str());
}
