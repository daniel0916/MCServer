
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "WaterWalkingChecker.h"
#include "AntiCheat.h"
#include "../Entities/Player.h"





bool cWaterWalkingChecker::Check(cPlayer & a_Player, double a_DiffX, double a_DiffY, double a_DiffZ)
{
	if (cAntiCheat::m_WaterWalking)
	{
		BLOCKTYPE Block1 = a_Player.GetWorld()->GetBlock(a_Player.GetPosition());
		BLOCKTYPE Block2 = a_Player.GetWorld()->GetBlock(a_Player.GetPosX(), a_Player.GetPosY() - 1, a_Player.GetPosZ());

		if ((!a_Player.IsInVehicle()) && (!a_Player.IsFlying()))
		{
			if (cBlockInfo::IsLiquid(Block2))
			{
				if (a_Player.m_IsInWater)
				{
					if (a_Player.m_IsInWaterCache)
					{
						bool b;
						if ((!a_Player.IsSprinting()) && (!a_Player.IsFlying()))
						{
							b = a_DiffX > 0.19 || a_DiffZ > 0.19;
						}
						else
						{
							b = a_DiffX > 0.3 || a_DiffZ > 0.3;
						}

						if ((!b) && (!cAntiCheat::isFullyInWater(a_Player)) && (cAntiCheat::isHoveringOverWater(a_Player, 1)) && (a_DiffY == 0) && (!Block1 == E_BLOCK_LILY_PAD))
						{
							b = true;
						}

						if (b)
						{
							if (a_Player.m_WaterSpeedViolation != -1)
							{
								int v = a_Player.m_WaterSpeedViolation;
								if (v >= 4)
								{
									a_Player.m_WaterSpeedViolation = 0;

									cAntiCheat::LOG(a_Player.GetName() + " stood on water " + Printf("%i", v) + " times (can't stand on " + Printf("%i", Block2) + ")");

									return true;
								}
								else
								{
									a_Player.m_WaterSpeedViolation = v + 1;
								}
							}
							else
							{
								a_Player.m_WaterSpeedViolation = 1;
							}
						}
					}
					else
					{
						a_Player.m_IsInWaterCache = true;
					}
				}
				else
				{
					a_Player.m_IsInWater = true;
				}
			}

			else
			{
				a_Player.m_IsInWater = false;
				a_Player.m_IsInWaterCache = false;
			}
		}
	}

	return false;
}




