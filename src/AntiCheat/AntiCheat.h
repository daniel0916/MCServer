
#pragma once

#include "Entities/Player.h"





class cAntiCheat
{
public:
	cAntiCheat(void);
	~cAntiCheat();

	void LoadSettings(void);

	static void LOG(AString Message);

	static bool isDoing(cPlayer & a_Player, time_t a_Map, double a_Max);
	static bool isMovingExempt(cPlayer & a_Player);
	static bool isSpeedExempt(cPlayer & a_Player);
	static bool IsInWeb(cPlayer & a_Player);
	static bool isHoveringOverWater(cPlayer & a_Player, int blocks);
	static bool isFullyInWater(cPlayer & a_Player);
	static bool justPlaced(cPlayer & a_Player);
	static bool justBroke(cPlayer & a_Player);
	static bool justVelocity(cPlayer & a_Player);
	static bool extendVelocityTime(cPlayer & a_Player);
	static void logEnterExit(cPlayer & a_Player);
	static void logToggleSneak(cPlayer & a_Player);
	static void logJoin(cPlayer & a_Player);
	static double fixXAxis(double a_X);
	static UInt64 doubleToRawLongBits(double a_Num);
	static int increment(cPlayer & a_Player, int map, int num);


	static bool m_SpamProtection;
	static double m_MaxMessages;
	static bool m_Speed;
	static bool m_Spider;
	static bool m_WaterWalking;
	static bool m_FastDrop;
	static bool m_VClip;

	static cLog * m_Log;

};




