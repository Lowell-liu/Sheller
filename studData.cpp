#include "studData.h"
#include "Stud\Stud.h"
#include "puPEinfoData.h"
#include "CompressionData.h"

PuPEInfo obj_pePe;

extern _Stud* g_stu;

studData::studData()
{
	m_lpBase = obj_pePe.puGetImageBase();

	m_dwNewSectionAddress = (DWORD)obj_pePe.puGetSectionAddress((char *)m_lpBase, (BYTE *)".mas");

	m_Oep = obj_pePe.puOldOep();
}

studData::~studData()
{

}

// Stud����׼��
BOOL studData::LoadLibraryStud()
{
	CompressionData obj_compress;

	m_studBase = obj_compress.puGetStubBase();
	// ���ص��ڴ�
	// m_studBase = LoadLibraryEx(L"E:\\VS��Ŀ\\�ӿ���\\Release\\Stud.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
	// ��ȡdll�ĵ�������
	dexportAddress = GetProcAddress((HMODULE)m_studBase, "main");
	// WinMain = GetProcAddress((HMODULE)m_studBase, "WinMain");
	// ����ȫ��
	// g_stu = (_Stud*)GetProcAddress((HMODULE)m_studBase, "g_stud");
	// �������ε�ַ
	m_dwStudSectionAddress = (DWORD)obj_pePe.puGetSectionAddress((char *)m_studBase, (BYTE *)".text");

	m_dwNewSectionAddress = (DWORD)obj_pePe.puGetSectionAddress((char *)m_lpBase, (BYTE *)".mas");

	m_ImageBase = ((PIMAGE_NT_HEADERS)obj_pePe.puGetNtHeadre())->OptionalHeader.ImageBase;
	
	return TRUE;
}

// �޸��ض�λ
BOOL studData::RepairReloCationStud()
{
	PIMAGE_DOS_HEADER pStuDos = (PIMAGE_DOS_HEADER)m_studBase;

	PIMAGE_NT_HEADERS pStuNt = (PIMAGE_NT_HEADERS)(pStuDos->e_lfanew + (DWORD)m_studBase);

	PIMAGE_BASE_RELOCATION pStuRelocation = (PIMAGE_BASE_RELOCATION)(pStuNt->OptionalHeader.DataDirectory[5].VirtualAddress + (DWORD)m_studBase);
	
	typedef struct _Node
	{
		WORD offset : 12;
		WORD type : 4;
	}Node, *PNode;

	while (pStuRelocation->SizeOfBlock)
	{
		DWORD nStuRelocationBlockCount = (pStuRelocation->SizeOfBlock - 8) / 2;

		_Node* RelType = (PNode)(pStuRelocation + 1);

		for (DWORD i = 0; i < nStuRelocationBlockCount; ++i)
		{
			if (RelType[i].type != 3) continue;

			DWORD* pRel = (DWORD *)(pStuRelocation->VirtualAddress + RelType[i].offset + (DWORD)m_studBase);

			DWORD OldAttribute = 0;

			VirtualProtect(pRel, 8, PAGE_READWRITE, &OldAttribute);

			*pRel = *pRel - (DWORD)m_studBase - ((PIMAGE_SECTION_HEADER)m_dwStudSectionAddress)->VirtualAddress + m_ImageBase + ((PIMAGE_SECTION_HEADER)m_dwNewSectionAddress)->VirtualAddress;
			
			VirtualProtect(pRel, 8, OldAttribute, &OldAttribute);
		}
		pStuRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pStuRelocation + pStuRelocation->SizeOfBlock);
	}
	return TRUE;
}

// ����stud���ݵ���������
BOOL studData::CopyStud()
{
	g_stu->s_dwOepBase = m_Oep;

	FILE* fpFile = nullptr;

	if ((fpFile = fopen("FileData.txt", "a+")) == NULL)
		AfxMessageBox(L"�ļ���ʧ��");

	fwrite(&m_Oep, sizeof(DWORD), 1, fpFile);

	fclose(fpFile);

	PIMAGE_SECTION_HEADER studSection = obj_pePe.puGetSectionAddress((char *)m_studBase, (BYTE *)".text");
	
	PIMAGE_SECTION_HEADER SurceBase = obj_pePe.puGetSectionAddress((char *)m_lpBase, (BYTE *)".mas");
		
	memcpy(
		(void *)(SurceBase->PointerToRawData + (DWORD)m_lpBase),
		(void *)(studSection->VirtualAddress + (DWORD)m_studBase),
		studSection->Misc.VirtualSize
		);

	DWORD dwRiteFile = 0;	OVERLAPPED overLapped = { 0 };

	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)obj_pePe.puGetNtHeadre();

	// OEP�ص�
	pNt->OptionalHeader.AddressOfEntryPoint = (DWORD)dexportAddress - (DWORD)m_studBase - studSection->VirtualAddress + SurceBase->VirtualAddress;

	int nRet = WriteFile(obj_pePe.puFileHandle(), obj_pePe.puGetImageBase(), obj_pePe.puFileSize(), &dwRiteFile, &overLapped);

	if (!nRet)
		return FALSE;

	return TRUE;
}