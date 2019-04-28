#pragma once
#ifndef STUDDATA_H_
#define STUDDATA_H_
#include "stdafx.h"


/*
	�����ƣ�studData
	��;��stud������ز���
	ʱ�䣺2018/12/1
	�޸����ڣ�2018/12/2
*/


class studData
{
public:
	studData();
	~studData();

public:
	void puLoadLibraryStud(){ this->LoadLibraryStud(); }

	void puRepairReloCationStud(){ this->RepairReloCationStud(); }

	BOOL puCopyStud(){ return this->CopyStud(); }

private:
	// ����stud
 	BOOL LoadLibraryStud();
	// �޸��ض�λ
	BOOL RepairReloCationStud();
	// ����stud���ݵ���������
	BOOL CopyStud();

private:
	// ���浼�������ĵ�ַ
	void* dexportAddress = 0; // main
	void* WinMain = 0;		  // WinMain
	// ������ػ�ַstud
	void* m_studBase = nullptr;
	// ���浱ǰ�ļ�lpBase���ػ�ַ
	void* m_lpBase = nullptr;
	// ���������ε���ʼ��ַ
	DWORD m_dwNewSectionAddress = 0;
	// studԭ���ε���ʼλ��
	DWORD m_dwStudSectionAddress = 0;
	// ����OEPԭʼ
	DWORD m_Oep = 0;
	// ����ImageBase
	DWORD m_ImageBase = 0;
};

#endif