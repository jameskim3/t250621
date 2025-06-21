using Cognex.VisionPro.Implementation.Internal;
using Cognex.VisionPro;
using Cognex.VisionPro.PMAlign;
using Cognex.VisionPro.ToolBlock;
using System;
using System.Security.Cryptography;
using System.Text;
using System.Runtime.InteropServices;

namespace vppEncrypt
{

    public class HiddenSignatureInserter
    {
        //[DllImport("vppEncryptDll.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [DllImport(@"C:\install\vppEncryptDll.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int VerifyRecipeSignature(string value, string signature);
        public static extern int VerifyRecipeSignature(string signature);
        static string SecretKey = "UltraSecretKey@456";

        public void AddHiddenSignatureInput(string vppPath)
        {
            var tb = (CogToolBlock)CogSerializer.LoadObjectFromFile(vppPath);

            // 예시 파라미터 (실제로는 더 복합적으로 가능)
            //double threshold = ((CogPMAlignTool)tb.Tools["Align1"]).RunParams.AcceptThreshold;
            string signature = GenerateSignature("aaaFgf$", SecretKey);

            // 필드 이름은 의도를 숨기기 위해 난독화
            string hiddenKeyName = "inp_sigx3h"; // 일반 입력처럼 보이도록
            if (!tb.Inputs.Contains(hiddenKeyName))
                tb.Inputs.Add(new CogToolBlockTerminal(hiddenKeyName, typeof(string)));

            tb.Inputs[hiddenKeyName].Value = signature;

            CogSerializer.SaveObjectToFile(tb, vppPath);
        }

        static string GenerateSignature(string data, string key)
        {
            using (var hmac = new HMACSHA256(Encoding.UTF8.GetBytes(key)))
            {
                var hash = hmac.ComputeHash(Encoding.UTF8.GetBytes(data));
                return BitConverter.ToString(hash).Replace("-", "").ToLower();
            }
        }
        public int verify(string vppPath)
        {
            string fieldName = "inp_sigx3h";  // 암호 서명이 삽입된 Inputs 이름

            // 1. ToolBlock 로드
            var tb = (CogToolBlock)CogSerializer.LoadObjectFromFile(vppPath);

            // 3. 서명 필드 확인
            if (!tb.Inputs.Contains(fieldName))
            {
                return -200;
            }

            string signature = tb.Inputs[fieldName].Value as string;

            // 4. C++ DLL에 검증 요청
            int result = VerifyRecipeSignature(signature);
            return result;
        }
    }
}