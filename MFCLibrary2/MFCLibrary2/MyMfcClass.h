#pragma once

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

#include <afx.h>
#include <string>

class MYDLL_API CMyMfcClass {
public:
    CMyMfcClass();
    ~CMyMfcClass();

    void SetData(double value);  // 데이터 설정
    double GetData() const;      // 데이터 가져오기
    double SquareData();         // 데이터의 제곱 계산

private:
    double m_data;
};

// C 스타일 인터페이스
extern "C" {
    MYDLL_API void* CreateMyMfcClass();
    MYDLL_API void DeleteMyMfcClass(void* pInstance);
    MYDLL_API void SetMyMfcClassData(void* pInstance, double value);
    MYDLL_API double GetMyMfcClassData(void* pInstance);
    MYDLL_API double SquareMyMfcClassData(void* pInstance);
}
