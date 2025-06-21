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

    void SetData(double value);  // ������ ����
    double GetData() const;      // ������ ��������
    double SquareData();         // �������� ���� ���

private:
    double m_data;
};

// C ��Ÿ�� �������̽�
extern "C" {
    MYDLL_API void* CreateMyMfcClass();
    MYDLL_API void DeleteMyMfcClass(void* pInstance);
    MYDLL_API void SetMyMfcClassData(void* pInstance, double value);
    MYDLL_API double GetMyMfcClassData(void* pInstance);
    MYDLL_API double SquareMyMfcClassData(void* pInstance);
}
