
#pragma once

#include "../Entities/Player.h"





class cSpeedChecker
{
public:
	static bool checkYSpeed(cPlayer & a_Player, double a_DiffY);
	static bool checkXZSpeed(cPlayer & a_Player, double a_DiffX, double a_DiffZ);
	static void logVelocity(cPlayer & a_Player);
};