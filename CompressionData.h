#pragma once
#ifndef COMPRESSIONDATA_H_
#define COMPRESSIONDATA_H_
#include "stdafx.h"

/*
	�����ƣ�CompressionData
	��;��ѹ��PE����
	ʱ�䣺2018/12/1	
*/

class CompressionData
{
public:
	CompressionData();
	~CompressionData();

public:
	// void puAddCompresData(){ this->AddCompreDataSection(); }
	BOOL puCompressSection(){ return this->CompressSectionData(); }
	// void puCleanDirectData(char* NewAddress){ this->CleanDirectData(NewAddress); }
	HANDLE puGetStubBase(){ return this->m_studBase; }

private:
	// ѹ��PE��������
	BOOL CompressSectionData();
	// �������Ŀ¼��
	BOOL CleanDirectData(const char* NewAddress, const DWORD & CompresSize, const DWORD & Size);
	// �ж��������������ݴ�С��δ���룩
	DWORD IsSectionSize(DWORD MiscVirtualsize, DWORD sizeOfRawData);
	// ���һ�����θ�ѹ���������ʹ��
	void AddCompreDataSection(const DWORD & size);

private:
	// ��ȡ��ַ
	static void* m_lpBase;
	// ��������ͷ
	void* m_SectionHeadre = nullptr;
	// �����ļ���С
	DWORD m_hFileSize = 0;
	// �������θ���
	DWORD m_SectionCount = 0;
	// ��ȡ�ļ����
	void* m_hFile = nullptr;
	// �������stub�ĵ�ַ
	static HANDLE m_studBase;
	// ����mask�����ε�ַ
	PIMAGE_SECTION_HEADER m_maskAddress;
	// �����ļ����Ƹ���
	CString FileName;
	// �ļ����
	FILE* fpFile = nullptr;
};

#endif