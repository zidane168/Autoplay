// SendAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendApp.h"
#include "SendAppDlg.h"
#include "..\\DllMain\\DllMain.h"
#include "..\\DllMain\\\GameInfo.h"
#include "Character.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <Strsafe.h>
// #include "logwriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT WM_HOOK_WRITE = RegisterWindowMessage(_T("WM_HOOK_WRITE"));
const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));

// CSendAppDlg dialog
HHOOK hhk;

CSendAppDlg::CSendAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendAppDlg::IDD, pParent)
	, m_ixPos(0)
	, m_iYPos(0)
	, m_iX1(0)
	, m_iY1(0)
	, m_iX2(0)
	, m_iY2(0)
	, m_sPhai(_T(""))
	, m_iIdSkill(0)
	, m_iIdLeftSkill(0)
	, m_iIndexNPC(0)
	, m_iCurXPos(0)
	, m_iCurYPos(0)
	, m_iRadius(2000)
	, m_bAutoTrain(false)
	, m_sAutoTrainLabel(_T(""))
	, m_iXNext(0)
	, m_iYNext(0)
	, m_sNameNext(_T(""))
	, m_iPosNext(0)
	, m_cbPKPlayer(FALSE)
	, m_cbKillMonster(FALSE)
	, m_iEnemyHP(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRunning = 0;
	m_hVLWin = 0;
	m_hProcess = 0;
	nTimerDelay = 0;	
}

void CSendAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SKILL, m_cbSkill);
	DDX_Control(pDX, IDC_COMBO_USESKILL, m_cbUseSkill);
	DDX_Text(pDX, IDC_EDIT_X, m_ixPos);
	DDX_Text(pDX, IDC_EDIT_Y, m_iYPos);
	DDX_Text(pDX, IDC_EDIT_X1, m_iX1);
	DDX_Text(pDX, IDC_EDIT2_Y1, m_iY1);
	DDX_Text(pDX, IDC_EDIT_X2, m_iX2);
	DDX_Text(pDX, IDC_EDIT_Y2, m_iY2);

	DDX_Text(pDX, IDC_STATIC_PHAI, m_sPhai);
	DDX_Text(pDX, IDC_EDIT_IDSKILL, m_iIdSkill);
	DDX_Text(pDX, IDC_EDIT_IDLEFTSKILL, m_iIdLeftSkill);
	DDX_Text(pDX, IDC_EDIT_INDEXNPCARRAY, m_iIndexNPC);
	DDX_Text(pDX, IDC_EDIT_CurXPos, m_iCurXPos);
	DDX_Text(pDX, IDC_EDIT_CurYPos, m_iCurYPos);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_iRadius);
	DDV_MinMaxInt(pDX, m_iIdSkill, 0, INT_MAX);
	DDX_Text(pDX, IDC_STATIC_AUTOTRAIN, m_sAutoTrainLabel);
	DDX_Text(pDX, IDC_EDIT_XNEXT, m_iXNext);
	DDX_Text(pDX, IDC_EDIT_YNEXT, m_iYNext);
	DDX_Text(pDX, IDC_EDIT_NAMEENEMY, m_sNameNext);
	DDX_Text(pDX, IDC_EDIT_POSNEXT, m_iPosNext);	
	DDX_Control(pDX, IDC_LIST_MONSTERLST, m_lstMonster);
	DDX_Check(pDX, IDC_CHECK_PK, m_cbPKPlayer);
	DDX_Check(pDX, IDC_CHECK_PKMONSTER, m_cbKillMonster);
	DDX_Control(pDX, IDC_BUTTON_AUTOTRAIN, m_btnAutoTrain);
	DDX_Control(pDX, IDC_BUTTON_CURRENTPOS, m_btnCurrentPos);
	DDX_Text(pDX, IDC_EDIT_ENEMYHP, m_iEnemyHP);
}

BEGIN_MESSAGE_MAP(CSendAppDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSendAppDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSendAppDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDCANCEL, &CSendAppDlg::OnBnClickedCancel)
		
	ON_BN_CLICKED(IDC_BUTTON_SETLEFTSKILL, &CSendAppDlg::OnBnClickedButtonSetleftskill)
	ON_BN_CLICKED(IDC_BUTTON_USESKILL, &CSendAppDlg::OnBnClickedButtonUseskill)
	ON_BN_CLICKED(IDC_BUTTON_GOTO, &CSendAppDlg::OnBnClickedButtonGoto)
	ON_BN_CLICKED(IDC_BUTTON_AUTOPK, &CSendAppDlg::OnBnClickedButtonAutopk)
		
	ON_BN_CLICKED(IDC_BUTTON_SPLIT, &CSendAppDlg::OnBnClickedButtonSplit)
	ON_BN_CLICKED(IDC_BUTTON_NOICHUOI, &CSendAppDlg::OnBnClickedButtonNoichuoi)
	ON_BN_CLICKED(IDC_BUTTON_USELEFTSKILL, &CSendAppDlg::OnBnClickedButtonUseleftskill)
	ON_BN_CLICKED(IDC_BUTTON_FOLLOWPEOPLE, &CSendAppDlg::OnBnClickedButtonFollowpeople)
	ON_BN_CLICKED(IDC_BUTTON_CURRENTPOS, &CSendAppDlg::OnBnClickedButtonCurrentpos)
	ON_BN_CLICKED(IDC_BUTTON_AUTOTRAIN, &CSendAppDlg::OnBnClickedButtonAutotrain)
	ON_BN_CLICKED(IDC_BUTTON_SENDSTRUCTURE, &CSendAppDlg::OnBnClickedButtonSendstructure)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_MONSTERLST, &CSendAppDlg::OnNMCustomdrawListMonsterlst)
END_MESSAGE_MAP()

// CSendAppDlg message handlers

BOOL CSendAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// Set Skill ---------
	/*CString strArry[3] = {L"Hạn Địa Lôi", L"Thiên Phong Địa Nhân", L"Phong Lâm Hỏa Sơn"};		
	for (int i=0; i<3; i++)
		m_cbSkill.InsertString(i, strArry[i]);	
	m_cbSkill.SetCurSel(0);*/

	// Use Skill --------
	CString strSkill[7] = {L"Khai Sơn Trảm", L"Điện Quan Trảm", L"Tinh Thông Trường Đao", L"Tam Đầu Lục Thủ", L"Hỏa Quang Trảm", L"Lạc Địa Trảm", L"Khuynh Thành Nhất Kích"};
	for (int i=0; i<7; i++)
	{
		m_cbUseSkill.InsertString(i, strSkill[i]);
		m_cbSkill.InsertString(i, strSkill[i]);	
	}
	
	m_cbUseSkill.SetCurSel(0);
	m_cbSkill.SetCurSel(0);

	m_lstMonster.SetExtendedStyle(m_lstMonster.GetStyle() | LVS_EX_FULLROWSELECT);
	m_lstMonster.InsertColumn(0,L"Pos",LVCFMT_LEFT,35);
	m_lstMonster.InsertColumn(1,L"Name",LVCFMT_LEFT,70);
	m_lstMonster.InsertColumn(2,L"x",LVCFMT_LEFT,50);	
	m_lstMonster.InsertColumn(3,L"y",LVCFMT_LEFT,50);	
	m_lstMonster.InsertColumn(4,L"Dist",LVCFMT_LEFT,50);

	
	SetTimer(ID_TIMER_FINDMONSTER, INTERVAL_FIND_MONSTER, NULL);	// set skill

	//g_LogWriter.Initialize(_T("D:\\logs\\"), LOG_TYPE_FATAL | ..., L"Debug Info", TRUE); // thiết lập log


	// Set tooltip
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSendAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSendAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// set hook
void CSendAppDlg::OnBnClickedButton1()
{
   if (!m_bRunning) {
	   	   	   
	   // CString sClassName = L"FSOnline Class";	   
	   // CString sWindowName = L"Calculator";
	   // m_hVLWin = ::FindWindow(NULL, sWindowName);

	   CString sClassName = L"FSOnline Class";
	   m_hVLWin = ::FindWindow(sClassName, NULL);

		if (!m_hVLWin)
			return;
		DWORD dwProcessID;
		GetWindowThreadProcessId(m_hVLWin, &dwProcessID);
		m_hProcess = OpenProcess(PROCESS_VM_READ|PROCESS_ALL_ACCESS, FALSE, dwProcessID);
		if (!m_hProcess)
			return;
		if (InjectDll(m_hVLWin))
		{
			SetWindowTextW(L"Inject Succeed! Ready to call ASM ...");
		}
		
   }
	else {
		UnmapDll(m_hVLWin);
		CloseHandle(m_hProcess);
	}
	m_bRunning = !m_bRunning;
}

void CSendAppDlg::OnBnClickedButton2()
{
	// sendMessage(hWnd, MESSAGE, Wparam, lparam)
	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_USEITEM, 1 );
	//wparam = 1 =>
}

void CSendAppDlg::OnBnClickedCancel()
{
	UnmapDll(m_hVLWin);
	CloseHandle(m_hProcess);
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CSendAppDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	UnmapDll(m_hVLWin);
	CloseHandle(m_hProcess);
	CDialog::OnClose();
}

void CSendAppDlg::OnBnClickedButtonSetleftskill()
{
	// TODO: Add your control notification handler code here
	const int MAX = 7;

	//CString strSkill[7] = {L"Khai Sơn Trảm", L"Điện Quan Trảm", L"Tinh Thông Trường Đao", L"Tam Đầu Lục Thủ", L"Hỏa Quang Trảm", L"Lạc Địa Trảm", L"Khuynh Thành Nhất Kích"};
	int iSkillIndex[MAX] = {29, 31, 34, 35, 37, 39, 42};

	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_SETLEFTSKILL, (int)iSkillIndex[m_cbSkill.GetCurSel()] );
}

void CSendAppDlg::OnBnClickedButtonUseskill()
{
	// TODO: Add your control notification handler code here

	const int MAX = 7;
	int iSkillIndex[MAX] = {29, 31, 34, 35, 37, 39, 42};

	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_USESKILL, (int)iSkillIndex[m_cbUseSkill.GetCurSel()] );	
}

void CSendAppDlg::OnBnClickedButtonGoto()
{
	// TODO: Add your control notification handler code here
	POINTS pt;
	UpdateData(TRUE);
	pt.x = m_ixPos;
	pt.y = m_iYPos;	
	
	//::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_RUNTO, MAKELPARAM(pt.x, pt.y));
	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_SENDCLIENTCMDRUN, MAKELPARAM(pt.x, pt.y));
}

void CSendAppDlg::OnBnClickedButtonAutopk()
{
	// TODO: Add your control notification handler code here
	
	// Demo truyền nhiều giá tri (SendMessage)	
	wchar_t *wData;
	wData = new	wchar_t[10];
	memset(wData, 0x0, 10);
	wData = L"18:20:25";
	COPYDATASTRUCT info = { 2, _tcsclen(wData) * sizeof(wData[0]) + 1, wData};
	::SendMessage(m_hVLWin, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&info));	// send chuỗi qua process 
	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_AUTOPK, 1);					// call lệnh autoPK
}

// --------------------------------------------------------------
// Search target
// --------------------------------------------------------------

CCharacter CSendAppDlg::SearchTargetPos(CString sTargetName)
{	
	CCharacter character;

	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	
	if (!hwnd)
		return character;

	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read
	if (!phandle)	
		return character;

	DWORD dwNpcBaseAddress;
	ReadProcessMemory(phandle, (void*)NPC_BASEADDRESS, &dwNpcBaseAddress, (DWORD)sizeof(dwNpcBaseAddress), 0);	
		
	for (int i=1; i<NPC_MAX; i++)
	{
		CCharacter character;	

		// Character Name;		
		DWORD dwBaseAddressName = dwNpcBaseAddress + NPC_OFFSET_B5C0 * i + NPC_OFFSET_15D0;		

		char sName[255];
		memset(sName, 0x0, sizeof(sName));
		ReadProcessMemory(phandle, (void*)dwBaseAddressName, &sName, (DWORD)sizeof(sName), 0);
		character.m_sName = (CString)sName;

		// Current XPos;
		DWORD dwCurXPos = dwBaseAddressName - 0x00000054;
		ReadProcessMemory(phandle, (void*)dwCurXPos, &character.m_iCurXPosPixel, (DWORD)sizeof(character.m_iCurXPosPixel), 0);

		// Current YPos;
		DWORD dwCurYPos = dwBaseAddressName - 0x00000050;
		ReadProcessMemory(phandle, (void*)dwCurYPos, &character.m_iCurYPosPixel, (DWORD)sizeof(character.m_iCurYPosPixel), 0);

		// Current HP
		DWORD dwHP = dwBaseAddressName - 0x000004AC;
		ReadProcessMemory(phandle, (void*)dwHP, &character.m_iHP, (DWORD)sizeof(character.m_iHP), 0);

		if (sTargetName == character.m_sName)
			return character;	// tìm thấy target
	}

	CloseHandle(phandle);	
	return character;
}

double CSendAppDlg::Distance(int x1, int y1, int x2, int y2)
{	
	double rel = sqrtf ( (x2-x1) * (x2-x1) + (y2-y1) * (y2-y1) );
	return rel;
}

CCharacter CSendAppDlg::GetCurrentPlayerInfo(void)
{
	CCharacter character;

	DWORD dwNpcBaseAddress;
	ReadProcessMemory(m_hProcess, (void*)NPC_BASEADDRESS, &dwNpcBaseAddress, (DWORD)sizeof(dwNpcBaseAddress), 0);	

	int i=1; 
	
	// Character Name;		
	DWORD dwBaseAddressName = dwNpcBaseAddress + NPC_OFFSET_B5C0 * i + NPC_OFFSET_15D0;		
	character.m_iID = 1;

	char sName[255];
	memset(sName, 0x0, sizeof(sName));
	ReadProcessMemory(m_hProcess, (void*)dwBaseAddressName, &sName, (DWORD)sizeof(sName), 0);
	character.m_sName = (CString)sName;

	// Current XPos;
	DWORD dwCurXPos = dwBaseAddressName - 0x00000054; // + 0x00000018;
	ReadProcessMemory(m_hProcess, (void*)dwCurXPos, &character.m_iCurXPosPixel, (DWORD)sizeof(character.m_iCurXPosPixel), 0);
	character.m_iCurXPosMap = character.m_iCurXPosPixel / 256;

	// Current YPos;
	DWORD dwCurYPos = dwBaseAddressName - 0x00000050; // + 0x00000018;
	ReadProcessMemory(m_hProcess, (void*)dwCurYPos, &character.m_iCurYPosPixel, (DWORD)sizeof(character.m_iCurYPosPixel), 0);
	character.m_iCurYPosMap = character.m_iCurYPosPixel / 512;

	// Current HP
	DWORD dwHP = dwBaseAddressName - 0x000004AC;
	ReadProcessMemory(m_hProcess, (void*)dwHP, &character.m_iHP, (DWORD)sizeof(character.m_iHP), 0);

	return character;
}

// Get môn phái
DWORD CSendAppDlg::GetSectarian(void)
{
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	

	if (!hwnd)	
	{
		MessageBox(_T("Window FS not found"), _T("Message"), MB_OK | MB_ICONINFORMATION);		
		return 0x0;
	}

	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ,   0, pid); // Get permission to read
	if (!phandle)	
	{
		MessageBox(_T("Could not get handle"));
		return 0x0;
	}

	DWORD dwNpcBaseAddress;
	ReadProcessMemory(phandle, (void*)NPC_BASEADDRESS, &dwNpcBaseAddress, (DWORD)sizeof(dwNpcBaseAddress), 0);	

	// Current Player = phần tử thứ nhất
	DWORD dwBaseAddressName = dwNpcBaseAddress + NPC_OFFSET_B5C0 * 1 + NPC_OFFSET_15D0;		

	// Môn phái
	DWORD dwSectarian = dwBaseAddressName + 0x00000068;
	DWORD dwValue;
	ReadProcessMemory(phandle, (void*)dwSectarian, &dwValue, (DWORD)sizeof(dwValue), 0);
	
	CloseHandle(phandle);
	return dwValue;
}

void CSendAppDlg::CatChuoi(wchar_t* s)
{	
	wchar_t sSource[20] = L"";	
	wcscpy(sSource, s);
	wchar_t* token = wcstok(sSource, L":");
	while (token)
	{				
		CString str;
		str.Format(L"%s", token);
		MessageBox(str, L"Info", MB_OK);
		token = wcstok(NULL, L":");
	}
}

void CSendAppDlg::OnBnClickedButtonSplit()
{
	// TODO: Add your control notification handler code here
	CatChuoi(L"12:34:56:78");
}

void CSendAppDlg::NoiChuoi(CString str1, CString str2)
{
	/*CString message(_T("Hello there"));
	const wchar_t *messageArray = static_cast<const wchar_t *>(message);
	const wchar_t firstCharacter = message[0];*/

	//const wchar_t *messageArray = static_cast<const wchar_t *>(message);
	const wchar_t *wcstr1 = static_cast<const wchar_t *>(str1);
	const wchar_t *wcstr2 = static_cast<const wchar_t *>(str2);

	wchar_t *wcstr3 = new wchar_t[200];
	memset(wcstr3, 0x0, 200);

	wcscat (wcstr3, wcstr1);
	wcscat (wcstr3, wcstr2);

	CString str;
	str.Format(L"%s", wcstr3);
	MessageBox(str, L"Info", MB_OK);
}

void CSendAppDlg::OnBnClickedButtonNoichuoi()
{
	// TODO: Add your control notification handler code here
	//NoiChuoi(L"Anh Là ai", L" - yes?");
}

void CSendAppDlg::OnBnClickedButtonFollowpeople()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_FOLLOWPEOPLE, m_iIdLeftSkill);
}

void CSendAppDlg::OnBnClickedButtonUseleftskill()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);	
	::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_LOCKSOMEONEUSESKILL, m_iIndexNPC);
}

void CSendAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (!m_bRunning)
		return;	
		
	// Version 1.0
	//if (m_bAutoTrain)	
	//{
	//	UpdateData(TRUE);

	//	CCharacter character = GetCurrentPlayerInfo();	
	//
	//	CArray<CCharacter, CCharacter&> arrEnemies;
	//	GetMonsterIndexInRadius(character, m_iRadius, arrEnemies);	
	//	DisplayEnemyInfo(arrEnemies);
	//	
	//	// int iPos = character.GetIndexOfMinDistance(arrEnemies);
	//	CCharacter nearestEnemy = character.GetIndexOfMinDistance(arrEnemies);

	//	DisplayNextCharacter(arrEnemies, nearestEnemy.m_iID);

	//	//srand(time(NULL));
	//	//int iPos = (rand() % 20)+1;

	//	if (nearestEnemy.m_iID > 1) 
	//	{				
	//		CString strTemp;
	//		strTemp.Format(L"%d:%d", nearestEnemy.m_iID, m_iIdSkill);
	//		const wchar_t *wcstrTemp = static_cast<const wchar_t *>(strTemp);

	//		wchar_t *wData;
	//		const int LEN = 8;
	//		
	//		wData = new	wchar_t[LEN];
	//		memset(wData, 0x0, LEN);
	//		wcscat(wData, wcstrTemp);

	//		COPYDATASTRUCT info = { WM_LOCKSOMEONEUSESKILL, _tcsclen(wData) * sizeof(wData[0]) + 1, wData};
	//		//::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_FOLLOWPEOPLE, nearestEnemy.m_iID);
	//		::SendMessage(m_hVLWin, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&info));		// info->lpData	= L"Pos:Index";
	//		::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_LOCKSOMEONEUSESKILL, 1);					
	//	}
	//	UpdateData(FALSE);
	//}


	//////////////////////////////////////////////////////////////////////////

	UpdateData(TRUE);
	CCharacter character;
	CArray<CCharacter, CCharacter&> arrEnemies;	

	switch (nIDEvent)
	{
		case ID_TIMER_DELAY:	// sau khoảng thời gian không đánh được target, ta tìm target khác
			if (m_bAutoTrain)	
			{
				character = GetCurrentPlayerInfo();					
				GetMonsterIndexInRadius(character, m_iRadius, arrEnemies);	
				character.DistanceAscending(arrEnemies);	// sắp tăng dần để debug cho dễ
				DisplayEnemyInfo(character, arrEnemies);		
				nearestEnemy = character.GetIndexOfMinDistance(arrEnemies);
				DisplayNextCharacter(arrEnemies, nearestEnemy.m_iID);

				currentMonster = nearestEnemy;

				KillTimer(ID_TIMER_DELAY);
				nTimerDelay = 0;

				//g_LogWriter.WriteLog(LOG_TYPE_ERROR, L"ID_TIMER_DELAY DeActived ......");
			}
			break;

		case ID_TIMER_FINDMONSTER:			
			if (m_bAutoTrain)	
			{				
				if (nearestEnemy.m_iHP == 0)	// đã kill thành công monster này, search monster khác
				{
					character = GetCurrentPlayerInfo();						
					GetMonsterIndexInRadius(character, m_iRadius, arrEnemies);	
					character.DistanceAscending(arrEnemies);	// sắp tăng dần để debug cho dễ
					DisplayEnemyInfo(character, arrEnemies);		
					nearestEnemy = character.GetIndexOfMinDistance(arrEnemies);
					DisplayNextCharacter(arrEnemies, nearestEnemy.m_iID);		
				}
				else
				{
					if (nearestEnemy.m_iID < 2)
						return;
					
					CString strTemp;
					strTemp.Format(L"%d:%d", nearestEnemy.m_iID, m_iIdSkill);
					const wchar_t *wcstrTemp = static_cast<const wchar_t *>(strTemp);

					wchar_t *wData;
					const int LEN = 8;

					wData = new	wchar_t[LEN];
					memset(wData, 0x0, LEN);
					wcscat(wData, wcstrTemp);

					COPYDATASTRUCT info = { WM_LOCKSOMEONEUSESKILL, _tcsclen(wData) * sizeof(wData[0]) + 1, wData};		
					::SendMessage(m_hVLWin, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&info));		// info->lpData	= L"Pos:Index";
					::SendMessage(m_hVLWin, WM_HOOK_WRITE, WPARAM_LOCKSOMEONEUSESKILL, 1);		

					// cập nhật lại thông tin, để biết enemy được giết hay chưa???
					//////////////////////////////////////////////////////////////////////////
					DWORD dwNpcBaseAddress;
					ReadProcessMemory(m_hProcess, (void*)NPC_BASEADDRESS, &dwNpcBaseAddress, (DWORD)sizeof(dwNpcBaseAddress), 0);

					// Character Name;		
					DWORD dwBaseAddressName = dwNpcBaseAddress + NPC_OFFSET_B5C0 * nearestEnemy.m_iID + NPC_OFFSET_15D0;		

					// HP
					DWORD dwHP = dwBaseAddressName - 0x000004AC;
					ReadProcessMemory(m_hProcess, (void*)dwHP, &nearestEnemy.m_iHP, (DWORD)sizeof(nearestEnemy.m_iHP), 0);

					//////////////////////////////////////////////////////////////////////////

					//g_LogWriter.WriteLogFormat(LOG_TYPE_DEBUG, L"nTimerDelay = %d", nTimerDelay);
									
					if (nearestEnemy.m_iHP == currentMonster.m_iHP)	// đánh không trúng target hay bị chặn bởi chướng ngại vật (núi,..)
					{
						if (nTimerDelay == 0)	// chưa set timer delay
						{							
							nTimerDelay = SetTimer(ID_TIMER_DELAY, INTERVAL_TIMER_DELAY, NULL);							
							//g_LogWriter.WriteLog(LOG_TYPE_FATAL, L"ready to Set Timer delay ...");
						}
					}
					else	// đang đánh quái này
					{
						if (nTimerDelay)
						{
							KillTimer(nTimerDelay);	// không set nữa
							nTimerDelay = 0;
						}
						currentMonster = nearestEnemy;
					}
				}				
			}

			break;
	}

	
	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}

void CSendAppDlg::GetMonsterIndexInRadius(CCharacter character, double iRadus, CArray<CCharacter, CCharacter&> &arrEnemies)
{	
	if (!m_hProcess)	
		return;

	DWORD dwNpcBaseAddress;
	ReadProcessMemory(m_hProcess, (void*)NPC_BASEADDRESS, &dwNpcBaseAddress, (DWORD)sizeof(dwNpcBaseAddress), 0);
	
	int k = 0;
	for (int i=2; i<NPC_MAX; i++)
	{
		CCharacter enemy;	
		enemy.m_iID = i;

		// Character Name;		
		DWORD dwBaseAddressName = dwNpcBaseAddress + NPC_OFFSET_B5C0 * i + NPC_OFFSET_15D0;		

		char sName[255];
		memset(sName, 0x0, sizeof(sName));
		ReadProcessMemory(m_hProcess, (void*)dwBaseAddressName, &sName, (DWORD)sizeof(sName), 0);
		enemy.m_sName = (CString)sName;

		if (m_cInitChar.m_sName.Compare(enemy.m_sName) == 0 || enemy.m_sName == L"")	// trường hợp trùng tên và tên rỗng
			continue;
		
		// Current XPos;
		DWORD dwCurXPos = dwBaseAddressName - 0x00000054;
		ReadProcessMemory(m_hProcess, (void*)dwCurXPos, &enemy.m_iCurXPosPixel, (DWORD)sizeof(enemy.m_iCurXPosPixel), 0);
		enemy.m_iCurXPosMap = enemy.m_iCurXPosPixel / 256;

		// Current YPos;
		DWORD dwCurYPos = dwBaseAddressName - 0x00000050;
		ReadProcessMemory(m_hProcess, (void*)dwCurYPos, &enemy.m_iCurYPosPixel, (DWORD)sizeof(enemy.m_iCurYPosPixel), 0);
		enemy.m_iCurYPosMap = enemy.m_iCurYPosPixel / 512;

		// Phái;
		DWORD dwPhai = dwBaseAddressName + 0x00000068;
		ReadProcessMemory(m_hProcess, (void*)dwPhai, &enemy.m_dwPhai, (DWORD)sizeof(enemy.m_dwPhai), 0);
		
		// Current HP;
		DWORD dwCurrentHP = dwBaseAddressName - 0x000004AC;
		ReadProcessMemory(m_hProcess, (void*)dwCurrentHP, &enemy.m_iHP, (DWORD)sizeof(enemy.m_iHP), 0);

		POINT point;
		point.x = m_cInitChar.m_iCurXPosPixel;
		point.y = m_cInitChar.m_iCurYPosPixel;		

		if (m_cbPKPlayer == TRUE && m_cbKillMonster == FALSE)	// checked -> Pk with player only
		{
			if ( (CheckPointBelongCircle(point, m_iRadius, enemy) == 1) && IsPlayer(enemy) == 1)
				arrEnemies.Add(enemy);			
		}
		else if (m_cbKillMonster == TRUE && m_cbPKPlayer == FALSE)	
		{
			if ( (CheckPointBelongCircle(point, m_iRadius, enemy) == 1) && IsPlayer(enemy) == 0)
				arrEnemies.Add(enemy);			
		}
		else			// not checked --> kill all monster and NPC
		{
			if ( (CheckPointBelongCircle(point, m_iRadius, enemy) == 1) && IsPlayer(enemy) == 0)
				arrEnemies.Add(enemy);			
		}
	}
}

void CSendAppDlg::OnBnClickedButtonCurrentpos()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CCharacter character = GetCurrentPlayerInfo();	

	m_iCurXPos = character.m_iCurXPosPixel;
	m_iCurYPos = character.m_iCurYPosPixel;

	m_iX1 = character.m_iCurXPosPixel / 256;
	m_iY1 = character.m_iCurYPosPixel / 512;
	UpdateData(FALSE);
}

double CSendAppDlg::Distance(POINT pPoint, CCharacter character)
{
	return sqrtf(	(pPoint.x - character.m_iCurXPosPixel) *  (pPoint.x - character.m_iCurXPosPixel) + 
					(pPoint.y - character.m_iCurYPosPixel) * (pPoint.y - character.m_iCurYPosPixel) );
	
}


void CSendAppDlg::OnBnClickedButtonAutotrain()
{
	// TODO: Add your control notification handler code here
	m_bAutoTrain = !m_bAutoTrain;

	UpdateData(TRUE);

	if (m_bAutoTrain)
		m_sAutoTrainLabel = L"AutoTrain - Enable";
	else
		m_sAutoTrainLabel = L"AutoTrain - Disable @@@@-@@@@";

	// tính khoảng cách của vị trí nhân vật hiện tại so với bán kính // áp dụng trường hợp ngoài thành (đang chiến đấu)
	/*do 
	{*/
		m_cInitChar = GetCurrentPlayerInfo();

		CArray<CCharacter, CCharacter&> arrEnemies;
		GetMonsterIndexInRadius(m_cInitChar, m_iRadius, arrEnemies);	
		m_cInitChar.DistanceAscending(arrEnemies);	// sắp tăng dần để debug cho dễ
		DisplayEnemyInfo(m_cInitChar, arrEnemies);
	
		nearestEnemy = m_cInitChar.GetIndexOfMinDistance(arrEnemies);
		DisplayNextCharacter(arrEnemies, nearestEnemy.m_iID);
	//} while (nearestEnemy.m_iID < 2);	
	
	nTimerDelay = 0;
	UpdateData(FALSE);
}

void CSendAppDlg::OnBnClickedButtonSendstructure()
{
	// TODO: Add your control notification handler code here

	COPYDATASTRUCT MyCDS;
	Locksomeoneuseskill ptr;
	ptr.iIndex = 168;
	ptr.iPos = 29;
	HRESULT hResult = StringCbCopy(ptr.s, sizeof(ptr.s), L"Good luck~");
	if (hResult != S_OK)
		return;

	MyCDS.dwData = WM_SENDSTRUCTURE;
	MyCDS.cbData = sizeof(ptr);
	MyCDS.lpData = &ptr;

	::SendMessage(m_hVLWin, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&MyCDS));	// send struct qua process 
}

void CSendAppDlg::DisplayNextCharacter(const CArray<CCharacter, CCharacter&> &Characters, int pos)
{
	DWORD dwNpcBaseAddress;
	ReadProcessMemory(m_hProcess, (void*)NPC_BASEADDRESS, &dwNpcBaseAddress, (DWORD)sizeof(dwNpcBaseAddress), 0);
	
	CCharacter enemy;	
	
	// Character Name;		
	DWORD dwBaseAddressName = dwNpcBaseAddress + NPC_OFFSET_B5C0 * pos + NPC_OFFSET_15D0;		
	m_iPosNext = pos;

	char sName[255];
	memset(sName, 0x0, sizeof(sName));
	ReadProcessMemory(m_hProcess, (void*)dwBaseAddressName, &sName, (DWORD)sizeof(sName), 0);
	enemy.m_sName = (CString)sName;
	m_sNameNext = enemy.m_sName;

	// Current XPos;
	DWORD dwCurXPos = dwBaseAddressName - 0x00000054;
	ReadProcessMemory(m_hProcess, (void*)dwCurXPos, &enemy.m_iCurXPosPixel, (DWORD)sizeof(enemy.m_iCurXPosPixel), 0);
	enemy.m_iCurXPosMap = enemy.m_iCurXPosPixel / 256;
	m_iXNext = enemy.m_iCurXPosPixel;
	m_iX2 = enemy.m_iCurXPosMap;

	// Current YPos;
	DWORD dwCurYPos = dwBaseAddressName - 0x00000050;
	ReadProcessMemory(m_hProcess, (void*)dwCurYPos, &enemy.m_iCurYPosPixel, (DWORD)sizeof(enemy.m_iCurYPosPixel), 0);
	enemy.m_iCurYPosMap = enemy.m_iCurYPosPixel / 512;
	m_iYNext = enemy.m_iCurYPosPixel;
	m_iY2 = enemy.m_iCurYPosMap;

	// HP
	DWORD dwHP = dwBaseAddressName - 0x000004AC;
	ReadProcessMemory(m_hProcess, (void*)dwHP, &enemy.m_iHP, (DWORD)sizeof(enemy.m_iHP), 0);
	m_iEnemyHP = enemy.m_iHP;
}

void CSendAppDlg::ListControlAddData(CListCtrl &ctrl, int row, int col,CString str)
{
	LVITEM lv;
	lv.iItem = row;
	lv.iSubItem = col;
	lv.pszText =  (LPTSTR) (LPCTSTR)str;
	lv.mask = LVIF_TEXT;
	if(col == 0)
		ctrl.InsertItem(&lv);
	else
		ctrl.SetItem(&lv);  
}


void CSendAppDlg::DisplayEnemyInfo(CCharacter character, const CArray<CCharacter, CCharacter&> &Characters)
{
	m_lstMonster.DeleteAllItems();

	CString str;

	for (int i=0; i<Characters.GetCount(); i++)
	{
		// Character Name;		
		str.Format(L"%d", Characters[i].m_iID);		
		ListControlAddData(m_lstMonster, i, 0, str);
		ListControlAddData(m_lstMonster, i, 1, Characters[i].m_sName);

		// Current XPos;					
		str.Format(L"%d", Characters[i].m_iCurXPosPixel);
		ListControlAddData(m_lstMonster, i, 2, str);

		// Current YPos;
		str.Format(L"%d", Characters[i].m_iCurYPosPixel);
		ListControlAddData(m_lstMonster, i, 3, str);

		// k/c
		double kc = character.Distance(Characters[i]);
		str.Format(L"%f", kc);
		ListControlAddData(m_lstMonster, i, 4, str);
	
	}
}

// xác định một điểm nằm trong vòng tròn
//  1: nằm trong hình tròn
// -1: nằm ngoài hình tròn
int CSendAppDlg::CheckPointBelongCircle(POINT point, int radius, const CCharacter& Character)
{
	// tâm I(x,y),ban kình r, diem A(x1,y1)
	// (x1-x)^2+(y1-y)^2 - r^2
	// >0 nam ngoai duong tron
	// <=0 nam trong duong tron

	int xA = Character.m_iCurXPosPixel - point.x;
	int yA = Character.m_iCurYPosPixel - point.y;

	int IA =  ((xA * xA) + (yA * yA)) - (radius * radius);
	return (IA > 0 ? -1 : 1);
}

//void CSendAppDlg::OnBnClickedCheck1()
//{
//	// TODO: Add your control notification handler code here
//}

int CSendAppDlg::IsPlayer(const CCharacter& character)
{
	
	if (character.m_dwPhai == DAOSI)
		return 1;
	if (character.m_dwPhai == GIAPSI)
		return 1;
	if (character.m_dwPhai == DINHAN)
		return 1;

	return 0;
}

void CSendAppDlg::OnNMCustomdrawListMonsterlst(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	//COLORREF BackColor;
	//DWORD_PTR dwRow;
	//posFormat = 0;

	//switch(pNMCD ->nmcd.dwDrawStage)
	//{	
	//case CDDS_ITEMPREPAINT:
	//	//Set background
	//	dwRow = pNMCD->nmcd.dwItemSpec;

	//	BackColor = RGB(219, 219, 238);
	//	if (dwRow == posFormat)
	//		pNMCD->clrTextBk = BackColor;
	//
	//	break;

	//default:
	//	*pResult = 0;
	//}
}
