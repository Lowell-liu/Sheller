#pragma once
#ifndef UNSHELL_H_
#define UNSHELL_H_
#include "stdafx.h"

/*
	�����ƣ�UnShell
	��;���ѿ�
	ʱ�䣺2018/12/7
*/

class UnShell
{
public:
	UnShell();
	~UnShell();

public:
	void puRepCompressionData(){ this->RepCompressionData(); }
	void puDeleteSectionInfo(){ this->DeleteSectionInfo(); }
	BOOL puSaveUnShell(){ return this->SaveUnShell(); }

private:
	// �ָ�ѹ��������
	BOOL RepCompressionData();
	// ɾ����������
	BOOL DeleteSectionInfo();
	// ���漰��β����
	BOOL SaveUnShell();

private:
	// �����ļ���ַ
	void* m_Base = nullptr;
	// �����ļ�NT
	void* m_NtAddress = nullptr;
	// �����ļ����
	HANDLE hFile = nullptr;
	// ����stub����
	void* m_studBase = nullptr;
	// ����PE����
	PIMAGE_DOS_HEADER pDosHander;
	PIMAGE_NT_HEADERS pHeadres;
	PIMAGE_SECTION_HEADER pSection;
	// ���������Ľ�ѹ����
	char* UnShellNewFile = nullptr;
	// �����������ݴ�С
	DWORD TotaldwSize = 0;
	// �����ѹ�������
	char* Sectionbuf = nullptr;
	// �ļ�ָ��
	FILE *fpFile = nullptr;
};

#endif