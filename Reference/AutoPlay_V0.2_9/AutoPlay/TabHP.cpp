// TabHP.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPlay.h"
#include "TabHP.h"


// CTabHP dialog

IMPLEMENT_DYNAMIC(CTabHP, CDialog)

CTabHP::CTabHP(CWnd* pParent /*=NULL*/)
	: CDialog(CTabHP::IDD, pParent)
{
	
}

CTabHP::~CTabHP()
{
}

void CTabHP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLD_LIMIT_HP, m_sldLimitHP);
	DDX_Control(pDX, IDC_SLD_LIMIT_MP, m_sldLimitMP);
	DDX_Control(pDX, IDC_TXT_PERCENT_HP, m_txtPercentHP);
	DDX_Control(pDX, IIDC_TXT_PERCENT_MP, m_txtPercentMP);
	DDX_Control(pDX, IDC_CMB_HP1, m_cmbHP1);
	DDX_Control(pDX, IDC_CMB_HP2, m_cmbHP2);
	DDX_Control(pDX, IDC_CMB_HP3, m_cmbHP3);
	DDX_Control(pDX, IDC_CMB_MP1, m_cmbMP1);
	DDX_Control(pDX, IDC_CMB_MP2, m_cmbMP2);
	DDX_Control(pDX, IDC_CMB_MP3, m_cmbMP3);
	
}


BEGIN_MESSAGE_MAP(CTabHP, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_TXT_PERCENT_HP, &CTabHP::OnEnChangeTxtPercentHp)
	ON_EN_CHANGE(IIDC_TXT_PERCENT_MP, &CTabHP::OnEnChangeTxtPercentMp)

END_MESSAGE_MAP()


BOOL CTabHP::OnInitDialog(){
	CDialog::OnInitDialog();
	m_txtPercentHP.SetWindowTextW(L"0");
	m_txtPercentMP.SetWindowTextW(L"0");
	return true;
}

// CTabHP message handlers

void CTabHP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	int iCurPos = m_sldLimitHP.GetPos();
	CString strPos;
	strPos.Format(L"%d",iCurPos);
	m_txtPercentHP.SetWindowTextW(strPos);

	iCurPos = m_sldLimitMP.GetPos();
	strPos.Format(L"%d",iCurPos);
	m_txtPercentMP.SetWindowTextW(strPos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

// Khi kéo thanh trược HP thì số hiện lên đây
void CTabHP::OnEnChangeTxtPercentHp()
{
	CString strNumHP;
	m_txtPercentHP.GetWindowTextW(strNumHP);
	int iCurPos =  _ttoi(strNumHP);
	m_sldLimitHP.SetPos(iCurPos);
	if(iCurPos>100)
		m_txtPercentHP.SetWindowTextW(L"100");
}

void CTabHP::OnEnChangeTxtPercentMp()
{
	CString strNumHP;
	m_txtPercentMP.GetWindowTextW(strNumHP);
	int iCurPos =  _ttoi(strNumHP);
	m_sldLimitMP.SetPos(iCurPos);
	if(iCurPos>100)
		m_txtPercentMP.SetWindowTextW(L"100");
}

BOOL CTabHP::LoadPotionToList()
{
	//Get Ini File Path
	TCHAR m_curDir[256];
	GetCurrentDirectory(sizeof(m_curDir),m_curDir);
	CString strIniFileName;
	strIniFileName.Format(L"%s", m_curDir);
	strIniFileName += L"\\potion.ini";
	
	CFileStatus m_status;
	if( !CFile::GetStatus(strIniFileName, m_status))
	{
		// not existed
		return FALSE;
	}
	
	m_iniReader.setINIFileName(strIniFileName);
	CString strInfo = L"Info";

	//Load HP info
	CString strHPCount = L"HPCount";
	CString strHPType = L"HPType";

	int m_hpCount = m_iniReader.getItegerValue(strInfo, strHPCount);
	for (int i = 0; i < m_hpCount; i++)
	{
		CString strHPKey;
		strHPKey.Format(L"%d", i+1);
		CString strHP = m_iniReader.getKeyValue(strHPType, strHPKey);
		m_cmbHP1.InsertString(i, strHP);
		m_cmbHP2.InsertString(i, strHP);
		m_cmbHP3.InsertString(i, strHP);
	}

	m_cmbHP1.SetCurSel(0);
	m_cmbHP2.SetCurSel(0);
	m_cmbHP3.SetCurSel(0);

	//Load Mana info
	CString strManaCount = L"ManaCount";
	CString strManaType = L"ManaType";
	
	int m_manaCount = m_iniReader.getItegerValue(strInfo, strManaCount);
	for (int i = 0; i < m_manaCount; i++)
	{
		CString strManaKey;
		strManaKey.Format(L"%d", i+1);
		CString strMana = m_iniReader.getKeyValue(strManaType, strManaKey);
		m_cmbMP1.InsertString(i, strMana);
		m_cmbMP2.InsertString(i, strMana);
		m_cmbMP3.InsertString(i, strMana);		
	}

	m_cmbMP1.SetCurSel(0);
	m_cmbMP2.SetCurSel(0);
	m_cmbMP3.SetCurSel(0);
	return TRUE;
}

void CTabHP::SaveCharacterInfo(CString strName)
{
	TCHAR m_curDir[256];
	GetCurrentDirectory(sizeof(m_curDir),m_curDir);
	CString strIniFileName;
	strIniFileName.Format(L"%s", m_curDir);
	strIniFileName += L"\\Character";
	CreateDirectory(strIniFileName, NULL);
	strIniFileName = strIniFileName + L"\\" + strName + L".ini";

	CIniReader m_iniCharReader;
	m_iniCharReader.setINIFileName(strIniFileName);

	CString strHP1, strHP2, strHP3, strMana1, strMana2, strMana3;
	m_iniCharReader.setKey(L"Info", L"Name", strName);

	strHP1.Format(L"%d",m_cmbHP1.GetCurSel());
	m_iniCharReader.setKey(L"Info", L"HP1", strHP1);

	strHP2.Format(L"%d",m_cmbHP2.GetCurSel());
	m_iniCharReader.setKey(L"Info", L"HP2", strHP2);

	strHP3.Format(L"%d",m_cmbHP3.GetCurSel());
	m_iniCharReader.setKey(L"Info", L"HP3", strHP3);

	strMana1.Format(L"%d",m_cmbMP1.GetCurSel());
	m_iniCharReader.setKey(L"Info", L"MN1", strMana1);

	strMana2.Format(L"%d",m_cmbMP2.GetCurSel());
	m_iniCharReader.setKey(L"Info", L"MN2", strMana2);

	strMana3.Format(L"%d",m_cmbMP3.GetCurSel());
	m_iniCharReader.setKey(L"Info", L"MN3", strMana3);
	
	CString strHPPercent, strMNPercent;
	GetDlgItemText(IDC_TXT_PERCENT_HP, strHPPercent);
	GetDlgItemText(IIDC_TXT_PERCENT_MP, strMNPercent);
	
	m_iniCharReader.setKey(L"Info", L"HPPercent", strHPPercent);
	m_iniCharReader.setKey(L"Info", L"MNPercent", strMNPercent);
}

BOOL CTabHP::LoadCharacterPotion(CString strName)
{
	//Check if file is existed or not
	TCHAR m_curDir[256];
	GetCurrentDirectory(sizeof(m_curDir),m_curDir);
	CString strIniFileName;
	strIniFileName.Format(L"%s", m_curDir);
	strIniFileName += L"\\Character";
	CreateDirectory(strIniFileName, NULL);
	strIniFileName = strIniFileName + L"\\" + strName + L".ini";

	CFileStatus m_status;
	if( !CFile::GetStatus(strIniFileName, m_status))
	{
		// not existed
		return FALSE;
	}

	//Load Character Potion
	CIniReader m_iniCharReader;
	m_iniCharReader.setINIFileName(strIniFileName);
	m_cmbHP1.SetCurSel(m_iniCharReader.getItegerValue(L"Info", L"HP1"));
	m_cmbHP2.SetCurSel(m_iniCharReader.getItegerValue(L"Info", L"HP2"));
	m_cmbHP3.SetCurSel(m_iniCharReader.getItegerValue(L"Info", L"HP3"));
	m_cmbMP1.SetCurSel(m_iniCharReader.getItegerValue(L"Info", L"MN1"));
	m_cmbMP2.SetCurSel(m_iniCharReader.getItegerValue(L"Info", L"MN2"));
	m_cmbMP3.SetCurSel(m_iniCharReader.getItegerValue(L"Info", L"MN3"));
	m_txtPercentHP.SetWindowText(m_iniCharReader.getKeyValue(L"Info", L"HPPercent"));
	m_txtPercentMP.SetWindowText(m_iniCharReader.getKeyValue(L"Info", L"MNPercent"));
	UpdateData(FALSE);
	return TRUE;
}
void CTabHP::OnOK(){
}

void CTabHP::OnCancel(){
}