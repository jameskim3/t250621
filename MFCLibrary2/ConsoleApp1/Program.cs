using System;
using System.Runtime.InteropServices;

class Program
{
    // DLL 함수 선언
    [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr CreateMyMfcClass();

    [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void DeleteMyMfcClass(IntPtr pInstance);

    [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void SetMyMfcClassData(IntPtr pInstance, double value);

    [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double GetMyMfcClassData(IntPtr pInstance);

    [DllImport("createDll.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern double SquareMyMfcClassData(IntPtr pInstance);

    static void Main(string[] args)
    {
        // CMyMfcClass 인스턴스 생성
        IntPtr myClass = CreateMyMfcClass();

        // 데이터 설정
        SetMyMfcClassData(myClass, 155.5);

        // 데이터 가져오기
        double value = GetMyMfcClassData(myClass);
        Console.WriteLine($"Value: {value}");

        // 데이터의 제곱 계산
        double square = SquareMyMfcClassData(myClass);
        Console.WriteLine($"Square: {square}");

        // 인스턴스 삭제
        DeleteMyMfcClass(myClass);
    }
}