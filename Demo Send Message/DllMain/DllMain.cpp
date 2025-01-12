// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DllMain.h"
#include <atlstr.h>
#include "GameInfo.h"

const UINT WM_HOOK_WRITE = RegisterWindowMessage(L"WM_HOOK_WRITE");
const UINT WM_HOOKEX = RegisterWindowMessage(L"WM_HOOKEX_RK");
//
#define pCW ((CWPSTRUCT*)lParam)

#define BASE_ECX_ADDRESS   0x00912B94
#define OFFSET_ECX_ADDRESS 0x0002BAC0

//

WNDPROC				OldWndProc = NULL;
LRESULT CALLBACK	NewWndProc(HWND,UINT,WPARAM,LPARAM);
BOOL bHooked = 0;
HWND ghWnd;

#define WM_MYSTART (WM_USER+1)
#define WM_MYEND (WM_USER+2)

HHOOK hHookKeyBoard;
HINSTANCE hModuleDll;

int iIdSkill = 0;
int iPosition = 0;

/************************************************************************
VịLH - LockSomeOneUseSkill (09/05/2013) (Very good)
	Hàm ra skill với nhân vật khác 
param:
	int: index của mảng NPC
	int: index của skill	
************************************************************************/
void LockSomeOneUseSkill(HANDLE phandle, int iPos, int iIdSkill)
{

	/*
	CPU Disasm
	Address   Hex dump          Command                                  Comments	
	0056403D  |.  8B8424 B00000 MOV EAX,DWORD PTR SS:[ESP+0B0]
	00564044  |.  8B0D 48EF7500 MOV ECX,DWORD PTR DS:[75EF48]            ; PTR to ASCII "`GY"
	0056404A  |.  50            PUSH EAX
	0056404B  |.  8B4424 78     MOV EAX,DWORD PTR SS:[ESP+78]
	0056404F  |.  8B11          MOV EDX,DWORD PTR DS:[ECX]
	00564051  |.  50            PUSH EAX
	00564052  |.  FF52 38       CALL DWORD PTR DS:[EDX+38]               ; -- LockSomeoneUseSkill   CoreClient:CoreShell
	*/

	DWORD dwEcxAddress;		
	ReadProcessMemory(phandle, (void*)0x0075EF48, &dwEcxAddress, (DWORD)sizeof(dwEcxAddress), 0);	
	CloseHandle(phandle);	

	DWORD dwFunctionAddress = 0x00593DD0;
	__asm
	{
		PUSHAD
		PUSHFD		
		PUSH iIdSkill	// para 2		
		PUSH iPos		// para 1
		MOV ECX, dwEcxAddress		
		CALL dwFunctionAddress	
		POPFD
		POPAD
	}
}


/************************************************************************
 Hàm theo sau NPC khác với id mảng tương ứng (Mảng NPC = 1 -> 255)
param:
	int: index của mảng NPC
************************************************************************/
void FollowPeople(int iPos)
{
	//CPU Disasm
	//Address   Hex dump          Command                                  Comments
	//00603D80  |> \50            PUSH EAX                                 ; /Arg1
	//00603D81  |.  8BCD          MOV ECX,EBP                              ; |
	//00603D83  |.  E8 281D0000   CALL 00605AB0                            ; \game.00605AB0

	DWORD dwFunctionAddress = 0x00605AB0;
	DWORD dwEcxAddress		= 0x00DEF058;
	__asm
	{
		PUSHAD
		PUSHFD		
		PUSH iPos					// vị trí quái trong mảng
		MOV ECX, dwEcxAddress		
		CALL dwFunctionAddress	
		POPFD
		POPAD
	}
}


int getInfo()
{	
	//__asm
	//{
		//mov eax,x;
		//mov ebx,y;
		//mul ebx;
		//mov eax, [0x00906B48]
		//mov eax,DWORD PTR DS:[0x007420D0];
		//mov name,eax;

		/*push 1
		mov EAX, DWORD PTR DS:[0x100347C];
		call EAX;	*/			

	// }

	
	//DWORD dwAddress = 0x100347C;
	DWORD dwAddress = 0x006144E0;
	
	__asm
	{
		
		/*PUSH 1
		CALL dwAddress	*/
	
		// best
		//pushad
		//pushfd
		//push 1 // tham so 5
		//push 1 // tham so 4
		//push 0 // tham so 3
		//push 0 // tham so 2
		//push 0x29 //tham so 1
		//mov  ECX, 0x0289008C
		//CALL dwAddress
		//popfd
		//popad

		// Game - Do Mìn
		/*pushad
		pushfd
		PUSH 1
		MOV ECX, 0x006FCC0
		CALL dwAddress
		popfd
		popad*/

		PUSHAD
		PUSHFD
		PUSH 0
		PUSH 0
		MOV ECX, 0x2A2FC74
		CALL dwAddress
		POPFD
		POPAD

	}
	
	return 1;
}

void WriteFile(DWORD dwData)
{
	FILE *f = _wfopen(L"D:\\logs.txt", L"a+");
	wchar_t sz[100];
	wsprintf(sz, L"msg: %x\r\n", dwData);
	fwrite(sz, lstrlen(sz), 2, f);
	fclose(f);

}

/***************************************************/

HANDLE OpenGameProcessForRead()
{
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	

	if (!hwnd)	
	{
		//MessageBox(_T("Window FS not found"), _T("Message"), MB_OK | MB_ICONINFORMATION);		
		return NULL;
	}

	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ,   0, pid); // Get permission to read
	return phandle;
}


int ItemPosOnOneDimensionPackage1(DWORD dwIndex)
{
	HANDLE phandle = OpenGameProcessForRead();

	if (!phandle)	
	{
		//MessageBox(_T("Could not get handle"));
		return -1;
	}

	DWORD dwPackage;		
	ReadProcessMemory(phandle, (void*)BASE_PACKAGE, &dwPackage, (DWORD)sizeof(dwPackage), 0);

	// LEA ECX,[EAX*8+ESI+3D98] => LEA ECX, [EAX*8 + DWORD PTR SS:[912B94] + 0x36F00 + 0x3D98 ]
	DWORD dwPackageAddress = PACKAGEONE*8 + dwPackage + PACKAGE_OFFSET_ONE + PACKAGE_OFFSET_TWO;
	DWORD dwPackageBaseValue;
	ReadProcessMemory(phandle, (void*)dwPackageAddress, &dwPackageBaseValue, (DWORD)sizeof(dwPackageBaseValue), 0);

	// ==> tìm trên hành trang
	int i=0; // 35 ô trong hành trang
	DWORD dwIndexItem;
	while (i < 35)
	{

		ReadProcessMemory(phandle, (void*)dwPackageBaseValue, &dwIndexItem, (DWORD)sizeof(dwIndexItem), 0);

		if (dwIndexItem == dwIndex)
			break;
		i++;

		dwPackageBaseValue = dwPackageBaseValue + 4;

	}

	CloseHandle(phandle);
	return i;
}

int ItemPosOnOneDimensionPackage2(DWORD dwIndex)
{
	HANDLE phandle = OpenGameProcessForRead();

	if (!phandle)	
	{
		//MessageBox(_T("Could not get handle"));
		return -1;
	}

	DWORD dwPackage;		
	ReadProcessMemory(phandle, (void*)BASE_PACKAGE, &dwPackage, (DWORD)sizeof(dwPackage), 0);

	// LEA ECX,[EAX*8+ESI+3D98] => LEA ECX, [EAX*8 + DWORD PTR SS:[912B94] + 0x36F00 + 0x3D98 ]
	DWORD dwPackageAddress = PACKAGETWO*8 + dwPackage + PACKAGE_OFFSET_ONE + PACKAGE_OFFSET_TWO;
	DWORD dwPackageBaseValue;
	ReadProcessMemory(phandle, (void*)dwPackageAddress, &dwPackageBaseValue, (DWORD)sizeof(dwPackageBaseValue), 0);

	// ==> tìm trên hành trang
	int i=0; // 35 ô trong hành trang
	DWORD dwIndexItem;
	while (i < 35)
	{
		ReadProcessMemory(phandle, (void*)dwPackageBaseValue, &dwIndexItem, (DWORD)sizeof(dwPackageBaseValue), 0);

		if (dwIndexItem == dwIndex)
			break;
		i++;

		dwPackageBaseValue = dwPackageBaseValue + 4;

	}
	CloseHandle(phandle);
	return i;
}

void ItemPosOnPackage1(DWORD dwIndex, int& row, int& col)
{	
	int val = ItemPosOnOneDimensionPackage1(dwIndex);

	if (val == 35)
	{
		row = -1;
		col = -1;
		return;
	}

	row = val / 5;
	col = val % 5;	
}

void ItemPosOnPackage2(DWORD dwIndex, int& row, int& col)
{
	int val = ItemPosOnOneDimensionPackage2(dwIndex);

	if (val == 35)
	{
		row = -1;
		col = -1;
		return;
	}

	row = val / 5;
	col = val % 5;	
}

DWORD GetIndexofItem(int iType)
{
	HANDLE phandle = OpenGameProcessForRead();

	if (!phandle)	
		return -1;

	DWORD dwValNumItemTmp;		
	ReadProcessMemory(phandle, (void*)ITEMNUMBER_BASE_ADDRESS, &dwValNumItemTmp, (DWORD)sizeof(dwValNumItemTmp), 0);


	// TCHAR dwItemName[] = { 0x54, 0x69, 0xD3, 0x75, 0x20, 0x48, 0xE5, 0x6E, 0x67, 0x20, 0xAE, 0xAC, 0x6E};	// Tiểu Hồng Đơn
	// TCHAR dwItemName[] = { 0x6C, 0x61, 0x6D, 0x20, 0x31 };	// Tiểu Hoàn Đơn - Lam 1

	const int MAXLEN = 15;	
	TCHAR dwItemNameArr[][MAXLEN] = 
	{
		// ---- HP ----
		{0x54, 0x69, 0xD3, 0x75, 0x20, 0x48, 0xE5, 0x6E, 0x67, 0x20, 0xAE, 0xAC, 0x6E, 0x00},	// Tiểu Hồng Đơn	1	0	0	0
		{0x54, 0x72, 0x75, 0x6E, 0x67, 0x20, 0x48, 0xE5, 0x6E, 0x67, 0x20, 0xAE, 0xAC, 0x6E},	// Trung Hồng đơn	1	1	0	0
		{0x69, 0x20, 0x48, 0xE5, 0x6E, 0x67, 0x20, 0xAE, 0xAC, 0x6E, 0x00, 0x00, 0x00, 0x00},	// Đại Hồng đơn		1	2	0	0			
		{0x53, 0xAB, 0x20, 0x43, 0xAB, 0x20, 0x4C, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// Sô cô la			1	6	0	0
		{0x42, 0xCA, 0x74, 0x20, 0x54, 0xF6, 0x20, 0x64, 0xAD, 0xEE, 0x63, 0x00, 0x00, 0x00},	// Bất Tử dược		1	17	0	0	

		// ---- MP ----
		{0x54, 0x69, 0xD3, 0x75, 0x20, 0x48, 0x6F, 0xB5, 0x6E, 0x20, 0xAE, 0xAC, 0x6E, 0x00},	// Tiểu Hoàn đơn	1	3	0	0
		{0x54, 0x72, 0x75, 0x6E, 0x67, 0x20, 0x48, 0x6F, 0xB5, 0x6E, 0x20, 0xAE, 0xAC, 0x6E},	// Trung Hoàn đơn	1	4	0	0
		{0xA7, 0xB9, 0x69, 0x20, 0x48, 0x6F, 0xB5, 0x6E, 0x20, 0xAE, 0xAC, 0x6E, 0x00, 0x00},	// Đại Hoàn đơn		1	5	0	0
		{0x56, 0xE4, 0x6E, 0x67, 0x20, 0x44, 0x75, 0x79, 0xAA, 0x6E, 0x20, 0xAE, 0xAC, 0x6E}	// Vọng Duyên đơn	1	18	0	0

	};

	TCHAR dwItemName[MAXLEN];
	for (int i=0; i<MAXLEN; i++)
		dwItemName[i] = dwItemNameArr[iType][i];
	
	int len = sizeof(dwItemName) / sizeof(TCHAR);

	for (int id=1; id<= 70; id++)	// 70 chỗ trong hành trang
	{		
		// compare with item name;// Tiểu Hồng Đơn

		// MOV BYTE PTR DS:[EDX+ESI+64C]
		DWORD dwValNum = dwValNumItemTmp + ITEMNUMBER_OFFSET + (DISTANCE_ITEMNUMBER * id);

		DWORD dwPosItemNameAddress =  dwValNum - ITEMNAME_BASEADDRESS;	
		BYTE bValName;

		int iSame = 0;
		int k=0;
		for (int j=1; j<=20; j++)
		{
			ReadProcessMemory(phandle, (void*)dwPosItemNameAddress, &bValName, (BYTE)sizeof(bValName), 0);
			if (bValName == dwItemName[k])
				iSame++;
			k++;
			dwPosItemNameAddress = (DWORD)dwPosItemNameAddress + 0x1;

			if (k>len)
			{
				if (iSame == len )	// giống nhau y chang
					return id;		// id của item;
				else
					break;			// tìm vi trí hành trang số hai
			}
		}
	}

	return 0;
}

void SetLeftSkill(int iIndex) // test oke
{
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	

	if (!hwnd)
		return;

	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ,   0, pid); // Get permission to read

	if (!phandle)	
		return;
	
	/*
	Address   Hex dump          Command                                  Comments
	0058C59F  |> /A1 942B9100   MOV EAX,DWORD PTR DS:[912B94]
	0058C5A4  |. |8D88 C0BA0200 LEA ECX,[EAX+2BAC0]                      ; -- SetLeftSkill CoreClient:KPlayer
	0058C5AA  |. |E8 71620200   CALL 005B2820                            ; \game.005B2820
	*/

	DWORD dwValueTemp;		
	ReadProcessMemory(phandle, (void*)0x00912B94, &dwValueTemp, (DWORD)sizeof(dwValueTemp), 0);	
	DWORD dw_EcxAddress = dwValueTemp + 0x0002BAC0;
	CloseHandle(phandle);	

	DWORD dwAddress = 0x005B2820;

	__asm
	{

		PUSHAD
		PUSHFD
		PUSH iIndex
		MOV ECX, dw_EcxAddress		
		CALL dwAddress	
		POPFD
		POPAD
	};
}


/****************************************************/
// 5 parameters
/***************************************************
+ 1. Parameter 1: Id item (Hexa)
+ 2. Parameter 2: vị trí đề (Dec)
	- 0xB: nằm ở 4 vị trí bên dưới (shortcut)
	- 3: nằm trong hành trang
+ 3. Vị trí cột trong hành trang (Dec)
+ 4. Vị trí dòng trong hành trang (Dec)
+ 5. luôn luôn bằng 0; (Dec)
****************************************************/

void ApplyUseItem()
{
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	

	if (!hwnd)
		return;

	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ,   0, pid); // Get permission to read

	if (!phandle)	
		return;

	/*CPU Disasm
	Address   Hex dump          Command                                  Comments
	0058C4E5  |.  A1 942B9100   MOV EAX,DWORD PTR DS:[912B94]            ; |		==> chú ý chỗ này
	0058C4EA  |.  8BCC          MOV ECX,ESP                              ; |
	0058C4EC  |.  56            PUSH ESI                                 ; |Arg1
	0058C4ED  |.  8911          MOV DWORD PTR DS:[ECX],EDX               ; |Arg2 => [LOCAL.13]
	0058C4EF  |.  8959 04       MOV DWORD PTR DS:[ECX+4],EBX             ; |Arg3
	0058C4F2  |.  8979 08       MOV DWORD PTR DS:[ECX+8],EDI             ; |Arg4
	0058C4F5  |.  8D88 C0BA0200 LEA ECX,[EAX+2BAC0]                      ; |		==> chú ý chỗ này
	*/

	DWORD dwValueTemp;		
	ReadProcessMemory(phandle, (void*)BASE_ECX_ADDRESS, &dwValueTemp, (DWORD)sizeof(dwValueTemp), 0);	
	DWORD dw_EcxAddress = dwValueTemp + OFFSET_ECX_ADDRESS;
	
	CloseHandle(phandle);	

	// kiểm tra hành trang số 1;
	int row, col;
	DWORD dwIndex = GetIndexofItem(2);
	ItemPosOnPackage1(dwIndex, row, col);
	
	// kiểm tra hành trang số 2;
	
	DWORD dwFunctionAddress = 0x005B18A0;
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 0			// para 5 
		PUSH row		// para 4
		PUSH col		// para 3
		PUSH 3			// para 2	
		PUSH dwIndex	// para 1
		MOV ECX, dw_EcxAddress		
		CALL dwFunctionAddress	
		POPFD
		POPAD
	}
}

// *************************************************************
#define ECX_USESKILL 0x0075EF48

DWORD GetSectarian(void)
{
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	

	if (!hwnd)	
		return 0x0;

	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ,   0, pid); // Get permission to read
	if (!phandle)
		return 0x0;

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

void UseSkillGiapSi(DWORD dwFunction, DWORD dwECX_Address, int iIndex, DWORD param5)
{
	int x, y = 255;

	// phái = đạo sĩ
	for (int i=400-100; i<400+100; i++)
	{
		x = i;
		__asm 
		{
			PUSHAD
			PUSHFD	
			PUSH param5;
			PUSH dwECX_Address
			PUSH iIndex	
			PUSH y
			PUSH x			
			MOV ECX, dwECX_Address
			CALL dwFunction
			POPFD
			POPAD
		};		
	}
}

void UseSkillDiNhan(DWORD dwFunction, DWORD dwECX_Address, int iIndex, DWORD param5)
{
	int x, y = 255;

	// phái = đạo sĩ
	for (int i=400-50; i<400+50; i++)
	{
		x = i;
		__asm 
		{
			PUSHAD
			PUSHFD	
			PUSH param5;
			PUSH dwECX_Address
			PUSH iIndex	
			PUSH y
			PUSH x			
			MOV ECX, dwECX_Address
			CALL dwFunction
			POPFD
			POPAD
		};		
	}
}


void UseSkillDaoSi(DWORD dwFunction, DWORD dwECX_Address, int iIndex, DWORD param5)
{
	int x, y = 255;

	// phái = đạo sĩ
	for (int i=400-100; i<400+100; i++)
	{
			x = i;			
			__asm 
			{
				PUSHAD
				PUSHFD	
				PUSH param5;
				PUSH dwECX_Address
				PUSH iIndex	
				PUSH y
				PUSH x			
				MOV ECX, dwECX_Address
				CALL dwFunction
				POPFD
				POPAD
			};		
	}
}



/***********************************************************************
VỊLH - Use Skill - Hàm sử dụng right click 
có 5 tham số:
	PUSH param5;
	PUSH dwECX_Address
	PUSH iIndex	
	PUSH y
	PUSH x
- x,y là tọa độ click trên màn hình
- Index là index của skill
- dwECXAddress: vùng nhớ thanh ghi ECX
- param5: luôn có giá trị 0x9
	
***********************************************************************/

void UseSkill(int iIndex)
{
	//DWORD x				= 0x000001A0;	// tọa độ x click trên màn hình
	//DWORD y				= 0x00000154;	// tọa độ y click trên màn hình	

	//int x = 166;							// tọa độ x click trên màn hình
	//int y = 255;							// tọa độ y click trên màn hình	

	DWORD dwFunction	= 0X005938B0;
	//DWORD dwIndex		= 0x0000002A;	// Khuynh Thành nhất kích
	//DWORD dwIndex		= 0x00000023;	// Tam đầu lục thủ
	//DWORD dwIndex		= 0x0000001F;	// Điện Quang Trảm
	DWORD param5		= 0x9;
	DWORD dwECX_Address;


	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	
	if (!hwnd)
		return;
	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read
	if (!phandle)	
		return;

	ReadProcessMemory(phandle, (void*)ECX_USESKILL, &dwECX_Address, (DWORD)sizeof(dwECX_Address), 0);	
	CloseHandle(phandle);	

	switch (GetSectarian())
	{
		case 0x12:
			UseSkillGiapSi(dwFunction, dwECX_Address, iIndex, param5);
			break;
		case 0x13:
			UseSkillDaoSi(dwFunction, dwECX_Address, iIndex, param5);
			break;
		case 0x14:
			UseSkillDiNhan(dwFunction, dwECX_Address, iIndex, param5);
			break;
		case 0x0:		
			break;
		
	}

	/*__asm 
	{
		PUSHAD
		PUSHFD	
		PUSH param5;
		PUSH dwECX_Address
		PUSH iIndex	
		PUSH y
		PUSH x			
		MOV ECX, dwECX_Address
		CALL dwFunction
		POPFD
		POPAD
	};	*/
}

// --------------------------------------------------------------
// SendClientCmdSkill
// skill hỗ trợ:
// dùng tọa độ map (200,201) trên màn hình


// Skill target trên nhân vật
// CPU Disasm
// Address   Hex dump          Command                                  Comments
// 00593D0E  |.  52            PUSH EDX
// 00593D0F  |.  6A FF         PUSH -1
// 00593D11  |>  8B8406 141100 MOV EAX,DWORD PTR DS:[EAX+ESI+1114]      ; -- SendClientCmdSkill CoreClient:KProtocol
// 00593D18  |.  50            PUSH EAX                                 ; |Arg1
// 00593D19  |.  E8 32480B00   CALL 00648550                            ; \game.00648550

// --------------------------------------------------------------
void SendClientCmdSkill()	// chưa chạy được
{
	//DWORD pid;		
	//HWND hwnd = FindWindowA("FSOnline Class", NULL );	
	//if (!hwnd)
	//	return;
	//GetWindowThreadProcessId(hwnd, &pid);	
	//HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read
	//if (!phandle)	
	//	return;

	//DWORD dwEax_Address;
	//ReadProcessMemory(phandle, (void*)0x008FA464, &dwEax_Address, (DWORD)sizeof(dwEax_Address), 0);	
	//
	////DWORD dwEax = dwEax_Address + 0x00001114 + 0x0000B5C0;
	//DWORD dwEax = dwEax_Address + 0x0000C6D4;
	//short sValue;
	//ReadProcessMemory(phandle, (void*)dwEax, &sValue, (DWORD)sizeof(sValue), 0);	

//	CloseHandle(phandle);	

	DWORD dwFunction = 0x00648550;
	int y = 14791;
	__asm 
	{
		PUSHAD
		PUSHFD	
		PUSH y
		PUSH -1			
		PUSH 0x5
		MOV EAX, 0x5
		CALL dwFunction
		POPFD
		POPAD
	};	
}

// --------------------------------------------------------------
// Chạy trong phạm vi ngắn 
// --------------------------------------------------------------
// x,y = toa độ map (220,180)
// x =  51236 Dec
// y = 100256 Dec

void RunTo(int x,int y)
{
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	
	if (!hwnd)
		return;
	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read
	if (!phandle)	
		return;
	
	DWORD dwECX_BaseAddress		= 0x008FA464;
	DWORD dwOffsetECX_Address	= 0x0000B5C0;	 
	DWORD dwECX_Value;

	ReadProcessMemory(phandle, (void*)dwECX_BaseAddress, &dwECX_Value, (DWORD)sizeof(dwECX_Value), 0);	
	DWORD dwECX_Address = dwECX_Value + dwOffsetECX_Address;
	CloseHandle(phandle);	

	/*
	CPU Disasm
	Address   Hex dump          Command                               Comments
	0059D2DB  |.  8B86 442A0000 MOV EAX,DWORD PTR DS:[ESI+2A44]       ; -- RunTo CoreClient:KNpc
	0059D2E1  |.  52            PUSH EDX                              ; /Arg2
	0059D2E2  |.  50            PUSH EAX                              ; |Arg1
	0059D2E3  |.  8BCE          MOV ECX,ESI                           ; |ASCII "G0"
	0059D2E5  |.  E8 F61D0000   CALL 0059F0E0                         ; \game.0059F0E0
	*/
	DWORD dwFunction = 0x0059F0E0;

	// không convert
	//x = x * 256;
	//y = y * 512;

	__asm 
	{
		PUSHAD
		PUSHFD	
		PUSH y
		PUSH x			
		MOV ECX, dwECX_Address
		CALL dwFunction
		POPFD
		POPAD
	};	
}


// --------------------------------------------------------------
// Tự động tìm đường (phạm vi rộng)
// Tọa độ game:	(x,y) = (220,198)
// x = x * 8; 
// y = y * 16;
// --------------------------------------------------------------

void AutoRun(int x, int y)
{

	/*	
	CPU Disasm
	Address   Hex dump          Command        Comments
	0058F66A  |.  57            PUSH EDI       ; /Arg6
	0058F66B  |.  6A 00         PUSH 0         ; |Arg5 = 0
	0058F66D  |.  57            PUSH EDI       ; |Arg4
	0058F66E  |.  57            PUSH EDI       ; |Arg3
	0058F66F  |.  50            PUSH EAX       ; |Arg2 => [ARG.3]
	0058F670  |.  56            PUSH ESI       ; |Arg1
	0058F671  |.  E8 EAD80A00   CALL 0063CF60  ; |
	0058F676  |.  8BC8          MOV ECX,EAX    ; |
	0058F678  |.  E8 33E90A00   CALL 0063DFB0  ; \game.0063DFB0
	*/

	DWORD dwECX_Address = 0x00E03D18;
	DWORD dwFunction	= 0x0063DFB0;
	x = x * 8;
	y = y * 16;

	__asm 
	{
		PUSHAD
		PUSHFD	
		PUSH 1
		PUSH 0
		PUSH 1
		PUSH 1
		PUSH y
		PUSH x			
		MOV ECX, dwECX_Address
		CALL dwFunction
		POPFD
		POPAD
	};	
}

// --------------------------------------------------------------
// Search target
// --------------------------------------------------------------

#define NPC_OFFSET_B5C0		0x0000B5C0
#define NPC_OFFSET_15D0		0x000015D0
#define NPC_BASEADDRESS		0X008FA464
#define NPC_MAX				255

// --------------------------------------------------------------

// *************************************************************
//void GoToWhere()
//{
//	/*CPU Disasm
//	Address   Hex dump          Command                                  Comments
//	004907AF  |.  8B0D 48EF7500 MOV ECX,DWORD PTR DS:[75EF48]            ; PTR to ASCII "`GY"
//	004907B5  |.  85C9          TEST ECX,ECX
//	004907B7  |.  74 4E         JE SHORT 00490807
//	004907B9  |.  8BD6          MOV EDX,ESI
//	004907BB  |.  8B01          MOV EAX,DWORD PTR DS:[ECX]
//	004907BD  |.  C1EA 10       SHR EDX,10
//	004907C0  |.  0FBFD2        MOVSX EDX,DX
//	004907C3  |.  6A 00         PUSH 0
//	004907C5  |.  52            PUSH EDX
//	004907C6  |.  0FBFD6        MOVSX EDX,SI
//	004907C9  |.  52            PUSH EDX
//	004907CA  |.  FF50 44       CALL DWORD PTR DS:[EAX+44]               ; -- GotoWhere CoreClient:CoreShell*/
//
//	DWORD pid;		
//	HWND hwnd = FindWindowA("FSOnline Class", NULL );	
//	if (!hwnd)
//		return;
//	GetWindowThreadProcessId(hwnd, &pid);	
//	HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read
//	if (!phandle)	
//		return;
//
//	DWORD dwECX_Address;
//	ReadProcessMemory(phandle, (void*)0x0075EF48, &dwECX_Address, (DWORD)sizeof(dwECX_Address), 0);	
//	CloseHandle(phandle);	
//	
//	DWORD dwFunction = 0x005940E0;
//
//	//DWORD dwxPos	 = 0x00000190;	// tọa độ x click trên màn hình
//	//DWORD dwyPos	 = 0x000001F4;	// tọa độ y click trên màn hình	
//	//__asm 
//	//{
//	//	PUSHAD
//	//	PUSHFD		
//	//	PUSH 0
//	//	PUSH dwyPos
//	//	PUSH dwxPos
//	//	MOV ECX, dwECX_Address
//	//	CALL dwFunction		
//	//	POPFD
//	//	POPAD
//	//};
//
//	DWORD dwXvalue, dwYvalue;;
//	ReadProcessMemory(phandle, (void*)0x0097D3E0, &dwXvalue, (DWORD)sizeof(dwXvalue), 0);		
//	ReadProcessMemory(phandle, (void*)0x0097D3E4, &dwYvalue, (DWORD)sizeof(dwYvalue), 0);	
//	int iCurXPos = dwXvalue / 256;
//	int iCurYPos = dwXvalue / 512;
//	
//
//	int iDestXPos = 209; 
//	int iDestYPos = 209;
//
//	//DWORD dwCurXPos = 0x320, dwCurYPos = 0x64; //800,600 
//	
//	// Left
//	int iYPosScreenArr[8] = {267, 162, 81,  116, 273, 373, 459, 385};
//	int iXPosScreenArr[8] = {215, 270, 386, 523, 573, 533, 400, 283};
//	
//	int xPos, yPos;
//	
//	while (iCurXPos != iDestXPos && iCurYPos != iDestYPos)
//	{
//		if ( (iCurXPos > iDestXPos)  && (iCurYPos > iDestYPos) )
//		{
//			iCurXPos--;
//			iCurYPos--;
//			xPos = iXPosScreenArr[1];
//			yPos = iYPosScreenArr[1];
//
//		}
//		else if ( (iCurXPos < iDestXPos)  && (iCurYPos > iDestYPos) )
//		{
//			iCurXPos ++;
//			iCurYPos --;
//
//			xPos = iXPosScreenArr[3];
//			yPos = iYPosScreenArr[3];
//		}
//		else if ( (iCurXPos > iDestXPos)  && (iCurYPos < iDestYPos) )
//		{
//			iCurXPos --;
//			iCurYPos ++;
//
//			xPos = iXPosScreenArr[7];
//			yPos = iYPosScreenArr[7];
//		}
//		else if ( (iCurXPos < iDestXPos)  && (iCurYPos < iDestYPos) )
//		{
//			iCurXPos ++;
//			iCurYPos ++;
//
//			xPos = iXPosScreenArr[5];
//			yPos = iYPosScreenArr[5];
//		}
//
//		__asm 
//		{
//			PUSHAD
//			PUSHFD		
//			PUSH 0
//			PUSH yPos
//			PUSH xPos
//			MOV ECX, dwECX_Address
//			CALL dwFunction		
//			POPFD
//			POPAD
//		};
//
//		ReadProcessMemory(phandle, (void*)0x0097D3E0, &dwXvalue, (DWORD)sizeof(dwXvalue), 0);		
//		ReadProcessMemory(phandle, (void*)0x0097D3E4, &dwYvalue, (DWORD)sizeof(dwYvalue), 0);	
//		iCurXPos = dwXvalue / 256;
//		iCurYPos = dwXvalue / 512;
//	}	
//
//}

void SpilitPosAndIdSkill(wchar_t* s)
{	
	wchar_t sSource[20] = L"";	
	wcscpy(sSource, s);
	wchar_t* token = wcstok(sSource, L":");
	iPosition = _tstoi(token);
	iIdSkill = _tstoi(wcstok(NULL, L":"));
}

// *************************************************************
// x: tọa độ x (trên map) 210,235
// y: tọa độ y (trên map) 210,235
void SendClientCmdRun(int x, int y)
{
		
	/*CPU Disasm
	Address   Hex dump          Command                                  Comments
	00594393  |.  52            PUSH EDX                                 ; /Arg2 => [ARG.2]
	00594394  |.  50            PUSH EAX                                 ; |Arg1 => [ARG.1]
	00594395  |.  E8 963F0B00   CALL 00648330                            ; \game.00648330*/
	
	DWORD pid;		
	HWND hwnd = FindWindowA("FSOnline Class", NULL );	
	if (!hwnd)
		return;
	GetWindowThreadProcessId(hwnd, &pid);	
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read
	if (!phandle)	
		return;

	DWORD dwtemp;
	ReadProcessMemory(phandle, (void*)0x00912B94, &dwtemp, (DWORD)sizeof(dwtemp), 0);	
	CloseHandle(phandle);	

	DWORD dwECX_Address = dwtemp + 0x0002BAC0;

	DWORD dwFunction = 0x00648330;
	x = x * 256;
	y = y * 512;

	// DỊCH chuyển tức thời
	__asm 
	{
		PUSHAD
		PUSHFD		
		PUSH y
		PUSH x
		MOV ECX, dwECX_Address
		CALL dwFunction		
		POPFD
		POPAD
	};
}

//void SendClientCmdSkill()
//{
//	DWORD dwFunction = 0x00648550;
//	
//	DWORD pid;		
//	HWND hwnd = FindWindowA("FSOnline Class", NULL );	
//
//	if (!hwnd)
//		return;
//
//	GetWindowThreadProcessId(hwnd, &pid);	
//	HANDLE phandle = OpenProcess(PROCESS_VM_READ,   0, pid); // Get permission to read
//
//	if (!phandle)	
//		return;
//
//	unsigned short iXPos, iYPos;
//
//	DWORD XPosAdd = 0x0097D3E0;	// Tọa độ hiện tại x của nhân vật
//	DWORD YPosAdd = 0x0097D3E4;	// Tọa độ hiện tại y của nhân vật
//
//	ReadProcessMemory(phandle, (void*)XPosAdd, &iXPos, (DWORD)sizeof(iXPos), 0);	
//	ReadProcessMemory(phandle, (void*)YPosAdd, &iYPos, (DWORD)sizeof(iYPos), 0);	
//	CloseHandle(phandle);	
//
//	int index = 42;
//	
//	DWORD x = 0x0000E4C2;
//	DWORD y = 0x00018860;
//
//	__asm
//	{
//		PUSHAD
//		PUSHFD		
//		//PUSH iYPos
//		//PUSH iXPos
//
//		PUSH y
//		PUSH x
//
//		PUSH index				
//		CALL dwFunction
//		POPFD
//		POPAD
//	};
//}

// ************************************************************************

BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hModuleDll = (HINSTANCE) hModule;
		DisableThreadLibraryCalls(hModuleDll);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

LRESULT _stdcall CALLBACK HookProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode<0)
		goto END;
	
	HWND hVLWnd = pCW->hwnd;
	HHOOK hHook = (HHOOK)pCW->wParam;
	if((pCW->message == WM_HOOKEX) && pCW->lParam)
	{
		UnhookWindowsHookEx(hHookKeyBoard);
		if (bHooked)
			goto END;
		TCHAR lib_name[MAX_PATH];
		GetModuleFileName(hModuleDll, lib_name, MAX_PATH);
		if(!LoadLibrary(lib_name))
			goto END;
		OldWndProc = (WNDPROC)SetWindowLong(hVLWnd, GWL_WNDPROC, (LONG)NewWndProc);
		if(OldWndProc==NULL) {
			FreeLibrary(hModuleDll);
		}
		else
		{
			bHooked = TRUE;
		}
	}
	else if(pCW->message == WM_HOOKEX) 
	{
		UnhookWindowsHookEx(hHookKeyBoard);
		if (!bHooked)
			goto END;
		if(!SetWindowLong(hVLWnd, GWL_WNDPROC, (LONG)OldWndProc))
			goto END;
		FreeLibrary(hModuleDll);
		bHooked = FALSE;
	}
	END:
		return CallNextHookEx(hHook,nCode, wParam,lParam);
}

// *****************************************************************************
LRESULT CALLBACK NewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	switch(uMsg) 
	{
		case WM_COPYDATA:
			COPYDATASTRUCT * pcds = (COPYDATASTRUCT *)lParam;

			if (pcds->dwData == WM_LOCKSOMEONEUSESKILL)
			{
				wchar_t szGameAlias[MAX_PATH + 1] = {0};
				memcpy(szGameAlias, pcds->lpData, min(pcds->cbData, MAX_PATH));
					
				// cắt chuỗi:
				SpilitPosAndIdSkill(szGameAlias);					
			}		
			else if (pcds->dwData == WM_SENDSTRUCTURE)
			{
				wchar_t szGameAlias[MAX_PATH + 1] = {0};
				memcpy(szGameAlias, pcds->lpData, min(pcds->cbData, MAX_PATH));

				int index = ((Locksomeoneuseskill *)(pcds->lpData))->iIndex;
				int pos = ((Locksomeoneuseskill *)(pcds->lpData))->iPos;
				CString str = ((Locksomeoneuseskill *)(pcds->lpData))->s;
								
				str.AppendFormat(L"-index = %d - Pos = %d", index, pos);
				MessageBox(hWnd, str, L"Info", MB_OK);
			}
			break;	
	}
		
	if (uMsg == WM_HOOK_WRITE)
	{
			int x,y;			
			switch (wParam)				// sendMessage side(Dlg side)
			{			
				case WPARAM_USEITEM:
					ApplyUseItem();
					break;

				case WPARAM_SETLEFTSKILL:
					SetLeftSkill((int)lParam);				
					break;

				case WPARAM_USESKILL:				
					UseSkill((int)lParam);
					break;			

				case WPARAM_RUNTO:				
					x = LOWORD(lParam);
					y = HIWORD(lParam);
					//RunTo(x,y);			// chạy trong phạm vi ngắn thôi (1 client chạy, các client khác không active)
					AutoRun(x, y);			// tự động tìm đường
					break;

				case WPARAM_SENDCLIENTCMDRUN:
					x = LOWORD(lParam);
					y = HIWORD(lParam);
					SendClientCmdRun(x, y);
					break;
		
				case WPARAM_SENDCLIENTCMDSKILL:
					SendClientCmdSkill();
					break;

				case WPARAM_AUTOPK:					
					UseSkill((int)lParam);
					break;
					// character = SearchTarget(L"TaiQUachABC");	
					// RunTo(character.m_iCurXPos - 100, character.m_iCurYPos + 100);			// chạy trong phạm vi ngắn thôi (1 client chạy)
					// AutoRun(character.m_iCurXPos / 256, character.m_iCurYPos / 512);			// oke
					// AutoRun(character.m_iCurXPos / 32, character.m_iCurYPos / 32); 
					// UseSkill((int)lParam);									
					/*
					CString str;
					str.Format(L"name = %s\nxPos = %d - yPos = %d\nHP = %d", character.strName, character.m_iCurXPos / 256, character.m_iCurYPos / 512, character.m_iHP);
					MessageBox(hWnd, str, L"Information", MB_OK);
					*/
				case WPARAM_FOLLOWPEOPLE:
					FollowPeople((int)lParam);	// dịch chuyển tức thời
					break;
				
				case WPARAM_LOCKSOMEONEUSESKILL:
										
					if (!hWnd)
						return CallWindowProc(OldWndProc, hWnd, uMsg, wParam, lParam);

					DWORD pid;
					GetWindowThreadProcessId(hWnd, &pid);	
					HANDLE phandle = OpenProcess(PROCESS_VM_READ, 0, pid); // Get permission to read

					if (!phandle)	
						return CallWindowProc(OldWndProc, hWnd, uMsg, wParam, lParam);

					if (iPosition > 0 && iIdSkill > 0)
						LockSomeOneUseSkill(phandle, iPosition, iIdSkill);

					break;			
			}			
	}

	return CallWindowProc(OldWndProc, hWnd, uMsg, wParam, lParam);
}

int InjectDll(HWND hWnd)
{	
	if (!IsWindow(hWnd))
		return 0;
	ghWnd = hWnd;
	hHookKeyBoard = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC) HookProc, hModuleDll,  GetWindowThreadProcessId(hWnd,NULL));
	if(hHookKeyBoard == NULL)
	{
		 //Error
		MessageBox(NULL,L"Cannot set Hook to FS Window", L"InjectDLL Error", MB_OK);
		return 0;
	}
	else{
		SendMessage(hWnd, WM_HOOKEX, WPARAM(hHookKeyBoard), 1);
	}
	return 1;
}

int UnmapDll(HWND hWnd)
{	
	if (!IsWindow(hWnd))
		return 0;
	HHOOK hHook = SetWindowsHookEx(WH_CALLWNDPROC,(HOOKPROC)HookProc, hModuleDll, GetWindowThreadProcessId(hWnd,NULL));
	if(hHook==NULL)
		return 0;
	SendMessage(hWnd, WM_HOOKEX, (WPARAM)hHook, 0);
	return 1;

}

