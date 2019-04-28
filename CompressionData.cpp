#include "CompressionData.h"
#include "puPEinfoData.h"
#include "Stud\Stud.h"
#include "lz4.h"
#include "AddSection.h"

_Stud* g_stu = nullptr;

void* CompressionData::m_lpBase = nullptr;

HANDLE CompressionData::m_studBase = nullptr;

CompressionData::CompressionData()
{
	PuPEInfo obj_peInfo;

	m_lpBase = obj_peInfo.puGetImageBase();
	
	m_SectionHeadre = obj_peInfo.puGetSection();

	m_SectionCount = ((PIMAGE_NT_HEADERS)(obj_peInfo.puGetNtHeadre()))->FileHeader.NumberOfSections;

	m_hFile = obj_peInfo.puFileHandle();

	m_hFileSize = obj_peInfo.puFileSize();
}

CompressionData::~CompressionData()
{
}

// ���һ�����θ�ѹ���������ʹ��
void CompressionData::AddCompreDataSection(const DWORD & size)
{
	BYTE Name[] = ".com";

	DWORD Compresdata = size;

	AddSection obj_addSection;

	obj_addSection.puModifySectioNumber();

	obj_addSection.puModifySectionInfo(Name, Compresdata);

	obj_addSection.puModifySizeofImage();

	obj_addSection.puAddNewSectionByData(Compresdata);
}

// ѹ��PE��������
BOOL CompressionData::CompressSectionData()
{
	if ((fpFile = fopen("FileData.txt", "w+")) == NULL)
	{
		AfxMessageBox(L"�ļ���ʧ��");
	}

	m_studBase = LoadLibraryEx(L"E:\\VS��Ŀ\\�ӿ���\\Release\\Stud.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);

	g_stu = (_Stud*)GetProcAddress((HMODULE)m_studBase, "g_stud");

	g_stu->s_OneSectionSizeofData = FALSE;

	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(((PIMAGE_DOS_HEADER)m_lpBase)->e_lfanew + (DWORD)m_lpBase);

	DWORD dSectionCount = pNt->FileHeader.NumberOfSections;

	PIMAGE_SECTION_HEADER psection = (PIMAGE_SECTION_HEADER)m_SectionHeadre;

	PuPEInfo obj_peInfo;

	m_maskAddress = obj_peInfo.puGetSectionAddress((char *)m_lpBase, (BYTE *)".mas");

	// ������.textbss������
	for (DWORD i = 0; i < dSectionCount; ++i)
	{
		if (psection->PointerToRawData != 0)
			break;
		++psection;
	}

	// pe��׼��С����󣨼��ػ�ַ + .text->pointertorawdata�����ݣ�= ��С
	DWORD pStandardHeadersize = psection->PointerToRawData;

	char* SaveCompressData = (char*)malloc(m_hFileSize);

	memset(SaveCompressData, 0, m_hFileSize);

	PIMAGE_SECTION_HEADER pSections = (PIMAGE_SECTION_HEADER)m_SectionHeadre;

	DWORD ComressTotalSize = 0;

	// ��ѹ�����������Σ��ӿ����Σ�
	for (DWORD i = 0; i < dSectionCount - 2; ++i)
	{
		
		DWORD DataSize = pSections->SizeOfRawData;

		if (pSections->SizeOfRawData == 0)
		{
			fwrite(&pSections->SizeOfRawData, sizeof(DWORD), 1, fpFile);
			fflush(fpFile);
			++pSections;
			g_stu->s_OneSectionSizeofData = TRUE;
			continue;
		}

		void* DataAddress = (void *)(pSections->PointerToRawData + (DWORD)m_lpBase);

		char* buf = NULL;

		DWORD blen;

		// ���㰲ȫ������
		blen = LZ4_compressBound(pSections->SizeOfRawData);

		// ��ȫ�ռ�����
		if ((buf = (char*)malloc(sizeof(char) * blen)) == NULL)
		{
			AfxMessageBox(L"no enough memory!\n");
			return -1;
		}

		DWORD dwCompressionSize = 0;

		/* ѹ�� */
		dwCompressionSize = LZ4_compress_default((char*)DataAddress, buf, pSections->SizeOfRawData, blen);
	
		fwrite(&dwCompressionSize, sizeof(DWORD), 1, fpFile);

		fflush(fpFile);

		// ���㻺��ȥ���С
		memcpy(&g_stu->s_blen[i], &dwCompressionSize, sizeof(DWORD));

		// ����ѹ�����������ݣ�ƴ��ÿһ��ѹ�����Σ�
		memcpy(&SaveCompressData[ComressTotalSize], buf, dwCompressionSize);

		// ����ѹ�����ܴ�С
		ComressTotalSize += dwCompressionSize;

		free(buf);

		++pSections;
	}

	// 0x400 + (ѹ����Ĵ�С / 0x200 + ----ѹ����Ĵ�С % 0x200 ? 1 : 0) 0x200;
	// ���������
	DWORD Size = 0;
	if (ComressTotalSize % 0x200 == 0)
	{
		Size = pStandardHeadersize + ((ComressTotalSize / 0x200) * 0x200);
		int a = 10;
	}
	else
	{
		Size = pStandardHeadersize + (((ComressTotalSize / 0x200) + 1) * 0x200);
		int a = 10;
	}

	DWORD ModifySize = Size - 0x400;

	// ����һ��������
	AddCompreDataSection(ModifySize);

	// ���¼��ظ���PE���ݱ�֤�������ݻ�ȡ����
	PuPEInfo obj_Peinfo;

	CloseHandle(obj_Peinfo.puFileHandle());

	FileName = obj_peInfo.puFilePath();

	obj_Peinfo.puOpenFileLoad(FileName);

	CompressionData obj_Compre;
	// �޸������ε���Ϣ���� �ļ�ƫ�� 0x400  ��С ѹ�������ݶ����С
	BYTE Name[] = ".com";

	obj_peInfo.puSetFileoffsetAndFileSize(m_lpBase, 0x400, ModifySize, Name);

	BYTE Nmase[] = ".com";

	PIMAGE_SECTION_HEADER compSectionAddress = obj_peInfo.puGetSectionAddress((char*)m_lpBase, Nmase);

	// �����ڴ��ַ ���ڽ�ѹ��ַ
	g_stu->s_CompressionSectionRva = compSectionAddress->VirtualAddress;

	// ����ѹ���������(����) --> �¼ӵ�����
	memcpy((void*)(compSectionAddress->PointerToRawData + (DWORD)m_lpBase), SaveCompressData, ModifySize);

	// ƴ�ӱ�׼PEͷ + ѹ�����ݵ����� + �Լ�������
	char* ComressNewBase = (char*)malloc(Size + m_maskAddress->SizeOfRawData);

	memset(ComressNewBase, 0, (Size + m_maskAddress->SizeOfRawData));

	// ƴ�ӱ�׼PE
	memcpy(ComressNewBase, m_lpBase, pStandardHeadersize);

	// ƴ��ѹ�����ȫ������(��һ��ͷ��Ϣ)
	memcpy(&ComressNewBase[pStandardHeadersize], (void*)(compSectionAddress->PointerToRawData + (DWORD)m_lpBase), ComressTotalSize);

	// ƴ�Ӽӿ���������
	memcpy(&ComressNewBase[Size], (void *)(m_maskAddress->PointerToRawData + (DWORD)m_lpBase), m_maskAddress->SizeOfRawData);

	DWORD dwWrite = 0;	OVERLAPPED OverLapped = { 0 };

	// �������Ŀ¼��(��β����)
	CleanDirectData(ComressNewBase, ComressTotalSize, Size);

	// �����ļ�
	HANDLE Handle = CreateFile(L"MaskCompre.exe", GENERIC_READ | GENERIC_WRITE, FALSE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// д��exe�������ѹ��
	int nRet = WriteFile(Handle, ComressNewBase, (Size + m_maskAddress->SizeOfRawData), &dwWrite, &OverLapped);

	// �رվ��
	CloseHandle(Handle);

	// �������ļ�·����
 	nRet = CopyFile(L"MaskCompre.exe", L"C:\\Users\\Administrator\\Desktop\\CompressionMask.exe", FALSE);

	if (!nRet)
		AfxMessageBox(L"CopyFile failure");

	if (!nRet)
		AfxMessageBox(L"CompressWriteFile failuer");

	fclose(fpFile);

	return TRUE;
}

// �ж��������������ݴ�С��δ���룩
DWORD CompressionData::IsSectionSize(DWORD MiscVirtualsize, DWORD sizeOfRawData)
{
	if (MiscVirtualsize > sizeOfRawData)
		return sizeOfRawData;
	if (MiscVirtualsize < sizeOfRawData)
		return MiscVirtualsize;
	if (MiscVirtualsize == sizeOfRawData)
		return sizeOfRawData;
	return 0;
}

// �������Ŀ¼������
BOOL CompressionData::CleanDirectData(const char* NewAddress, const DWORD & CompresSize, const DWORD & Size)
{
	if ((fpFile = fopen("FileData.txt", "a+")) == NULL)
	{
		AfxMessageBox(L"�ļ���ʧ��");
	}

	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(((PIMAGE_DOS_HEADER)NewAddress)->e_lfanew + (DWORD)NewAddress);

	PIMAGE_DATA_DIRECTORY pDirectory = (PIMAGE_DATA_DIRECTORY)pNt->OptionalHeader.DataDirectory;

	DWORD dwSectionCount = pNt->FileHeader.NumberOfSections;

	g_stu->s_SectionCount = dwSectionCount;

	int k = 0;
	// ����\�������Ŀ¼��
	for (DWORD i = 0; i < 16; ++i)
	{
		memcpy(&g_stu->s_DataDirectory[i][0], &pDirectory->VirtualAddress, sizeof(DWORD));
		memcpy(&g_stu->s_DataDirectory[i][1], &pDirectory->Size, sizeof(DWORD));
		//fprintf(fpFile, "%x %x", pDirectory->VirtualAddress, pDirectory->Size);
		fwrite(&pDirectory->VirtualAddress, sizeof(DWORD), 1, fpFile);
		fwrite(&pDirectory->Size, sizeof(DWORD), 1, fpFile);
		fflush(fpFile);
		pDirectory->VirtualAddress = 0;
		pDirectory->Size = 0;
		++pDirectory;
	}

	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);

	// ����\��������ļ���С���ļ�ƫ��
	for (DWORD i = 0; i < dwSectionCount - 2; ++i)
	{
		memcpy(&g_stu->s_SectionOffsetAndSize[i][0], &pSection->SizeOfRawData, sizeof(DWORD));
		memcpy(&g_stu->s_SectionOffsetAndSize[i][1], &pSection->PointerToRawData, sizeof(DWORD));
		//fprintf(fpFile, "% %04x", pSection->SizeOfRawData, pSection->PointerToRawData);
		fwrite(&pSection->SizeOfRawData, sizeof(DWORD), 1, fpFile);
		fwrite(&pSection->PointerToRawData, sizeof(DWORD), 1, fpFile);
		fflush(fpFile);
		pSection->SizeOfRawData = 0;
		pSection->PointerToRawData = 0;
		++pSection;
	}

	// ���һ�������ǿ����� ��Ϣ���� �޸��ļ�ƫ��
	// �ı��ļ�ƫ�ƶ�����ļ�ƫ�Ƶĵط�
	pSection->PointerToRawData = Size;

	fclose(fpFile);

	return 0;
}
