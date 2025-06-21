#include "pch.h"
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <openssl/hmac.h>
#include <cstdio>

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

std::string g_expected = "5c0cd63bdd0d359155eb503e42ba41f240193d7697be2ea7191d699545c0d154";

std::string CalcHMAC(const std::string& data, const std::string& key)
{
    unsigned char* result;
    static char hex[65] = { 0 };

    result = HMAC(EVP_sha256(),
        key.c_str(), key.length(),
        (const unsigned char*)data.c_str(), data.length(),
        NULL, NULL);

    for (int i = 0; i < 32; i++)
        sprintf_s(&hex[i * 2], 3, "%02x", (unsigned int)result[i]);

    return std::string(hex);
}

std::vector<std::string> LoadVppNamesFromManualJson()
{
    const char* configPath = "C:\\install\\vppList.json";
    std::ifstream file(configPath);
    std::vector<std::string> names;

    if (!file.is_open()) return names;

    std::string line;
    while (std::getline(file, line))
    {
        size_t colon = line.find(":");
        if (colon == std::string::npos) continue;

        size_t quote1 = line.find('"', colon);
        size_t quote2 = line.find('"', quote1 + 1);

        if (quote1 != std::string::npos && quote2 != std::string::npos && quote2 > quote1)
        {
            std::string value = line.substr(quote1 + 1, quote2 - quote1 - 1);
            if (!value.empty())
                names.push_back(value);
        }
    }
    return names;
}

std::string BuildHiddenKey(const std::vector<std::string>& vppNames)
{
    std::ostringstream oss;
    for (const auto& name : vppNames)
    {
        oss << name << "|";
    }
    std::string joined = oss.str();
    return CalcHMAC(joined, "UltraSecretKey@456");
}

bool CheckCalibrationDllPresent()
{
    HMODULE h = GetModuleHandleA("createDll.dll");
    return h != NULL;
}

void SaveGeneratedSignature(const std::string& sig)
{
    std::ofstream out("C:\\install\\generated_signature.txt");
    if (out.is_open())
    {
        out << sig;
        out.close();
    }
}

extern "C" __declspec(dllexport)
int VerifyRecipeSignature(const char* toolblockSignature)
{
    std::string inputSignature = toolblockSignature ? toolblockSignature : "";

    // 1단계: ToolBlock의 입력값과 고정 시드 서명 비교
    std::string expected = CalcHMAC("aaaFgf$", "UltraSecretKey@456");
    if (inputSignature != expected)
    {
        std::ofstream out("C:\\install\\generated_signature.txt");
        if (out.is_open())
        {
            out << expected << "||" << inputSignature;
            out.close();
        }
        return -3; // 입력된 signature가 올바르지 않음
    }

    // 2단계: Calibration DLL이 메모리에 존재하는지 확인
    if (!CheckCalibrationDllPresent())
        return -2;

    // 3단계: JSON에서 vpp 목록으로 HiddenKey 생성 및 검증
    std::vector<std::string> vppNames = LoadVppNamesFromManualJson();
    if (vppNames.empty()) return -1;

    std::string hiddenKey = BuildHiddenKey(vppNames);
    std::string generatedSig = CalcHMAC("threshold_seed", hiddenKey);
    SaveGeneratedSignature(generatedSig);

    if(generatedSig != g_expected)
    {
        return -1;
    }
    return 0;
}