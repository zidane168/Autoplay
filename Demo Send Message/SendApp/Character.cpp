#include "StdAfx.h"
#include "Character.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

CCharacter::CCharacter()
: m_iHP(0)
, m_iMP(0)
, m_iMaxHP(0)
, m_iMaxMP(0)
, m_iCurXPosPixel(0)
, m_iCurYPosPixel(0)
, m_iCurXPosMap(0)
, m_iCurYPosMap(0)
, m_iID(0)
, m_dwPhai(0)
{
	m_sName = "";
}

CCharacter::~CCharacter(void)
{
}

double CCharacter::Distance(const CCharacter &character)
{
	double rel = sqrtf (  (m_iCurXPosPixel - character.m_iCurXPosPixel) * (m_iCurXPosPixel- character.m_iCurXPosPixel) 
						+ (m_iCurYPosPixel - character.m_iCurYPosPixel) * (m_iCurYPosPixel - character.m_iCurYPosPixel));
	return rel;
}

CCharacter CCharacter::GetIndexOfMinDistance(const CArray<CCharacter, CCharacter&> &Characters)
{
	CCharacter character;
	
	if (Characters.GetCount() == 0)
		return character;		// character.m_iID = 0;

	double min = Distance(Characters[0]);
	double distance;
	character = Characters[0];	

	for (int i=1; i < Characters.GetCount(); i++)
	{		
		distance = Distance(Characters[i]); 
		if (min > distance)
		{
			min = distance;
			character = Characters[i];
		}
	}

	return character;
}

CCharacter CCharacter::GetIndexOfSecMinDistance(CArray<CCharacter, CCharacter&>& Characters)
{
	if (Characters.GetCount() > 1)	// có hai phần tử trở lên
	{
		DistanceAscending(Characters);
		return Characters[1];
	}
	return Characters[0];
}

void CCharacter::Swap(CCharacter& character1, CCharacter& character2)
{
	CCharacter chr;
	chr = character1;
	character1 = character2;
	character2 = chr;
}

void CCharacter::DistanceAscending(CArray<CCharacter, CCharacter&>& Characters)
{
	for (int i=0; i < Characters.GetCount() - 1; i++)
		for (int j=i+1; j < Characters.GetCount(); j++)
			if (Distance(Characters[i]) > Distance(Characters[j]))
				Swap(Characters[i], Characters[j]);
}
