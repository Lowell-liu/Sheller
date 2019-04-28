#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"


// MasterWindows �Ի���

class MasterWindows : public CDialogEx
{
	DECLARE_DYNAMIC(MasterWindows)

public:
	MasterWindows(CWnd* pParent = NULL);				// ��׼���캯��
	virtual ~MasterWindows();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_MasterStaticText;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CString m_MasterStaticTextStr;

public:
	afx_msg void OnBnClickedButton4();
//	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton9();

	/*�Զ�������*/
private:
	// ��ʾPE����
	void ShowPEInfoData(const CString & FileName);
	// ��������ڲ����ú���
	BOOL NewSection();
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
};
