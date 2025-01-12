#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "atlimage.h"
#include "INI.h"
#include "LibraryShared.h"

// TabPK dialog

class CTabPK : public CDialog
{
	DECLARE_DYNAMIC(CTabPK)

public:
	CTabPK(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabPK();

// Dialog Data
	enum { IDD = IDD_TAB_PK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CImageList m_SkillImage;
	CImageList m_lstSkillImage;
	CImage m_pngImage;
	CIniReader m_iniReader;
	CString m_Type;
	int m_Delay;

public:
	//CComboBox m_cmbSkill; - ThongPH - 20130527 - Change to ComboBoxEx with ImageList
	CButton m_btnAddSkill;
	CButton m_btnDelSkill;
	CButton m_btnMoveUp;
	CButton m_btnMoveDown;
	CListCtrl m_lstSelSkill;
	CEdit m_txtDelay;

protected:
	void setImage(CButton* btn, CString img);
	void addData(int row, int col,CString str);
public:
	BOOL LoadSkillFile(CString strFilename);
	void LoadSkillData(CString strType);
	afx_msg void OnBnClickedBtnAddskill();
	afx_msg void OnBnClickedBtnDelskill();
	afx_msg void OnBnClickedBtnMoveup();
	afx_msg void OnBnClickedBtnMovedown();
	void SaveCharacterSkill(CString strName);
	//afx_msg void OnCbnSelchangeCmbSkill(); - ThongPH - 20130527 - Change to ComboBoxEx with ImageList
private:
	void CTabPK::onChange();
public:
	int m_iXPos;
	int m_iYPos;
	int m_iRadius;	
	void LoadCurrentPos(CCharacter);
	CString m_sCurrentChracterName;
	afx_msg void OnBnClickedButtonGetpos();
	CCharacter m_cCurrentCharacter;		// character trung gian để lấy dữ liêu từ bên AutoPlayDlg (GUI) qua
	CButton m_cbPK;
	CButton m_cbMonster;
	CEdit m_txtRadius;
	CComboBoxEx m_cmbeSkill;
	afx_msg void OnCbnSelchangeComboboxexSkill();
};
