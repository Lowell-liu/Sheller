#include "stud.h"
#include <CommCtrl.h>
#include "../lz4.h"

#pragma comment(linker, "/merge:.data=.text")
#pragma comment(linker, "/merge:.rdata=.text")
#pragma comment(linker, "/section:.text,RWE")



// ==================����һ��ȫ�ֱ�������======================
extern "C"{
	__declspec(dllexport) Stud g_stud = { 0 };
}

// ����GetModulHnadle��NULL��
HINSTANCE g_hInstance;

// =====================α��������============================
typedef void* (WINAPI*FnGetProcAddress)(HMODULE, const char*);
FnGetProcAddress MyGetProcAddress;

typedef HMODULE (WINAPI* FnLoadLibraryExA)(_In_ LPCSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags);
FnLoadLibraryExA MyLoadLibraryExA;

typedef HMODULE (WINAPI* FnGetModuleHandleW)(_In_opt_ LPCWSTR lpModuleName);
FnGetModuleHandleW MyGetModuleHandleW;

typedef HBRUSH  (WINAPI* FnCreateSolidBrush)(_In_ COLORREF color);
FnCreateSolidBrush MyCreateSolidBrush;

typedef ATOM (WINAPI* FnRegisterClassW)(_In_ CONST WNDCLASSW *lpWndClass);
FnRegisterClassW MyRegisterClassW;

typedef	WINUSERAPI HWND(WINAPI* FnCreateWindowExW)(
	_In_ DWORD dwExStyle,
	_In_opt_ LPCWSTR lpClassName,
	_In_opt_ LPCWSTR lpWindowName,
	_In_ DWORD dwStyle,
	_In_ int X,
	_In_ int Y,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_opt_ HWND hWndParent,
	_In_opt_ HMENU hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam);
FnCreateWindowExW MyCreateWindowExW;

typedef BOOL (WINAPI* FnShowWindow)(_In_ HWND hWnd, _In_ int nCmdShow);
FnShowWindow MyShowWindow;

typedef BOOL (WINAPI* FnUpdateWindow)(_In_ HWND hWnd);
FnUpdateWindow MyUpdateWindow;

typedef BOOL (WINAPI* FnGetMessageW)(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax);
FnGetMessageW MyGetMessageW;

typedef BOOL (WINAPI* FnTranslateMessage)(_In_ CONST MSG *lpMsg);
FnTranslateMessage	MyTranslateMessage;

typedef LRESULT (WINAPI* FnDispatchMessageW)(_In_ CONST MSG *lpMsg);
FnDispatchMessageW MyDispatchMessageW;

typedef int (WINAPI* FnGetWindowTextW)(_In_ HWND hWnd, _Out_writes_(nMaxCount) LPWSTR lpString, _In_ int nMaxCount);
FnGetWindowTextW MyGetWindowTextW;

typedef int (WINAPI* FnlstrcmpW)(_In_ LPCWSTR lpString1, _In_ LPCWSTR lpString2);
FnlstrcmpW MylstrcmpW;

typedef int (WINAPI* FnMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType);
FnMessageBoxA MyMessageBoxA;

typedef VOID(WINAPI* FnPostQuitMessage)(_In_ int nExitCode);
FnPostQuitMessage MyPostQuitMessage;

typedef LRESULT(WINAPI* FnDefWindowProcW)(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
FnDefWindowProcW MyDefWindowProcW;

typedef HCURSOR(WINAPI* FnLoadCursorW)(_In_opt_ HINSTANCE hInstance, _In_ LPCWSTR lpCursorName);
FnLoadCursorW MyLoadCursorW;

typedef HICON (WINAPI* FnLoadIconW)(_In_opt_ HINSTANCE hInstance, _In_ LPCWSTR lpIconName);
FnLoadIconW MyLoadIconW;

typedef	ATOM (WINAPI* FnRegisterClassExW)(_In_ CONST WNDCLASSEXW *);
FnRegisterClassExW MyRegisterClassExW;

typedef VOID(WINAPI* FnExitProcess)(_In_ UINT uExitCode);
FnExitProcess MyExitProcess;

typedef HWND (WINAPI* FnGetDlgItem)(_In_opt_ HWND hDlg, _In_ int nIDDlgItem);
FnGetDlgItem MyGetDlgItem;

typedef void * (__cdecl* Fnmemcpy)(_Out_writes_bytes_all_(_Size) void * _Dst, _In_reads_bytes_(_Size) const void * _Src, _In_ size_t _Size);
Fnmemcpy Mymemcpy;

typedef BOOL(WINAPI* FnVirtualProtect)(_In_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flNewProtect, _Out_ PDWORD lpflOldProtect);
FnVirtualProtect MyVirtualProtect;

typedef HWND(WINAPI* FnFindWindowExW)(_In_opt_ HWND hWndParent, _In_opt_ HWND hWndChildAfter, _In_opt_ LPCWSTR lpszClass, _In_opt_ LPCWSTR lpszWindow);
FnFindWindowExW MyFindWindowExW;




// ===================��ȡģ���ַ============================
DWORD puGetModule(const DWORD Hash)
{
	DWORD	nDllBase = 0;
	__asm{
		jmp			start
	/*����1������PEB_LDR_DATA����HASH����*/
	GetModulVA:
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
		rep stos	dword ptr es : [edi];
		mov			esi, dword ptr fs : [0x30];
		mov			esi, dword ptr[esi + 0x0C];
		mov			esi, dword ptr[esi + 0x1C];
	tag_Modul:
		mov			dword ptr[ebp - 0x8], esi;	// ����LDR_DATA_LIST_ENTRY
		mov			ebx, dword ptr[esi + 0x20];	// DLL������ָ��(Ӧ��ָ��һ���ַ���)
		mov			eax, dword ptr[ebp + 0x8];
		push		eax;
		push		ebx;						// +0xC
		call		HashModulVA;
		test		eax, eax;
		jnz			_ModulSucess;
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
		jz			tag_failuers;
		xor			ecx, ecx;
		xor			eax, eax;
	tag_loops:
		mov			al, [esi + ecx];		// ��ȡ�ֽڼ���
		test		al, al;					// 0���˳�
		jz			tag_ends;
		mov			ebx, [ebp - 0x04];
		shl			ebx, 0x19;
		mov			edx, [ebp - 0x04];
		shr         edx, 0x07;
		or			ebx, edx;
		add			ebx, eax;
		mov[ebp - 0x4], ebx;
		inc			ecx;
		inc			ecx;
		jmp			tag_loops;
	tag_ends:
		mov			ebx, [ebp + 0x0C];		// ��ȡHASH
		mov			edx, [ebp - 0x04];
		xor			eax, eax;
		cmp			ebx, edx;
		jne			tag_failuers;
		mov			eax, 1;
		jmp			tag_funends;
	tag_failuers:
		mov			eax, 0;
	tag_funends:
		pop			esi;
		pop			edx;
		pop			ecx;
		pop			ebx;
		mov			esp, ebp;
		pop			ebp;
		ret			0x08

	start:
	/*��ģ��*/
		pushad;
		push		Hash;
		call		GetModulVA;
		add			esp, 0x4
		mov			nDllBase, eax;
		popad;
	}
	return nDllBase;
}

// ===================��ȡ������ַ============================
DWORD puGetProcAddress(const DWORD dllvalues, const DWORD Hash)
{
	DWORD FunctionAddress = 0;
	__asm{
		jmp			start
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
		call		_STRCMP;
		cmp			eax, 0;
		jnz			_SUCESS;
		inc			esi;
		LOOP		_WHILE;
		jmp			_ProgramEnd
		// �Աȳɹ�֮���ȡѭ���������±꣩cx�����±���
	_SUCESS :
		// ��ȡEOT������ű�����
		mov			ecx, esi;
		mov			ebx, dword ptr[ebp - 0x14];
		mov			esi, dword ptr[ebx + 0x24];
		mov			ebx, dword ptr[ebp - 0x8];
		lea			esi, [esi + ebx];				// ��ȡEOT��VA
		xor			edx, edx;
		mov			dx, [esi + ecx * 2];			// ע��˫�� ��ȡ���
		// ��ȡEAT��ַ��RVA
		mov			esi, dword ptr[ebp - 0x14];		// Export VA
		mov			esi, [esi + 0x1C];
		mov			ebx, dword ptr[ebp - 0x8];
		lea			esi, [esi + ebx];				// ��ȡEAT��VA			
		mov			eax, [esi + edx * 4];			// ����ֵeax��GetProcess��ַ��
		lea			eax, [eax + ebx];
		jmp			_ProgramEnd;

	_ProgramEnd:
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
		jmp			tag_funend;

	tag_failuer:
		mov			eax, 0;

	tag_funend:
		pop			esi;
		pop			edx;
		pop			ecx;
		pop			ebx;
		mov			esp, ebp;
		pop			ebp;
		ret			0x08

	start:
		pushad;
		push		Hash;						// Hash���ܵĺ�������
		push		dllvalues;					// ģ���ַ.dll
		call		GetHashFunVA;				// GetProcess
		mov			FunctionAddress, eax;		// �� �����ַ
		popad;
	}
	return FunctionAddress;
}

// =====================�ؼ�ʵ��==============================
void SetString(HWND hWnd)
{
	// ��ȡPostQuitMessage
	MyPostQuitMessage = (FnPostQuitMessage)puGetProcAddress(g_stud.s_User32, 0xCAA94781);
	// ��ȡDefWindowProcW
	MyDefWindowProcW = (FnDefWindowProcW)puGetProcAddress(g_stud.s_User32, 0x22E85CBA);
	 // ���ñ߿�
	MyCreateWindowExW(0L, WC_BUTTON, TEXT("   �ȿȡ����ҽ���һ�£����˽п�"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 20, 98, 300, 200, hWnd, NULL, 0, NULL);
	 // �����û������뾲̬�ı�
	MyCreateWindowExW(0L, WC_STATIC, TEXT("Account:"), WS_CHILD | WS_VISIBLE, 30, 145, 80, 20, hWnd, NULL, 0, NULL);
	MyCreateWindowExW(0L, WC_STATIC, TEXT("Passwd :"), WS_CHILD | WS_VISIBLE, 30, 175, 80, 20, hWnd, NULL, 0, NULL);
	 // �����ı���
	MyCreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, TEXT(""), WS_CHILD | WS_VISIBLE, 120, 145, 160, 20, hWnd, (HMENU)0x1001, 0, NULL);
	MyCreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, TEXT(""), WS_CHILD | WS_VISIBLE, 120, 175, 160, 20, hWnd, (HMENU)0x1002, 0, NULL);
	 // ��¼��ť
	MyCreateWindowExW(0L, WC_BUTTON, TEXT("��	¼��"), WS_CHILD | WS_VISIBLE, 120, 220, 70, 25, hWnd, (HMENU)0x1003, 0, NULL);
}

// =====================�����ص�==============================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		// �����ؼ�
		SetString(hWnd);
	}
	break;
	case WM_COMMAND:
	{
		if (0x1003 == LOWORD(wParam)) {
			WCHAR User[20] = { 0 };
			WCHAR Pass[20] = { 0 };
			MyGetWindowTextW(MyGetDlgItem(hWnd, 0x1001), User, 20);
			MyGetWindowTextW(MyGetDlgItem(hWnd, 0x1002), Pass, 20);
			if ((0 == MylstrcmpW(User, L"admin") && (0 == MylstrcmpW(Pass, L"admin"))))
				MyMessageBoxA(NULL, "Seucess", "�ɹ�", NULL);
			else
				MyMessageBoxA(NULL, "Failure", "ʧ��", NULL);
		}
	}
	break;
	case WM_CLOSE:
	{
		MyPostQuitMessage(0);
	}
	break;
	}
	// ���������Ϣȫ������Ĭ�ϻص�����
	return MyDefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// =====================��������==============================
int CreateWind()
{
	WNDCLASS WndClass = { 0 };

	// 1. Ԥ���崰�������Ϣ
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = MyCreateSolidBrush(RGB(255, 255, 255));
	WndClass.hCursor = MyLoadCursorW(NULL, IDC_ARROW);
	WndClass.hIcon = MyLoadIconW(NULL, IDI_APPLICATION);
	WndClass.hInstance = g_hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = TEXT("PasswdWind");
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_VREDRAW | CS_HREDRAW;

	// 2. ע�ᴰ����
	if (!MyRegisterClassW(&WndClass))
	{
		MyMessageBoxA(NULL, "ע�ᴰ����ʧ��", "����", MB_OK | MB_ICONERROR);
		MyExitProcess(0);
	}

	// 3. ����������
	HWND hWnd = MyCreateWindowExW(WS_EX_CLIENTEDGE,TEXT("PasswdWind"), TEXT("��¼����"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 357, 500, NULL, NULL, 0, NULL);

	// 4. ��ʾ��ˢ��������
	MyShowWindow(hWnd, SW_SHOWNORMAL);
	MyUpdateWindow(hWnd);

	// 5. ��ʼ������Ϣѭ��
	MSG msg = { 0 };
	while (MyGetMessageW(&msg, NULL, 0, 0))
	{
		// 5.1 ��Ϣת��
		MyTranslateMessage(&msg);
		// 5.2 ��Ϣ�ַ�
		MyDispatchMessageW(&msg);
	}
	return 0;
}

// ======================��ѹ��===============================
void UnCompression()
{
	// �� ���õĺ�������Ҫ��ȡα����
	// 1. ��ԭ����Ŀ¼���Լ�������Ϣ
	DWORD m_lpbase = 0x400000;
	
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(((PIMAGE_DOS_HEADER)m_lpbase)->e_lfanew + (DWORD)m_lpbase);

	PIMAGE_DATA_DIRECTORY pDataDirectory = (PIMAGE_DATA_DIRECTORY)pNt->OptionalHeader.DataDirectory;
	
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	
	DWORD Att_old = 0;

	for (DWORD i = 0; i < 16; ++i)
	{
		MyVirtualProtect(pDataDirectory, 0x8, PAGE_READWRITE, &Att_old);
		if (0 != g_stud.s_DataDirectory[i][0])
			pDataDirectory->VirtualAddress = g_stud.s_DataDirectory[i][0];
		if (0 != g_stud.s_DataDirectory[i][1])
			pDataDirectory->Size = g_stud.s_DataDirectory[i][1];
		MyVirtualProtect(pDataDirectory, 0x8, Att_old, &Att_old);
		++pDataDirectory;
	}

	for (DWORD i = 0; i < g_stud.s_SectionCount - 2; ++i)
	{
		MyVirtualProtect(pSection, 0x8, PAGE_READWRITE, &Att_old);
		if (0 != g_stud.s_SectionOffsetAndSize[i][0])
			pSection->SizeOfRawData = g_stud.s_SectionOffsetAndSize[i][0];
		if (0 != g_stud.s_SectionOffsetAndSize[i][1])
			pSection->PointerToRawData=g_stud.s_SectionOffsetAndSize[i][1];
		MyVirtualProtect(pSection, 0x8, PAGE_READWRITE, &Att_old);
		++pSection;
	}
	
	PIMAGE_SECTION_HEADER pSections = IMAGE_FIRST_SECTION(pNt);

	//  2. ��ѹȫ���������ݵ������ڴ� virtualAddress = VA
	DWORD Att_olds = 0;
	DWORD SectionAddress = g_stud.s_CompressionSectionRva;
	for (DWORD i = 0; i < g_stud.s_SectionCount - 2; ++i)
	{
		byte* Address = (byte *)(pSections->VirtualAddress + m_lpbase);

		MyVirtualProtect(Address, g_stud.s_SectionOffsetAndSize[i][0], PAGE_READWRITE, &Att_old);
		MyVirtualProtect((void*)SectionAddress, g_stud.s_blen[i], PAGE_READWRITE, &Att_olds);

		// ������  RVA+���ػ�ַ  ��������С  ѹ����ȥ�Ĵ�С
		int nRet = LZ4_decompress_safe((char*)(SectionAddress + m_lpbase), (char*)(pSections->VirtualAddress + m_lpbase), g_stud.s_blen[i], pSections->SizeOfRawData);
		
		MyVirtualProtect(Address, g_stud.s_SectionOffsetAndSize[i][0], Att_old, &Att_old);
		MyVirtualProtect((void*)SectionAddress, g_stud.s_blen[i], Att_olds, &Att_olds);

		++pSections;
		SectionAddress += g_stud.s_blen[i];
	}
}

// ======================������===============================
void FDebug()
{
	MyFindWindowExW = (FnFindWindowExW)puGetProcAddress(g_stud.s_User32, 0x4818F71E);
	char result = 0;
	__asm{
		push eax;
		xor eax, eax;
		mov eax, fs:[0x30];
		mov al, byte ptr[eax + 0x2];
		mov result, al;
		pop eax;
	}
	if (0x1 == result)
		MyExitProcess(0);
	else
	{
		HWND x32 = MyFindWindowExW(NULL, NULL, L"Qt5QWindowIcon", L"x32dbg");
		if (x32 != NULL)
			MyExitProcess(0);
	}
}

// =====================�޸�IAT===============================
void RepairTheIAT()
{
	// Mymemcpy = (Fnmemcpy)puGetProcAddress(g_stud.s_MSVCRT, 0x818F6ED7);
	// ��ȡ���ػ�ַ
	DWORD dwMoudle = (DWORD)MyGetModuleHandleW(NULL);
	// ��ȡ������VA
	DWORD ImportTabVA = g_stud.s_DataDirectory[1][0] + dwMoudle;
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)ImportTabVA;

	while (pImport->Name)
	{
		// ��ȡ��ַ����
		char* Name = (char*)(pImport->Name + dwMoudle);
		// ��ȡģ���ַ
		HMODULE hModuledll = MyLoadLibraryExA(Name, NULL, NULL);
		// ����ģ��
		PIMAGE_THUNK_DATA pThunkINT = (PIMAGE_THUNK_DATA)(pImport->OriginalFirstThunk + dwMoudle);
		PIMAGE_THUNK_DATA pThunkIAT = (PIMAGE_THUNK_DATA)(pImport->FirstThunk + dwMoudle);
		// INT���ļ�������ָ���RVA��һ���ģ� IAT�ڼ��ص��ڴ��ͱ�����������VA
		DWORD Att_old = 0;
		while (pThunkINT->u1.AddressOfData)
		{
			MyVirtualProtect((void*)pThunkIAT, 0x4, PAGE_READWRITE, &Att_old);
			if (!IMAGE_SNAP_BY_ORDINAL32(pThunkIAT->u1.Ordinal))
			{
				// ��ȡBY_NAME�ṹ��
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(pThunkINT->u1.AddressOfData + dwMoudle);
				// ��ȡpName�ĺ�������, �ַ�����̬����
				//unsigned long nDigest = 0;
				//char *p = nullptr;
				//p = pName->Name;
				//while (*p)
				//{
				//	nDigest = ((nDigest << 25) | (nDigest >> 7));
				//	nDigest = nDigest + *p;
				//	p++;
				//}
				// HASH��ȡ����VA
				// DWORD FunAddress = puGetProcAddress((DWORD)hModuledll, nDigest);
				DWORD FunAddress = (DWORD)MyGetProcAddress(hModuledll, pName->Name);
				// ���浽IAT��
				pThunkIAT->u1.Function = FunAddress;
			}
			else
			{
				DWORD dwFunOrdinal = (pThunkIAT->u1.Ordinal) & 0x7FFFFFFF;
				DWORD Fundll = (DWORD)MyGetProcAddress(hModuledll, (char*)dwFunOrdinal);
				pThunkIAT->u1.Function = Fundll;
			}
			// ��䵽IAT��
			MyVirtualProtect((void*)pThunkIAT, 0x4, Att_old, &Att_old);
			++pThunkINT;
			++pThunkIAT;
		}
		++pImport;
	}
}

// ===================��ָ�����=============================
void TakeInstruc()
{
	DWORD p;
	__asm{
		call	l1;
	l1:
		pop		eax;
		mov		p, eax;			//ȷ����ǰ����ε�λ��
		call	f1;
		_EMIT	0xEA;			//��ָ��˴���Զ����ִ�е�
		jmp		l2;				//call�����Ժ�ִ�е�����
	f1:							//������F8OD����ֹ���ԣ�F7�����Ļ�������,why?
		pop ebx;
		inc ebx;
		push ebx;
		mov eax, 0x11111111;
		ret;
	l2:
		call f2;				//��retָ��ʵ����ת
		mov ebx, 0x33333333;	//������Զ����ִ�е�
		jmp e;					//������Զ����ִ�е�
	f2:
		mov ebx, 0x11111111;
		pop ebx;				//����ѹջ�ĵ�ַ
		mov ebx, offset e;		//Ҫ��ת���ĵ�ַ
		push ebx;				//ѹ��Ҫ��ת���ĵ�ַ
		ret;					//��ת
	e:
		mov ebx, 0x22222222;
	}
}

// =====================������=================================
extern "C" __declspec(dllexport) // __declspec(naked)
void main()
{
	g_stud.s_Krenel32 = puGetModule(0xEC1C6278);
	// ��ȡLoadlibraryExA
	MyLoadLibraryExA = (FnLoadLibraryExA)puGetProcAddress(g_stud.s_Krenel32, 0xC0D83287);
	// ����User32.dll
	g_stud.s_User32 = (DWORD)MyLoadLibraryExA("User32.dll", NULL, NULL);
	// ��ȡExitProcW
	MyExitProcess = (FnExitProcess)puGetProcAddress(g_stud.s_Krenel32, 0x4FD18963);
	// ������
	FDebug();
	// ����GDI32.lib
	g_stud.s_Gdi32 = (DWORD)MyLoadLibraryExA("gdi32.dll", NULL, NULL);
	// ����
	g_stud.s_MSVCRT = (DWORD)MyLoadLibraryExA("msvcrt.dll", NULL, NULL);
	// ��ȡGetModuleW
	MyGetModuleHandleW = (FnGetModuleHandleW)puGetProcAddress(g_stud.s_Krenel32, 0xF4E2F2C8);
	g_hInstance = (HINSTANCE)MyGetModuleHandleW(NULL);
	// ��ȡCreateSolidBrush
	MyCreateSolidBrush = (FnCreateSolidBrush)puGetProcAddress(g_stud.s_Gdi32, 0xBB7420F9);
	// ��ȡUpdateData
	MyUpdateWindow = (FnUpdateWindow)puGetProcAddress(g_stud.s_User32, 0x9BB5D8DC);
	// ��ȡGetMessageW
	MyGetMessageW = (FnGetMessageW)puGetProcAddress(g_stud.s_User32, 0x61060461);
	// ��ȡTranslateMessage
	MyTranslateMessage = (FnTranslateMessage)puGetProcAddress(g_stud.s_User32, 0xE09980A2);
	// ��ȡDispatchMessageW
	MyDispatchMessageW = (FnDispatchMessageW)puGetProcAddress(g_stud.s_User32, 0x7A1506D8);
	// ��ȡShowWindow
	MyShowWindow = (FnShowWindow)puGetProcAddress(g_stud.s_User32, 0xDD8B5FB8);
	// ��ȡLoadCursorW
	MyLoadCursorW = (FnLoadCursorW)puGetProcAddress(g_stud.s_User32, 0xC6B20165);
	// ��ȡLoadIconW
	MyLoadIconW = (FnLoadIconW)puGetProcAddress(g_stud.s_User32, 0x7636E8F4);
	// ��ȡRegisterClassExW
	MyRegisterClassW = (FnRegisterClassW)puGetProcAddress(g_stud.s_User32, 0xBC05E48);
	// ��ȡMessageBox
	MyMessageBoxA = (FnMessageBoxA)puGetProcAddress(g_stud.s_User32, 0x1E380A6A);
	// ��ȡCreateWindowExW
	MyCreateWindowExW = (FnCreateWindowExW)puGetProcAddress(g_stud.s_User32, 0x1FDAF571);
	// ��ȡGetWindowTextW
	MyGetWindowTextW = (FnGetWindowTextW)puGetProcAddress(g_stud.s_User32, 0x457BF55A);
	// ��ȡlstrcmpW
	MylstrcmpW = (FnlstrcmpW)puGetProcAddress(g_stud.s_Krenel32, 0x7EAD1F86);
	// ��ȡDefWindowProcW
	MyDefWindowProcW = (FnDefWindowProcW)puGetProcAddress(g_stud.s_User32, 0x22E85CBA);
	// ��ȡGetDlgItem
	MyGetDlgItem = (FnGetDlgItem)puGetProcAddress(g_stud.s_User32, 0x5D0CB479);
	// ��ȡmemcpy
	// Mymemcpy = (Fnmemcpy)puGetProcAddress(g_stud.s_MSVCRT, 0x818F6ED7);
	MyVirtualProtect = (FnVirtualProtect)puGetProcAddress(g_stud.s_Krenel32, 0xEF64A41E);
	// ��ȡMyGetProcessAddress
	MyGetProcAddress = (FnGetProcAddress)puGetProcAddress(g_stud.s_Krenel32, 0xBBAFDF85);
	// ��ѹȫ����������
	UnCompression();
	// �޸�IAT
	RepairTheIAT();
	// ���뵯��
	CreateWind();
	// �ض�λ�޸� ֻ�޸���stub���ض�λ
	// ��ָ�������
	TakeInstruc();
	// ������OEP��ת
	__asm {
		pushad;
		mov		eax, g_stud.s_dwOepBase;
		add		eax, 0x400000;
		jmp		eax;
		popad;
	}
}


// ==================�޸��ض�λ(Ŀ�����)========================
// ====================TLS����==================================