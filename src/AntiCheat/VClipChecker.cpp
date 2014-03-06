
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "VClipChecker.h"
#include "AntiCheat.h"





bool cVClipChecker::Check(cPlayer & a_Player, double a_Y, double a_DiffY)
{
	double from = std::round(a_Player.GetPosY());
	double to = std::round(a_Y);

	if (a_Player.IsInVehicle() || (from == to || from < to) || std::round(a_DiffY) < 2)
	{
		return false;
	}

	for (int i = 0; i < (std::round(a_DiffY)) + 1; i++)
	{
		BLOCKTYPE Block = a_Player.GetWorld()->GetBlock(a_Player.GetPosX(), to + i, a_Player.GetPosZ());
		if (Block != E_BLOCK_AIR && g_BlockIsSolid[Block])
		{
			cAntiCheat::LOG(a_Player.GetName() + " tried to move through a solid block");
			return true;
		}
	}

	return false;
}




