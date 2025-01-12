class CCharacter
{
public:	
	int m_iHP;				// HP hiện tại của nhân vật
	int m_iMP;				// MP hiện tại của nhân vật	
	int m_iMaxHP;			// HP Max của nhân vật
	int m_iMaxMP;			// MP Max của nhân vật
	int m_iCurXPosPixel;	// vị trí x hiện tại (tính bằng pixels)
	int m_iCurYPosPixel;	// vị trí y hiện tại (tính bằng pixels)
	int m_iCurXPosMap;		// vị trí x hiện tại (tính trên map)
	int m_iCurYPosMap;		// vị trí y hiện tại (tính trên map)
	int m_iID;				// index của nhân vật trong mảng NPC

	DWORD m_dwPhai;			// phái nhân vật
	CString m_sName;		// tên nhân vật
	HANDLE m_hHandle;		// handle đang xét, quan trọng, dùng để đọc thông tin

	CCharacter();
	~CCharacter(void);
	CCharacter GetIndexOfMinDistance(const CArray<CCharacter, CCharacter&> &Characters);	
	
	CString ReadCharacterName();

	void GetCurrentPlayerInfo(void);
	void GetMonsterIndexInRadius(int radius, CArray<CCharacter, CCharacter&>& arrEnemies, int type);
	void GetCurrentPosMap(void);

	DWORD GetPlayerType(void);
	
	double Distance(const CCharacter& character);
	
	int CheckPointBelongCircle(POINT point, int radius, const CCharacter& enemy);
	int IsPlayer(const CCharacter& character);
		
	
	void GetHP(void);
	int GetSkillIDFromName(const CString& name);
	
};
