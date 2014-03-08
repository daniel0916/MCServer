
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "FastDropChecker.h"
#include "AntiCheat.h"
#include "../Entities/Player.h"





bool cFastDropChecker::Check(cPlayer & a_Player)
{
	if (cAntiCheat::m_FastDrop)
	{
		a_Player.m_BlocksDropped = cAntiCheat::increment(a_Player, a_Player.m_BlocksDropped, 10);

		if (a_Player.m_BlockTime == 0)
		{
			a_Player.m_BlockTime = time(0);
			return false;
		}
		else if (a_Player.m_BlocksDropped == 10)
		{
			time_t time1 = time(0) - a_Player.m_BlockTime;
			a_Player.m_BlockTime = 0;
			a_Player.m_BlocksDropped = 0;

			if (time1 < 0.8)
			{
				cAntiCheat::LOG(a_Player.GetName() + " dropped an item too fast (actual time=" + Printf("%d", time) + ", min time=0.8)");
				return true;
			}
		}
	}

	return false;
}




