#include "HashOfFunction.h"

HashofFunction::HashofFunction()
{

}

HashofFunction::~HashofFunction()
{

}

// ��ȡģ���ַ
DWORD HashofFunction::GetModule(const DWORD Hash)
{
	DWORD	nDllBase = 0;
	__asm{
	/*��ģ��*/
		pushad
		push		Hash;
		call		GetModulVA
		mov			nDllBase, eax;
		popad

	/*����1������PEB_LDR_DATA����HASH����*/
	GetModulVA :
		push		ebp;
		mov			ebp, esp;
		sub			esp, 0x20;
		push		edx;
		push		ebx;
		push		edi;
		push		esi;
		mov			ecx, 8;
		mov			eax, 0CCCCCCCCh;
		lea			edi, dword ptr[ebp - 0x20];
		rep stos	dword ptr es : [edi]
		mov			esi, dword ptr fs : [0x30];
		mov			esi, dword ptr[esi + 0x0C];
		mov			esi, dword ptr[esi + 0x1C];
	tag_Modul:
		mov			dword ptr[ebp - 0x8], esi;	// ����LDR_DATA_LIST_ENTRY
		mov			ebx, dword ptr[esi + 0x20];	// DLL������ָ��(Ӧ��ָ��һ���ַ���)
		mov			eax, dword ptr[ebp + 0x8];
		push		eax;
		push		ebx;						// +0xC
		call		HashModulVA
		test		eax, eax;
		jnz			_ModulSucess
		mov			esi, dword ptr[ebp - 0x8];
		mov			esi, [esi];					// ������һ��
		LOOP		tag_Modul
	_ModulSucess :
		mov			esi, dword ptr[ebp - 0x8];
		mov			eax, dword ptr[esi + 0x8];
		pop			esi;
		pop			edi;
		pop			ebx;
		pop			edx;
		mov			esp, ebp;
		pop			ebp;
		ret

	/*����2��HASH�����㷨�����ַ����ܣ�*/
	HashModulVA :
		push		ebp;
		mov			ebp, esp;
		sub			esp, 0x04;
		mov			dword ptr[ebp - 0x04], 0x00
		push		ebx;
		push		ecx;
		push		edx;
		push		esi;
		// ��ȡ�ַ�����ʼ����
		mov			esi, [ebp + 0x8];
		test		esi, esi;
		jz			tag_failuers
		xor			ecx, ecx;
		xor			eax, eax;
	tag_loops:
		mov			al, [esi + ecx];		// ��ȡ�ֽڼ���
		test		al, al					// 0���˳�
		jz			tag_ends
		mov			ebx, [ebp - 0x04];
		shl			ebx, 0x19;
		mov			edx, [ebp - 0x04];
		shr         edx, 0x07;
		or			ebx, edx;
		add			ebx, eax;
		mov[ebp - 0x4], ebx;
		inc			ecx;
		inc			ecx;
		jmp			tag_loops
	tag_ends :
		mov			ebx, [ebp + 0x0C];		// ��ȡHASH
		mov			edx, [ebp - 0x04];
		xor			eax, eax;
		cmp			ebx, edx;
		jne			tag_failuers
		mov			eax, 1
		jmp			tag_funends
		tag_failuers :
		mov			eax, 0
	tag_funends :
		pop			esi;
		pop			edx;
		pop			ecx;
		pop			ebx;
		mov			esp, ebp;
		pop			ebp;
		ret			0x08
	}
	return nDllBase;
}

// ��ȡ������ַ
DWORD HashofFunction::GetProcAddress(const DWORD dllvalues, const DWORD Hash)
{
	DWORD FunctionAddress = 0;
	__asm{
		pushad;
		push		Hash;						// Hash���ܵĺ�������
		push		dllvalues;					// ģ���ַ.dll
		call		GetHashFunVA;				// GetProcess
		mov			FunctionAddress, eax;		// �� �����ַ
		popad;
		// �Զ��庯������Hash�ҶԱȷ�����ȷ�ĺ���
	GetHashFunVA:
		push		ebp;
		mov			ebp, esp;
		sub			esp, 0x30;
		push		edx;
		push		ebx;
		push		esi;
		push		edi;
		lea			edi, dword ptr[ebp - 0x30];
		mov			ecx, 12;
		mov			eax, 0CCCCCCCCh;
		rep	stos	dword ptr es : [edi];
		// ���Ͽ���ջ֡������Debug�汾ģʽ��
		mov			eax, [ebp + 0x8];				// �� kernel32.dll(MZ)
		mov			dword ptr[ebp - 0x8], eax;
		mov			ebx, [ebp + 0x0c];				// �� GetProcAddress Hashֵ
		mov			dword ptr[ebp - 0x0c], ebx;
		// ��ȡPEͷ��RVA��ENT
		mov			edi, [eax + 0x3C];				// e_lfanew
		lea			edi, [edi + eax];				// e_lfanew + MZ = PE
		mov			dword ptr[ebp - 0x10], edi;		// �� ����PE��VA��
		// ��ȡENT
		mov			edi, dword ptr[edi + 0x78];		// ��ȡ������RVA
		lea			edi, dword ptr[edi + eax];		// ������VA
		mov[ebp - 0x14], edi;						// �� ���浼����VA
		// ��ȡ������������
		mov			ebx, [edi + 0x18];
		mov			dword ptr[ebp - 0x18], ebx;		// �� ���溯����������
		// ��ȡENT
		mov			ebx, [edi + 0x20];				// ��ȡENT(RVA)
		lea			ebx, [eax + ebx];				// ��ȡENT(VA)
		mov			dword ptr[ebp - 0x20], ebx;		// �� ����ENT(VA)
		// ����ENT ���ܹ�ϣֵ�Ա��ַ���
		mov			edi, dword ptr[ebp - 0x18];
		mov			ecx, edi;
		xor			esi, esi;
		mov			edi, dword ptr[ebp - 0x8];
		jmp			_WHILE
		// ����ѭ��
		_WHILE :
		mov			edx, dword ptr[ebp + 0x0c];		// HASH
		push		edx;
		mov			edx, dword ptr[ebx + esi * 4];	// ��ȡ��һ���������Ƶ�RVA
		lea			edx, [edi + edx];				// ��ȡһ���������Ƶ�VA��ַ
		push		edx;							// ENT���е�һ���ַ�����ַ
		call		_STRCMP
		cmp			eax, 0;
		jnz			_SUCESS
		inc			esi;
		LOOP		_WHILE
		jmp			_ProgramEnd
		// �Աȳɹ�֮���ȡѭ���������±꣩cx�����±���
	_SUCESS :
		// ��ȡEOT������ű�����
		mov			ecx, esi
		mov			ebx, dword ptr[ebp - 0x14];
		mov			esi, dword ptr[ebx + 0x24];
		mov			ebx, dword ptr[ebp - 0x8];
		lea			esi, [esi + ebx];				// ��ȡEOT��VA
		xor			edx, edx
		mov			dx, [esi + ecx * 2];			// ע��˫�� ��ȡ���
		// ��ȡEAT��ַ��RVA
		mov			esi, dword ptr[ebp - 0x14];		// Export VA
		mov			esi, [esi + 0x1C];
		mov			ebx, dword ptr[ebp - 0x8];
		lea			esi, [esi + ebx]				// ��ȡEAT��VA			
		mov			eax, [esi + edx * 4]			// ����ֵeax��GetProcess��ַ��
		lea			eax, [eax + ebx]
		jmp			_ProgramEnd

	_ProgramEnd :
		pop			edi;
		pop			esi;
		pop			ebx;
		pop			edx;
		mov			esp, ebp;
		pop			ebp;
		ret			0x8;

		// ѭ���Ա�HASHֵ
	_STRCMP:
		push		ebp;
		mov			ebp, esp;
		sub			esp, 0x04;
		mov			dword ptr[ebp - 0x04], 0x00;
		push		ebx;
		push		ecx;
		push		edx;
		push		esi;
		// ��ȡ�ַ�����ʼ����
		mov			esi, [ebp + 0x8];
		xor			ecx, ecx;
		xor			eax, eax;

	tag_loop:
		mov			al, [esi + ecx];		// ��ȡ�ֽڼ���
		test		al, al;					// 0���˳�
		jz			tag_end;
		mov			ebx, [ebp - 0x04];
		shl			ebx, 0x19;
		mov			edx, [ebp - 0x04];
		shr         edx, 0x07;
		or			ebx, edx;
		add			ebx, eax;
		mov[ebp - 0x4], ebx;
		inc			ecx;
		jmp			tag_loop

	tag_end :
		mov			ebx, [ebp + 0x0C];		// ��ȡHASH
		mov			edx, [ebp - 0x04];
		xor			eax, eax;
		cmp			ebx, edx;
		jne			tag_failuer;
		mov			eax, 1;
		jmp			tag_funend

	tag_failuer :
		mov			eax, 0

	tag_funend :
		pop			esi;
		pop			edx;
		pop			ecx;
		pop			ebx;
		mov			esp, ebp;
		pop			ebp;
		ret			0x08
	}
	return FunctionAddress;
}