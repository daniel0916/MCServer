
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "AntiCheat.h"
#include "inifile/iniFile.h"
#include "Entities/Player.h"
#include "Blocks/BlockHandler.h"
#include "Log.h"





bool cAntiCheat::m_SpamProtection;
double cAntiCheat::m_MaxMessages;
bool cAntiCheat::m_Speed;
bool cAntiCheat::m_Spider;
bool cAntiCheat::m_WaterWalking;
bool cAntiCheat::m_FastDrop;
bool cAntiCheat::m_VClip;


cLog * cAntiCheat::m_Log = NULL;





cAntiCheat::cAntiCheat(void)
{
}





cAntiCheat::~cAntiCheat()
{
	m_Log->Log("--- Stopped AntiCheat-Log ---\n");
	delete m_Log;
	m_Log = NULL;
}





void cAntiCheat::LoadSettings(void)
{
	cFile::CreateFolder(FILE_IO_PREFIX + AString("AntiCheat"));

	cIniFile AntiCheatIniFile;
	if (!AntiCheatIniFile.ReadFile("AntiCheat/AntiCheat.ini"))
	{
		LOGWARN("Regenerating AntiCheat.ini, all settings will be reset");
		AntiCheatIniFile.AddHeaderComment(" This is the anticheat configuration");
		AntiCheatIniFile.AddValueB("Spam-Protection", "Enabled", false);
		AntiCheatIniFile.AddValueF("Spam-Protection", "Value", 5);
		AntiCheatIniFile.AddValueB("Speed", "Enabled", false);
		AntiCheatIniFile.AddValueB("Spider", "Enabled", false);
		AntiCheatIniFile.AddValueB("WaterWalking", "Enabled", false);
		AntiCheatIniFile.AddValueB("FastDrop", "Enabled", false);
		AntiCheatIniFile.AddValueB("VClip", "Enabled", false);
		AntiCheatIniFile.WriteFile("AntiCheat/AntiCheat.ini");
	}

	m_SpamProtection = AntiCheatIniFile.GetValueB("Spam-Protection", "Enabled");
	m_MaxMessages = AntiCheatIniFile.GetValueF("Spam-Protection", "Value");
	m_Speed = AntiCheatIniFile.GetValueB("Speed", "Enabled");
	m_Spider = AntiCheatIniFile.GetValueB("Spider", "Enabled");
	m_WaterWalking = AntiCheatIniFile.GetValueB("WaterWalking", "Enabled");
	m_FastDrop = AntiCheatIniFile.GetValueB("FastDrop", "Enabled");
	m_VClip = AntiCheatIniFile.GetValueB("VClip", "Enabled");

	AString FileName;
	Printf(FileName, "LOG_%d.txt", (int)time(NULL));

	m_Log = new cLog(FileName, "AntiCheat/logs");
	m_Log->Log("--- Started AntiCheat-Log ---\n");
}





void cAntiCheat::LOG(AString Message)
{
	m_Log->Log(Message.c_str());
}





bool cAntiCheat::isDoing(cPlayer & a_Player, time_t a_Map, double a_Max)
{
	if (a_Map != 0)
	{
		if (a_Map != -1)
		{
			if (((time(0) - a_Map) / 1000) > a_Max)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			// Termination time has already been calculated
			if (a_Map < time(0))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	else
	{
		return false;
	}
}





bool cAntiCheat::isMovingExempt(cPlayer & a_Player)
{
	if (!isDoing(a_Player, a_Player.m_MovingExempt, -1))
	{
		a_Player.m_MovingExempt = 0;
		return false;
	}
	return true;
}





bool cAntiCheat::isSpeedExempt(cPlayer & a_Player)
{
	return isMovingExempt(a_Player) || justVelocity(a_Player);
}





bool cAntiCheat::IsInWeb(cPlayer & a_Player)
{
	return a_Player.GetWorld()->GetBlock(a_Player.GetPosition()) == E_BLOCK_COBWEB;
}





bool cAntiCheat::justPlaced(cPlayer & a_Player)
{
	if (!isDoing(a_Player, a_Player.m_PlacedBlock, 0.1))
	{
		a_Player.m_PlacedBlock = 0;
		return false;
	}
	return true;
}





bool cAntiCheat::justBroke(cPlayer & a_Player)
{
	if (!isDoing(a_Player, a_Player.m_BrokenBlock, 0.1))
	{
		a_Player.m_BrokenBlock = 0;
		return false;
	}
	return true;
}





bool cAntiCheat::justVelocity(cPlayer & a_Player)
{
	return (true ? (time(0) - a_Player.m_Velocitized) < 2.1 : false);
}





bool cAntiCheat::isHoveringOverWater(cPlayer & a_Player, int blocks)
{
	for (int i = a_Player.GetPosY(); i > a_Player.GetPosY() - blocks; i--)
	{
		BLOCKTYPE Block = a_Player.GetWorld()->GetBlock(a_Player.GetPosX(), i, a_Player.GetPosZ());
		if (Block != E_BLOCK_AIR)
		{
			return g_BlockIsLiquid[Block];
		}
	}
	return false;
}




bool cAntiCheat::isFullyInWater(cPlayer & a_Player)
{
	double touchedX = fixXAxis(a_Player.GetPosX());

	BLOCKTYPE Block1 = a_Player.GetWorld()->GetBlock(touchedX, a_Player.GetPosY(), a_Player.GetPosZ());
	BLOCKTYPE Block2 = a_Player.GetWorld()->GetBlock(touchedX, std::round(a_Player.GetPosY()), a_Player.GetPosZ());

	if ((!g_BlockIsLiquid[Block1]) && (!g_BlockIsLiquid[Block2]))
	{
		return true;
	}

	return (g_BlockIsLiquid[Block1]) && (g_BlockIsLiquid[Block2]);
}





double cAntiCheat::fixXAxis(double a_X)
{
	double touchedX = a_X;
	double rem = touchedX - std::round(touchedX) + 0.01;
	if (rem < 0.30)
	{
		int floor = (int)a_X;
		touchedX = (floor == a_X ? floor : floor + (int)(doubleToRawLongBits(a_X) ^ (long)((ULONG)0xFFFFFFFF >> 63))) -1;
	}
	return touchedX;
}





UInt64 cAntiCheat::doubleToRawLongBits(double a_Num)
{
	UInt64 bits;
	memcpy(&bits, &a_Num, sizeof bits);
	return bits;
}





int cAntiCheat::increment(cPlayer & a_Player, int map, int num)
{
	if (map == 0)
	{
		return 1;
	}
	else
	{
		int amount = map + 1;
		if (amount < num + 1)
		{
			return amount;
		}
		else
		{
			return num;
		}
	}
}





