// SendAppDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Character.h"
#include "afxcmn.h"
//#include "LogWriter.h"

// CSendAppDlg dialog
class CSendAppDlg : public CDialog
{
// Construction
public:
	CSendAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SENDAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	HWND m_hVLWin;
	HANDLE m_hProcess;
	BOOL m_bRunning;

public:
	CComboBox m_cbSkill;	
	CComboBox m_cbUseSkill;

	int m_ixPos;
	int m_iYPos;	
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonSetleftskill();
	afx_msg void OnBnClickedButtonGoto();
	afx_msg void OnBnClickedButtonUseskill();
	afx_msg void OnBnClickedButtonAutopk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonNoichuoi();
	afx_msg void OnBnClickedButtonSplit();	
	afx_msg void OnBnClickedButtonUseleftskill();
	afx_msg void OnBnClickedButtonFollowpeople();
	afx_msg void OnBnClickedButtonCurrentpos();
	afx_msg void OnBnClickedButtonAutotrain();

	int m_iX1;
	int m_iY1;
	int m_iX2;
	int m_iY2;
	int m_iIdSkill;
	int m_iIdLeftSkill;
	int m_iIndexNPC;
	int m_iCurXPos;
	int m_iCurYPos;
	int m_iRadius;
	int m_iXNext;
	int m_iYNext;
	CString m_sAutoTrainLabel;	
	CString m_sNameNext;
	CString m_sPhai;
	//double m_dDistance;
	bool m_bAutoTrain;

			
	DWORD GetSectarian(void);

	void CatChuoi(wchar_t s[]);
	void NoiChuoi(CString str1, CString str2);
	void GetMonsterIndexInRadius(CCharacter character, double iRadus, CArray<CCharacter, CCharacter&> &arrEnemies);
	void DisplayNextCharacter(const CArray<CCharacter, CCharacter&> &Characters, int pos);

	CCharacter SearchTargetPos(CString sName);
	CCharacter GetCurrentPlayerInfo(void);

	double Distance(int x1, int y1, int x2, int y2);
	double Distance(POINT pPoint, CCharacter character);	
	afx_msg void OnBnClickedButtonSendstructure();	
	int m_iPosNext;
	void ListControlAddData(CListCtrl &ctrl, int row, int col,CString str);

	void DisplayEnemyInfo(CCharacter, const CArray<CCharacter, CCharacter&> &Characters);
	CListCtrl m_lstMonster;
	// xác định một điểm nằm trong vòng tròn
	int CheckPointBelongCircle(POINT point, int radius, const CCharacter& Character);
	int nTimerDelay;	
	//int posFormat;

	CCharacter m_cInitChar;
	afx_msg void OnBnClickedCheck1();
	BOOL m_cbPKPlayer;
	int IsPlayer(const CCharacter& character);
	BOOL m_cbKillMonster;
	CButton m_btnAutoTrain;
	CButton m_btnCurrentPos;
	CCharacter nearestEnemy;
	CCharacter currentMonster;

	//CLogWriter g_LogWriter;	
	
	afx_msg void OnNMCustomdrawListMonsterlst(NMHDR *pNMHDR, LRESULT *pResult);
	int m_iEnemyHP;
};
