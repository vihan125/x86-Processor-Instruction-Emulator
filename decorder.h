#pragma once
#include <list>
#include <iterator>
#include <string>
#include <map>
#include <vector>
#include "emulator.h"

enum keywords{
	AL, EAX, ES, CS, SS, DS, ONE, CL, XMM0, BND0, BAD, MM0,
	IMM8, IMM16, IMM32, REL8, REL32, PTR1632, R, RM,
	BYTE, WORD, DWORD, QWORD, FWORD, XMMWORD, BL, DL, AH, ESP, EBP, ESI,
  CH, DH, EDX, BH, EDI, EBX, ECX
};

typedef struct {
	char hasModRM;
  char size;
	char mnemonic[0xFF];
	int argument_count;
	char arguments[4];
  //char arguments_2;
  //char arguments_3;
} Instruction;

class Decorder
{
private:
  Emulator e;
  std::vector<std::string> oneInstruction;
  std::list<std::string> innerInstruction;
  std::list<unsigned char> input_bytes;

  std::list <unsigned char>::iterator it;

  std::array<std::string, 8> register_mnemonics8 = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
	std::array<std::string, 8 > register_mnemonics16 = { "ax", "cx", "dx", "bx", "ax", "cx", "dx", "bx" };
	std::array<std::string , 8> register_mnemonics32 = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };

  std::array<std::string , 8> sib_base_mnemonics = { "[eax", "[ecx", "[edx", "[ebx", "[esp", "[ebp", "[esi", "[edi" };
	std::array<std::string , 4> sib_scale_mnemonics = { "*1", "*2", "*4", "*8" };




  std::map<int,Instruction> standard_instructions{ 
    {0x0, { 1, BYTE, "add ", 2, RM, R }}, // 0
    {0x1, { 1, DWORD, "add ", 2, RM, R }}, // 1
    {0x2, { 1, BYTE, "add ", 2, R, RM }}, // 2
    {0x3, { 1, DWORD, "add ", 2, R, RM }}, // 3
    {0x4, { 0, 0, "add ", 2, AL, IMM8 }}, // 4
    {0x5, { 0, 0, "add ", 2, EAX, IMM32 }}, // 5
    {0x6, { 0, 0, "push es", 0 }}, // 6
    {0x7, { 0, 0, "pop es", 0 }}, // 7
    {0x8, { 1, BYTE, "or ", 2, RM, R }}, // 8
    {0x9, { 1, DWORD, "or ", 2, RM, R }}, // 9
    {0xA, { 1, BYTE, "or ", 2, R, RM }}, // A
    {0xB, { 1, DWORD, "or ", 2, R, RM }}, // B
    {0xC, { 0, 0, "or ", 2, AL, IMM8 }}, // C // <- edited { 0, 0, "or ", 2, AL, IMM8 }}
    {0xD, { 0, 0, "or ", 2, EAX, IMM32 }}, // D
    {0xE, { 0, 0, "push cs", 0 }}, // E
    {0xF, { 0 }}, // F - Two-byte instructions
    {0x10, { 1, BYTE, "adc ", 2, RM, R }}, // 10
    {0x11, { 1, DWORD, "adc ", 2, RM, R }}, // 11
    {0x12, { 1, BYTE, "adc ", 2, R, RM }}, // 12
    {0x13, { 1, DWORD, "adc ", 2, R, RM }}, // 13
    {0x14, { 0, 0, "adc ", 2, AL, IMM8 }}, // 14
    {0x15, { 0, 0, "adc ", 2, EAX, IMM32 }}, // 15
    {0x16, { 0, 0, "push ss", 0 }}, // 16
    {0x17, { 0, 0, "pop ss", 0 }}, // 17
    {0x18, { 1, BYTE, "sbb ", 2, RM, R }}, // 18
    {0x19, { 1, DWORD, "sbb ", 2, RM, R }}, // 19
    {0x1A, { 1, BYTE, "sbb ", 2, R, RM }}, // 1A
    {0x1B, { 1, DWORD, "sbb ", 2, R, RM }}, // 1B
    {0x1C, { 0, 0, "sbb ", 2, AL, IMM8 }}, // 1C
    {0x1D, { 0, 0, "sbb ", 2, EAX, IMM32 }}, // 1D
    {0x1E, { 0, 0, "push ds", 0 }}, // 1E
    {0x1F, { 0, 0, "pop ds", 0 }}, // 1F
    {0x20, { 1, BYTE, "and ", 2, RM, R }}, // 20
    {0x21, { 1, DWORD, "and ", 2, RM, R }}, // 21
    {0x22, { 1, BYTE, "and ", 2, R, RM }}, // 22
    {0x23, { 1, DWORD, "and ", 2, R, RM }}, // 23
    {0x24, { 0, 0, "and ", 2, AL, IMM8 }}, // 24
    {0x25, { 0, 0, "and ", 2, EAX, IMM32 }}, // 25
    {0x26, { 0, 0, "es ", 0 }}, // 26
    {0x27, { 0, 0, "daa ", 0 }}, // 27
    {0x28, { 1, BYTE, "sub ", 2, RM, R }}, // 28
    {0x29, { 1, DWORD, "sub ", 2, RM, R }}, // 29
    {0x2A, { 1, BYTE, "sub ", 2, R, RM }}, // 2A
    {0x2B, { 1, DWORD, "sub ", 2, R, RM }}, // 2B
    {0x2C, { 0, 0, "sub ", 2, AL, IMM8 }}, // 2C
    {0x2D, { 0, 0, "sub ", 2, EAX, IMM32 }}, // 2D
    {0x2E, { 0, 0, "cs ", 0 }}, // 2E
    {0x2F, { 0, 0, "das ", 0 }}, // 2F
    {0x30, { 1, BYTE, "xor ", 2, RM, R }}, // 30
    {0x31, { 1, DWORD, "xor ", 2, RM, R }}, // 31
    {0x32, { 1, BYTE, "xor ", 2, R, RM }}, // 32
    {0x33, { 1, DWORD, "xor ", 2, R, RM }}, // 33
    {0x34, { 0, 0, "xor ", 2, AL, IMM8 }}, // 34
    {0x35, { 0, 0, "xor ", 2, EAX, IMM32 }}, // 35
    {0x36, { 0, 0, "ss ", 0 }}, // 36
    {0x37, { 0, 0, "aaa ", 0 }}, // 37
    {0x38, { 1, BYTE, "cmp ", 2, RM, R }}, // 38
    {0x39, { 1, DWORD, "cmp ", 2, RM, R }}, // 39
    {0x3A, { 1, BYTE, "cmp ", 2, R, RM }}, // 3A
    {0x3B, { 1, DWORD, "cmp ", 2, R, RM }}, // 3B
    {0x3C, { 0, 0, "cmp ", 2, AL, IMM8 }}, // 3C
    {0x3D, { 0, 0, "cmp ", 2, EAX, IMM32 }}, // 3D
    {0x3E, { 0, 0, "ds ", 0 }}, // 3E
    {0x3F, { 0, 0, "aas ", 0 }}, // 3F
    {0x40, { 0, 0, "inc eax", 0 }}, // 40
    {0x41, { 0, 0, "inc ecx", 0 }}, // 41
    {0x42, { 0, 0, "inc edx", 0 }}, // 42
    {0x43, { 0, 0, "inc ebx", 0 }}, // 43
    {0x44, { 0, 0, "inc esp", 0 }}, // 44
    {0x45, { 0, 0, "inc ebp", 0 }}, // 45
    {0x46, { 0, 0, "inc esi", 0 }}, // 46
    {0x47, { 0, 0, "inc edi", 0 }}, // 47
    {0x48, { 0, 0, "dec eax", 0 }}, // 48
    {0x49, { 0, 0, "dec ecx", 0 }}, // 49
    {0x4A, { 0, 0, "dec edx", 0 }}, // 4A
    {0x4B, { 0, 0, "dec ebx", 0 }}, // 4B
    {0x4C, { 0, 0, "dec esp", 0 }}, // 4C
    {0x4D, { 0, 0, "dec ebp", 0 }}, // 4D
    {0x4E, { 0, 0, "dec esi", 0 }}, // 4E
    {0x4F, { 0, 0, "dec edi", 0 }}, // 4F

    {0x50, { 0, 0, "push eax", 0 }}, // 50
    {0x51, { 0, 0, "push ecx", 0 }}, // 51
    {0x52, { 0, 0, "push edx", 0 }}, // 52
    {0x53, { 0, 0, "push ebx", 0 }}, // 53
    {0x54, { 0, 0, "push esp", 0 }}, // 54
    {0x55, { 0, 0, "push ebp", 0 }}, // 55
    {0x56, { 0, 0, "push esi", 0 }}, // 56
    {0x57, { 0, 0, "push edi", 0 }}, // 57
    {0x58, { 0, 0, "pop eax", 0 }}, // 58
    {0x59, { 0, 0, "pop ecx", 0 }}, // 59
    {0x5A, { 0, 0, "pop edx", 0 }}, // 5A
    {0x5B, { 0, 0, "pop ebx", 0 }}, // 5B
    {0x5C, { 0, 0, "pop esp", 0 }}, // 5C
    {0x5D, { 0, 0, "pop ebp", 0 }}, // 5D
    {0x5E, { 0, 0, "pop esi", 0 }}, // 5E
    {0x5F, { 0, 0, "pop edi", 0 }}, // 5F
    {0x60, { 0, 0, "pusha", 0 }}, // 60
    {0x61, { 0, 0, "popa", 0 }}, // 61
    {0x62, { 1, QWORD, "bound ", 2, R, RM }}, // 62
    {0x63, { 1, WORD, "arpl ", 2, RM, R }}, // 63
    {0x64, { 0, 0, "fs ", 0 }}, // 64
    {0x65, { 0, 0, "gs ", 0 }}, // 65
    {0x66, { 0, 0, "data16 ", 0 }}, // 66
    {0x67, { 0, 0, "addr16 ", 0 }}, // 67
    {0x68, { 0, 0, "push ", 1, IMM32 }}, // 68
    {0x69, { 1, DWORD, "imul ", 3, R, RM, IMM32 }}, // 69
    {0x6A, { 0, 0, "push ", 1, IMM8 }}, // 6A
    {0x6B, { 1, DWORD, "imul ", 3, R, RM, IMM8 }}, // 6B
    {0x6C, { 0, 0, "ins BYTE PTR es:[edi],dx", 0 }}, // 6C
    {0x6D, { 0, 0, "ins DWORD PTR es:[edi],dx", 0 }}, // 6D
    {0x6E, { 0, 0, "outs dx,BYTE PTR ds:[esi]", 0 }}, // 6E
    {0x6F, { 0, 0, "outs dx,DWORD PTR ds:[esi]", 0 }}, // 6F

    {0x70, { 0, 0, "jo ", 1, REL8 }}, // 70
    {0x71, { 0, 0, "jno ", 1, REL8 }}, // 71
    {0x72, { 0, 0, "jb ", 1, REL8 }}, // 72
    {0x73, { 0, 0, "jnb ", 1, REL8 }}, // 73
    {0x74, { 0, 0, "jz ", 1, REL8 }}, // 74
    {0x75, { 0, 0, "jne ", 1, REL8 }}, // 75
    {0x76, { 0, 0, "jbe ", 1, REL8 }}, // 76
    {0x77, { 0, 0, "ja ", 1, REL8 }}, // 77
    {0x78, { 0, 0, "js ", 1, REL8 }}, // 78
    {0x79, { 0, 0, "jns ", 1, REL8 }}, // 79
    {0x7A, { 0, 0, "jp ", 1, REL8 }}, // 7A
    {0x7B, { 0, 0, "jnp ", 1, REL8 }}, // 7B
    {0x7C, { 0, 0, "jl ", 1, REL8 }}, // 7C
    {0x7D, { 0, 0, "jnl ", 1, REL8 }}, // 7D
    {0x7E, { 0, 0, "jle ", 1, REL8 }}, // 7E
    {0x7F, { 0, 0, "jnle ", 1, REL8 }}, // 7F


    {0x80, { 1, BYTE, "immGroup ", 2, RM, IMM8 }}, // 80
    {0x81, { 1, DWORD, "immGroup ", 2, RM, IMM32 }}, // 81
    {0x82, { 0, 0, "immGroup ", 0 }}, // 82
    {0x83, { 1, DWORD, "immGroup ", 2, RM, IMM8 }}, // 83
    
    
    /*
    {0x80, { 1, BYTE, "add ", 2, RM, IMM8 }}, // 80
    {0x81, { 1, DWORD, "add ", 2, RM, IMM32 }}, // 81
    {0x82, { 0, 0, ".byte 0x82", 0 }}, // 82
    {0x83, { 1, DWORD, "adc ", 2, RM, IMM8 }}, // 83
    */
    {0x84, { 1, BYTE, "test ", 2, RM, R }}, // 84
    {0x85, { 1, DWORD, "test ", 2, RM, R }}, // 85
    {0x86, { 1, BYTE, "xchg ", 2, RM, R }}, // 86
    {0x87, { 1, DWORD, "xchg ", 2, RM, R }}, // 87
    {0x88, { 1, BYTE, "mov ", 2, RM, R }}, // 88
    {0x89, { 1, DWORD, "mov ", 2, RM, R }}, // 89
    {0x8A, { 1, BYTE, "mov ", 2, R, RM }}, // 8A
    {0x8B, { 1, DWORD, "mov ", 2, R, RM }}, // 8B
    {0x8C, { 1, WORD, "mov ", 2, RM, SS }}, // 8C
    {0x8D, { 1, 0, "lea ", 2, R, RM }}, // 8D
    {0x8E, { 1, WORD, "mov ss,", 1, RM }}, // 8E
    {0x8F, { 1, DWORD, "pop ", 1, RM }}, // 8F
    {0x90, { 0, 0, "nop", 0 }}, // 90
    {0x91, { 0, 0, "xchg ecx,eax", 0 }}, // 91
    {0x92, { 0, 0, "xchg edx,eax", 0 }}, // 92
    {0x93, { 0, 0, "xchg ebx,eax", 0 }}, // 93
    {0x94, { 0, 0, "xchg esp,eax", 0 }}, // 94
    {0x95, { 0, 0, "xchg ebp,eax", 0 }}, // 95
    {0x96, { 0, 0, "xchg esi,eax", 0 }}, // 96
    {0x97, { 0, 0, "xchg edi,eax", 0 }}, // 97
    {0x98, { 0, 0, "cwde", 0 }}, // 98
    {0x99, { 0, 0, "cdq", 0 }}, // 99
    {0x9A, { 0, 0, "call ", 1, PTR1632 }}, // 9A
    {0x9B, { 0, 0, "fwait", 0 }}, // 9B
    {0x9C, { 0, 0, "pushf", 0 }}, // 9C
    {0x9D, { 0, 0, "popf", 0 }}, // 9D
    {0x9E, { 0, 0, "sahf", 0 }}, // 9E
    {0x9F, { 0, 0, "lahf", 0 }}, // 9F
    {0xA0, { 0, 0, "mov al,ds:", 1, IMM8 }}, // A0
    {0xA1, { 0, 0, "mov eax,ds:", 1, IMM8 }}, // A1
    {0xA2, { 0, 0, "mov ", 2, IMM8, AL }}, // A2
    {0xA3, { 0, 0, "mov ", 2, IMM32, EAX }}, // A3
    {0xA4, { 0, 0, "movs BYTE PTR es:[edi],BYTE PTR ds:[esi]", 0 }}, // A4
    {0xA5, { 0, 0, "movs DWORD PTR es:[edi],DWORD PTR ds:[esi]", 0 }}, // A5
    {0xA6, { 0, 0, "cmps BYTE PTR es:[esi],BYTE PTR ds:[edi]", 0 }}, // A6
    {0xA7, { 0, 0, "cmps DWORD PTR es:[esi],DWORD PTR ds:[edi]", 0 }}, // A7
    {0xA8, { 0, 0, "test al,", 1, IMM8 }}, // A8
    {0xA9, { 0, 0, "test eax,", 1, IMM32 }}, // A9
    {0xAA, { 0, 0, "stos BYTE PTR es:[edi],al", 0 }}, // AA
    {0xAB, { 0, 0, "stos DWORD PTR es:[edi],eax", 0 }}, // AB
    {0xAC, { 0, 0, "lods al,BYTE PTR ds:[esi]", 0 }}, // AC
    {0xAE, { 0, 0, "scas al,BYTE PTR es:[edi]", 0 }}, // AE
    {0xAF, { 0, 0, "scas eax,DWORD PTR es:[edi]", 0 }}, // AF


    

    {0xB0, { 0, 0, "mov ", 2, AL, IMM8 }}, // B0
    {0xB1, { 0, 0, "mov ", 2, CL, IMM8 }}, // B1
    {0xB2, { 0, 0, "mov ", 2, DL, IMM8 }}, // B2
    {0xB3, { 0, 0, "mov ", 2, BL, IMM8 }}, // B3
    {0xB4, { 0, 0, "mov ", 2, AH, IMM8 }}, // B4
    {0xB5, { 0, 0, "mov ch,", 2, CH, IMM8 }}, // B5
    {0xB6, { 0, 0, "mov dh,", 2, DH, IMM8 }}, // B6
    {0xB7, { 0, 0, "mov bh,", 2, BH, IMM8 }}, // B7
    {0xB8, { 0, 0, "mov eax,", 2,EAX , IMM32 }}, // B8
    {0xB9, { 0, 0, "mov ecx,", 2,ECX , IMM32 }}, // B9
    {0xBA, { 0, 0, "mov edx,", 2,EDX, IMM32 }}, // BA
    {0xBB, { 0, 0, "mov ebx,", 2,EBX, IMM32 }}, // BB
    {0xBC, { 0, 0, "mov esp,", 2,ESP, IMM32 }}, // BC
    {0xBD, { 0, 0, "mov ebp,", 2,EBP, IMM32 }}, // BD
    {0xBE, { 0, 0, "mov esi,", 2,ESI , IMM32 }}, // BE
    {0xBF, { 0, 0, "mov edi,", 1,EDI , IMM32 }}, // BF

    {0xC0, { 1, BYTE, "immGroup2 ", 2, RM, IMM8 }}, // C0
    {0xC1, { 1, DWORD, "immGroup2 ", 2, RM, IMM8 }}, // C1


    {0xC2, { 0, 0, "ret ", 1, IMM16 }}, // C2
    {0xC3, { 0, 0, "ret", 0 }}, // C3
    {0xC4, { 1, FWORD, "les eax,", 1, RM }}, // C4
    {0xC5, { 1, FWORD, "lds eax,", 1, RM }}, // C5
    {0xC6, { 1, BYTE, "mov ", 2, RM, IMM8 }}, // C6
    {0xC7, { 1, DWORD, "mov ", 2, RM, IMM32 }}, // C7
    {0xC8, { 0, 0, "enter ", 2, IMM16, IMM8 }}, // C8
    {0xC9, { 0, 0, "leave", 0 }}, // C9
    {0xCA, { 0, 0, "retf ", 1, IMM16 }}, // CA
    {0xCB, { 0, 0, "retf", 0 }}, // CB
    {0xCC, { 0, 0, "int3", 0 }}, // CC
    {0xCD, { 0, 0, "int ", 1, IMM8 }}, // CD
    {0xCE, { 0, 0, "into", 0 }}, // CE
    {0xCF, { 0, 0, "iret", 0 }}, // CF
    /*
    {0xD0, { 1, BYTE, "rol ", 2, RM, ONE }}, // D0
    {0xD1, { 1, DWORD, "rol ", 2, RM, ONE }}, // D1
    {0xD2, { 1, BYTE, "rol ", 2, RM, CL }}, // D2
    {0xD3, { 1, DWORD, "rol ", 2, RM, CL }}, // D3
    */
    {0xD0, { 1, BYTE, "immGroup2 ", 2, RM, ONE }}, // D0
    {0xD1, { 1, DWORD, "immGroup2 ", 2, RM, ONE }}, // D1
    {0xD2, { 1, BYTE, "immGroup2 ", 2, RM, CL }}, // D2
    {0xD3, { 1, DWORD, "immGroup2 ", 2, RM, CL }}, // D3

    {0xD4, { 0, 0, "aam ", 1, IMM8 }}, // D4
    {0xD5, { 0, 0, "aad ", 1, IMM8 }}, // D5
    {0xD6, { 0, 0, ".byte 0xd6", 0 }}, // D6
    {0xD7, { 0, 0, "xlat BYTE PTR ds:[ebx]", 0 }}, // D7
    {0xD8, { 1, DWORD, "fadd ", 1, RM }}, // D8
    {0xD9, { 1, DWORD, "fld ", 1, RM }}, // D9
    {0xDA, { 1, DWORD, "fiadd ", 1, RM }}, // DA
    {0xDB, { 1, DWORD, "fild ", 1, RM }}, // DB
    {0xDF, { 1, QWORD, "fadd ", 1, RM }}, // DC
    {0xDD, { 1, QWORD, "fld ", 1, RM }}, // DD
    {0xDE, { 1, WORD, "fiadd ", 1, RM }}, // DE
    {0xDF, { 1, WORD, "fild ", 1, RM }}, // DF
    {0xE0, { 0, 0, "loopne ", 1, REL8 }}, // E0
    {0xE1, { 0, 0, "loope ", 1, REL8 }}, // E1
    {0xE2, { 0, 0, "loop ", 1, REL8 }}, // E2
    {0xE3, { 0, 0, "jecxz ", 1, REL8 }}, // E3
    {0xE4, { 0, 0, "in al,", 1, IMM8 }}, // E4
    {0xE5, { 0, 0, "in eax,", 1, IMM8 }}, // E5
    {0xE6, { 0, 0, "out ", 2, IMM8, AL }}, // E6
    {0xE7, { 0, 0, "out ", 2, IMM8, EAX }}, // E7
    {0xE8, { 0, 0, "call ", 1, REL32 }}, // E8
    {0xE9, { 0, 0, "jmp ", 1, REL32 }}, // E9
    {0xEA, { 0, 0, "jmpf ", 1, PTR1632 }}, // EA
    {0xEB, { 0, 0, "jmp ", 1, REL8 }}, // EB
    {0xEC, { 0, 0, "in al,dx", 0 }}, // EC
    {0xED, { 0, 0, "in eax,dx", 0 }}, // ED
    {0xEE, { 0, 0, "out dx,al", 0 }}, // EE
    {0xEF, { 0, 0, "out dx,eax", 0 }}, // EF
    {0xF0, { 0, 0, "lock ", 0 }}, // F0
    {0xF1, { 0, 0, "icebp", 0 }}, // F1
    {0xF2, { 0, 0, "repnz ", 0 }}, // F2
    {0xF3, { 0, 0, "repz", 0 }}, // F3
    {0xF4, { 0, 0, "hlt", 0 }}, // F4
    {0xF5, { 0, 0, "cmc", 0 }}, // F5
    {0xF6, { 1, BYTE, "test ", 2, RM, IMM8 }}, // F6
    {0xF7, { 1, DWORD, "test ", 2, RM, IMM32 }}, // F7
    {0xF8, { 0, 0, "clc", 0 }}, // F8
    {0xF9, { 0, 0, "stc", 0 }}, // F9
    {0xFA, { 0, 0, "cli", 0 }}, // FA
    {0xFB, { 0, 0, "sti", 0 }}, // FB
    {0xFC, { 0, 0, "cld", 0 }}, // FC
    {0xFD, { 0, 0, "std", 0 }}, // FD
    {0xFE, { 1, BYTE, "inc ", 1, RM }}, // FE
    {0xFF, { 1, DWORD, "inc ", 1, RM }} // FF
};



public:
  Decorder(std::list<unsigned char> input_bytes);
  void showInputBytes();
  void decode(int testNum);
  //void createOutput(std::list <unsigned char>::iterator &it, char modRM_mod,  std::string R_output, std::string RM_output, unsigned char opcode, unsigned char modbyte);
  void createOutput(int testNum,char modRM_mod,  std::string R_output, std::string RM_output, unsigned char opcode, unsigned char modbyte);
  int getProgramCounter();

  void jump();


};