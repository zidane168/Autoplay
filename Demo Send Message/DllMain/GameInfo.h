
// ************************************************************
// Version 3.0.0
// ************************************************************

//#define CHARACTER_NAME_ADDRESS	0x007420A0
//
//#define CURRENT_HP_ADDRESS		0x008FA434
//#define CURRENT_HP_OFFSET		0x0000C6E4
//#define MAX_HP_ADDRESS			0x008FA434
//#define MAX_HP_OFFSET			0x0000C6E8
//
//#define CURRENT_MP_ADDRESS		0x008FA434
//#define CURRENT_MP_OFFSET		0x0000C6F0
//#define MAX_MP_ADDRESS			0x008FA434
//#define MAX_MP_OFFSET			0x0000C6F4


// ************************************************************
// Version 3.0.1
// ************************************************************
#define CHARACTER_NAME_ADDRESS			0x007420D0

#define BASE_ADDRESS					0x008FA464
#define CURRENT_HP_OFFSET				0x0000C6E4
#define MAX_HP_OFFSET					0x0000C6E8
#define CURRENT_MP_OFFSET				0x0000C6F0
#define MAX_MP_OFFSET					0x0000C6F4


// Item Address		
#define BASE_PACKAGE					0x00912B94
#define PACKAGE_OFFSET_ONE				0x00036F00
#define PACKAGE_OFFSET_TWO				0x00003D98

#define PACKAGEONE						0x00000000
#define PACKAGETWO						0x00000041
#define PACKAGETHREE					0x00000028

#define ITEMNUMBER_BASE_ADDRESS 		0x00912D60
#define ITEMNUMBER_OFFSET				0x0000064C
#define DISTANCE_ITEMNUMBER				0x000015F0	
#define ITEMNAME_BASEADDRESS			0x00000050

// Search Target		
#define NPC_OFFSET_B5C0					0x0000B5C0
#define NPC_OFFSET_15D0					0x000015D0
#define NPC_BASEADDRESS					0x008FA464
#define NPC_MAX							255

#define GIAPSI							0x12
#define DAOSI							0x13
#define DINHAN							0x14
#define QUAI							0xE

#define	INTERVAL_FIND_MONSTER			200		// khoảng thời gian ra skill đánh quái
#define INTERVAL_TIMER_DELAY			10000	// khoảng thời gian chờ đợi khi không tìm thấy quái

#define WPARAM_USEITEM					1000
#define WPARAM_SETLEFTSKILL				2000
#define WPARAM_USESKILL					3000	// use right skill
#define WPARAM_FOLLOWPEOPLE				3001	// use left skill
#define WPARAM_LOCKSOMEONEUSESKILL		3002	// Lock Some one use skills
#define WPARAM_RUNTO					4000
#define WPARAM_AUTOPK					5000
#define WPARAM_SENDCLIENTCMDSKILL		6000
#define WPARAM_SENDCLIENTCMDRUN			7000
#define WPARAM_TESTSENDSTRUCT			8000

#define WM_LOCKSOMEONEUSESKILL			 3
#define WM_SENDSTRUCTURE				10

typedef struct tag_locksomeoneuseskill
{
	wchar_t s[20];
	int iIndex;
	int iPos;
}Locksomeoneuseskill;

// KHOANG CACH NPC VOI NHAU = [0x008FA464] + 0x15D0 + 0xB5C0 * i (với i = 1 -> 255)

//DWORD XPosAdd = 0x0097D3E0;	// Tọa độ hiện tại x của nhân vật
//DWORD YPosAdd = 0x0097D3E4;	// Tọa độ hiện tại y của nhân vật


// Set Right Pos (when click)
// ==> xPos = DWORD PTR:SS[DEF058 + 5EC] 
// ==> yPos = DWORD PTR:SS[DEF058 + 5F0]

// tọa độ map	: (x, y) = (200, 195)
// Game			: x = x * 8			; y = y * 16	(API của game)
// màn hình		: x = x * 8 * 32	; y = y * 16 *32
// pixel		: (800, 600)

// 0x12: Giáp sĩ
// 0x13: Đạo sĩ
// 0x14: Dị nhân