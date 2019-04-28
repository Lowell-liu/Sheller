// MasterWindows.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MasterWindows.h"
#include "afxdialogex.h"
#include "puPEinfoData.h"
#include "SectionInfo.h"
#include "AddSection.h"
#include "studData.h"
#include "CompressionData.h"
#include "UnShell.h"

// MasterWindows �Ի���

IMPLEMENT_DYNAMIC(MasterWindows, CDialogEx)

MasterWindows::MasterWindows(CWnd* pParent /*=NULL*/)
	: CDialogEx(MasterWindows::IDD, pParent)
	, m_MasterStaticTextStr(_T(""))
{

}

MasterWindows::~MasterWindows()
{
}

void MasterWindows::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_MasterStaticText);
	DDX_Text(pDX, IDC_STATIC1, m_MasterStaticTextStr);
}

BEGIN_MESSAGE_MAP(MasterWindows, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &MasterWindows::OnBnClickedButton1)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON4, &MasterWindows::OnBnClickedButton4)
	// ON_BN_CLICKED(IDC_BUTTON3, &MasterWindows::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON9, &MasterWindows::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON3, &MasterWindows::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &MasterWindows::OnBnClickedButton2)
END_MESSAGE_MAP()

// MasterWindows ��Ϣ�������

BOOL MasterWindows::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/*��������ͼ��*/
	SetIcon(LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON1)), TRUE);

	return TRUE; 
}

// һ���̼ӿ�
void MasterWindows::OnBnClickedButton1()
{
	// �� ����������~�κ�ѹ��
	PuPEInfo obj_Peinfo;

	// 1. ��������
	if (NewSection())
		AfxMessageBox(L"��������γɹ�");
	else
		AfxMessageBox(L"���������ʧ��");

	CloseHandle(obj_Peinfo.puFileHandle()); UpdateData(TRUE);

	obj_Peinfo.puOpenFileLoad(m_MasterStaticTextStr);


	// 2. ѹ��ȫ������ ѹ����ʱ���������Ŀ¼���Լ����δ�С����ѹ���������Σ�
	CompressionData obj_ComperData;

	CloseHandle(obj_Peinfo.puFileHandle()); UpdateData(TRUE);

	obj_Peinfo.puOpenFileLoad(m_MasterStaticTextStr);

	if (!obj_ComperData.puCompressSection())
		AfxMessageBox(L"CompressSection failuer!");
	else
		AfxMessageBox(L"CompressSection Seucess!");

	CloseHandle(obj_Peinfo.puFileHandle()); 

	m_MasterStaticTextStr = "C:\\Users\\Administrator\\Desktop\\CompressionMask.exe";

	obj_Peinfo.puOpenFileLoad(m_MasterStaticTextStr);

	// 3. Stud���ݲ���...
	studData obj_stuData;

	obj_stuData.puLoadLibraryStud();

	obj_stuData.puRepairReloCationStud();

	if (obj_stuData.puCopyStud())
	{
		CloseHandle(obj_Peinfo.puFileHandle());
		UpdateData(TRUE);
		obj_Peinfo.puOpenFileLoad(m_MasterStaticTextStr);
	}
	else
		AfxMessageBox(L"StudWrite failure!");

	 // 4����β����
	 CloseHandle(obj_Peinfo.puFileHandle());
}

// ��Ӧ�ļ���ק
void MasterWindows::OnDropFiles(HDROP hDropInfo)
{
	// 1. ��ȡ��ק��Ŀ
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	// 2. �����ȡ��·��
	char wcStr[MAX_PATH] = {};
	for (int i = 0; i < DropCount; ++i)
	{
		wcStr[0] = 0;
		DragQueryFileA(hDropInfo, i, wcStr, MAX_PATH);
		m_MasterStaticTextStr = wcStr;
	}
	// 3. ������ʾ
	UpdateData(FALSE);
	// 4. ��ʾPE��Ϣ���������β鿴��
	ShowPEInfoData(m_MasterStaticTextStr);
	// 5. �ͷ��ڴ�
	DragFinish(hDropInfo);
	CDialogEx::OnDropFiles(hDropInfo);
}

// ��ʾPE����
void MasterWindows::ShowPEInfoData(const CString & FileName)
{
	PuPEInfo obj_puPe; CString Tempstr;	DWORD TempdwCode = 0;

	if (!obj_puPe.puOpenFileLoad(FileName))
		return;

	PIMAGE_NT_HEADERS pNtHeadre = (PIMAGE_NT_HEADERS)obj_puPe.puGetNtHeadre();

	PIMAGE_FILE_HEADER pFileHeadre = (PIMAGE_FILE_HEADER)&pNtHeadre->FileHeader;

	PIMAGE_OPTIONAL_HEADER pOption = (PIMAGE_OPTIONAL_HEADER)&pNtHeadre->OptionalHeader;

	// ��������
	TempdwCode = pFileHeadre->NumberOfSections;
	Tempstr.Format(L"%d", TempdwCode);
	SetDlgItemText(IDC_EDIT9, Tempstr);
	
	// OEP
	TempdwCode = pOption->AddressOfEntryPoint;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT1, Tempstr);

	// Ĭ�ϼ��ػ�ַ
	TempdwCode = pOption->ImageBase;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT3, Tempstr);

	// ��־��
	TempdwCode = pOption->Magic;
	Tempstr.Format(L"%04X", TempdwCode);
	SetDlgItemText(IDC_EDIT2, Tempstr);

	// ����Ŀ¼����
	TempdwCode = pOption->NumberOfRvaAndSizes;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT7, Tempstr);

	// ��ʼ������������ַ
	TempdwCode = pOption->BaseOfCode;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT4, Tempstr);

	// ��ʼ������Ե�ַ
	TempdwCode = pOption->BaseOfData;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT5, Tempstr);

	// ���������
	TempdwCode = pOption->SectionAlignment;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT6, Tempstr);

	// �ļ���������
	TempdwCode = pOption->FileAlignment;
	Tempstr.Format(L"%08X", TempdwCode);
	SetDlgItemText(IDC_EDIT8, Tempstr);

}

// ����������Ϣ
void MasterWindows::OnBnClickedButton4()
{
	SectionInfo obj_section;
	obj_section.DoModal();
	return;
}

// ��������ӣ��ǣ�
void MasterWindows::OnBnClickedButton9()
{
	if (NewSection())
		AfxMessageBox(L"��������γɹ�");
	else
		AfxMessageBox(L"���������ʧ��");
}

// ��������ڲ����ú���
BOOL MasterWindows::NewSection()
{
	// ������
	AddSection obj_addsection; BOOL nRet = TRUE;

	BYTE Name[] = ".mas";

	const DWORD SectionSize = 0x14D00;

	obj_addsection.puModifySectioNumber();

	nRet = obj_addsection.puModifySectionInfo(Name, SectionSize);

	obj_addsection.puModifyProgramEntryPoint();

	obj_addsection.puModifySizeofImage();

	nRet = obj_addsection.puAddNewSectionByData(SectionSize);

	return nRet;
}

// ȫ������ѹ��
void MasterWindows::OnBnClickedButton3()
{
	CompressionData obj_ComperData;
	
	if (!obj_ComperData.puCompressSection())
		AfxMessageBox(L"CompressSection failuer!");
	else
		AfxMessageBox(L"CompressSection Seucess!");
}

// һ���ѿ�
void MasterWindows::OnBnClickedButton2()
{
	UnShell obj_Unshell;

	obj_Unshell.puRepCompressionData();

	obj_Unshell.puDeleteSectionInfo();

	if (obj_Unshell.puSaveUnShell())
		AfxMessageBox(L"һ���ѿǳɹ�");

}
