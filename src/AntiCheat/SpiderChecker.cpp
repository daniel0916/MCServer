
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "SpiderChecker.h"
#include "AntiCheat.h"
#include "../Entities/Player.h"





bool cSpiderChecker::Check(cPlayer & a_Player, double a_DiffY)
{
	if (cAntiCheat::m_Spider)
	{
		if (a_DiffY <= 0.11761 && a_DiffY >= 0.11759 && !cBlockInfo::IsClimbable(a_Player.GetWorld()->GetBlock(a_Player.GetPosition())))
		{
			cAntiCheat::LOG(a_Player.GetName() + " tried to climb a non-ladder (" + Printf("%i", a_Player.GetWorld()->GetBlock(a_Player.GetPosition())) + ")");
			return true;
		}
	}

	return false;
}




