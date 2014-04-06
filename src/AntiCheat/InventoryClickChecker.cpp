
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "InventoryClickChecker.h"
#include "AntiCheat.h"
#include "../Entities/Player.h"





bool cInventoryClickChecker::Check(cPlayer & a_Player)
{
	if (a_Player.IsGameModeCreative())
	{
		return false;
	}

	int clicks = 1;
	if (a_Player.m_InventoryClicks > 0)
	{
		clicks = a_Player.m_InventoryClicks + 1;
	}
	a_Player.m_InventoryClicks = clicks;

	if (clicks == 1)
	{
		a_Player.m_InventoryTime = time(0);
	}
	else if (clicks == 10)
	{
		time_t time1 = time(0) - a_Player.m_InventoryTime;
		a_Player.m_InventoryClicks = 0;

		if (time1 < 0.05)
		{
			cAntiCheat::LOG(a_Player.GetName() + " clicked inventory slots " + Printf("%i", clicks) + " times in " + Printf("%d", time1) + " sec (max=10 in 0.05 sec)");
			return true;
		}
	}

	return false;
}




