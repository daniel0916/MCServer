
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "SpiderChecker.h"
#include "AntiCheat.h"





bool cSpiderChecker::Check(cPlayer & a_Player, double a_DiffY)
{
	if (a_DiffY <= 0.11761 && a_DiffY >= 0.11759 && !g_BlockIsClimbable[a_Player.GetWorld()->GetBlock(a_Player.GetPosition())])
	{
		cAntiCheat::LOG(a_Player.GetName() + " tried to climb a non-ladder (" + Printf("%i", a_Player.GetWorld()->GetBlock(a_Player.GetPosition())) + ")");
		return true;
	}

	return false;
}



