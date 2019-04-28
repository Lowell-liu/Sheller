#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxcmn.h"

// SectionInfo �Ի���

class SectionInfo : public CDialogEx
{
	DECLARE_DYNAMIC(SectionInfo)

public:
	SectionInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SectionInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SectionList;
	virtual BOOL OnInitDialog();

	/*�Զ�������*/
private:
	// ��ʾ������Ϣ
	void ShowSectionInfo();
	// �б���ʾ
	static const TCHAR strName[6][20];
	// ����������Ϣ
	PIMAGE_SECTION_HEADER pSectionHeadre;
	// ������������
	PIMAGE_NT_HEADERS pNtHeadre;
	DWORD SectionCount = 0;
};
