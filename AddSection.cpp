#include "AddSection.h"
#include "puPEinfoData.h"


char* AddSection::newlpBase = nullptr;

AddSection::AddSection()
{
	PuPEInfo obj_PuPE;

	pFileBaseData = obj_PuPE.puGetImageBase();

	pNtHeadre = obj_PuPE.puGetNtHeadre();

	pSectionHeadre = obj_PuPE.puGetSection();

	FileSize = obj_PuPE.puFileSize();

	FileHandle = obj_PuPE.puFileHandle();

	OldOep = obj_PuPE.puOldOep();
}

AddSection::~AddSection()
{

}

// �޸���������
BOOL AddSection::ModifySectionNumber()
{
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)this->pNtHeadre;

	DWORD temp = pNtHeaders->FileHeader.NumberOfSections;

	SectionSizeof = temp * 0x28;

	pNtHeaders->FileHeader.NumberOfSections += 0x1;

	return TRUE;
}

// �޸�������Ϣ(RVA\��С\����)
BOOL AddSection::ModifySectionInfo(const BYTE* Name, const DWORD & size)
{
	DWORD pSectionAddress = (DWORD)pSectionHeadre;
	// ��ȡ���һ�����νṹ�ĵ�ַ(ĩβ����һ����ʼ��)
	pSectionAddress = pSectionAddress + SectionSizeof - 0x28;
	// ������һ����������Ϣ
	PIMAGE_SECTION_HEADER PtrpSection = (PIMAGE_SECTION_HEADER)pSectionAddress;
	// �����ε�ַ
	pSectionAddress += 0x28;
	NewpSection = (PIMAGE_SECTION_HEADER)pSectionAddress;
	// �޸�����������
	memcpy(NewpSection->Name, Name, sizeof(Name));
	DWORD dwtemps = PtrpSection->VirtualAddress + PtrpSection->SizeOfRawData;
	// �ڴ�����޸���0x1000��
	__asm{
		pushad;
		mov		esi, dwtemps;
		mov		eax, dwtemps;
		mov		edx, 0x1;
		mov		cx, 0x1000;
		div		cx;
		test	dx, dx;
		jz		MemSucess
		shr		dx, 12;
		inc		dx;
		shl		dx, 12;
		add		esi, edx;
		shr		esi, 12;
		shl		esi, 12;
		mov		dwtemps, esi;
	MemSucess:
		popad
	}
	NewpSection->VirtualAddress = dwtemps;
	// �ļ������޸�
	/*
		ȱ��һ���жϣ���Ȼ�������������
			�ж��Ƿ��ļ�ĩβ���������ĩβ��ַ��ʹ���ļ�ĩβ��ַ+1�������п��ܻḲ��һЩ���ݣ�
	*/
	DWORD Temp = PtrpSection->SizeOfRawData + PtrpSection->PointerToRawData;
	// ����������� DWORD Temp = 0x00AA4567;
	__asm{
		pushad;
		mov		esi, Temp;
		mov		edx, 0x1;
		mov		eax, Temp;
		mov		ecx, 0x200;
		div		cx;
		test	dx, dx;
		jz		FileSucess
		xor		eax, eax
		mov		ax, 0x200;
		sub		ax, dx;
		add		esi, eax;
		mov		Temp, esi;
	FileSucess:
		popad
	}
	// �����α�����������ݱ���
	NewpSection->PointerToRawData = Temp;
	// �����δ�С(0x1000)��С
	NewpSection->SizeOfRawData = size;
	NewpSection->Misc.VirtualSize = NewpSection->SizeOfRawData;
	// �������Կɶ���д��ִ��
	NewpSection->Characteristics = 0xE00000E0;
	return TRUE;
}

// �޸���ڵ�
BOOL AddSection::ModifyProgramEntryPoint()
{
	// ��ڵ����VirtualAddress
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)pNtHeadre;

	pNt->OptionalHeader.AddressOfEntryPoint = NewpSection->VirtualAddress;

	return TRUE;
}

// �޸ľ����ַ��С��ȥ�������ַ
BOOL AddSection::ModifySizeofImage()
{
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)pNtHeadre;

	pNt->OptionalHeader.SizeOfImage = NewpSection->VirtualAddress + NewpSection->SizeOfRawData;
	
	pNt->OptionalHeader.DllCharacteristics = 0x8000;
	
	return TRUE;
}

// �����µ��ֽ�(һ������ڵ���)
BOOL AddSection::AddNewSectionByteData(const DWORD & size)
{
	// �����µĶѿռ�
	newlpBase = (char *)malloc(FileSize + size);
	// ��ʼ���ռ�
	memset(newlpBase, 0, (FileSize + size));
	// �����޸ĺ�����ݵ����ڴ�ռ�
	memcpy(newlpBase, pFileBaseData, FileSize);
	// �ͷ�ԭ���Ŀռ�
	free(pFileBaseData);

	DWORD dWriteSize = 0; OVERLAPPED OverLapped = { 0 };

	int nRetCode = WriteFile(FileHandle, newlpBase, (FileSize + size), &dWriteSize, &OverLapped);

	free(newlpBase);

	if (dWriteSize == 0){ AfxMessageBox(L"CreateSection WriteFIle faliuer"); return FALSE; }

	return TRUE;
}

