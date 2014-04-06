
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "SpeedChecker.h"
#include "AntiCheat.h"
#include "../Entities/Player.h"





bool cSpeedChecker::checkYSpeed(cPlayer & a_Player, double a_DiffY)
{
	if (cAntiCheat::m_Speed)
	{
		// TODO: Add Jump Potion Checking
		if (!cAntiCheat::isMovingExempt(a_Player) && !a_Player.IsInVehicle() && !a_Player.IsSleeping() && a_DiffY > 0.5 && !cAntiCheat::isDoing(a_Player, a_Player.m_Velocitized, 0.003))
		{
			a_Player.m_Velocitized = 0;
			cAntiCheat::LOG(a_Player.GetName() + "'s y speed was too high (speed=" + Printf("%f", a_DiffY) + ", max=0.5)");
			return true;
		}
	}

	return false;
}





bool cSpeedChecker::checkXZSpeed(cPlayer & a_Player, double a_DiffX, double a_DiffZ)
{
	if (cAntiCheat::m_Speed)
	{
		if (!cAntiCheat::isSpeedExempt(a_Player) && !a_Player.IsInVehicle())
		{
			AString reason = "";
			double max = 0.25;

			if (a_Player.GetWorld()->GetBlock(a_Player.GetPosX(), a_Player.GetPosY() - 1, a_Player.GetPosZ()) == E_BLOCK_SOULSAND)
			{
				if (a_Player.IsSprinting())
				{
					reason = "on soulsand while sprinting ";
					max = 0.2;
				}
				// TODO: Add Check for Speed Potion
				else
				{
					reason = "on soulsand ";
					max = 0.13;
				}
			}
			else if (a_Player.IsFlying())
			{
				reason = "while flying ";
				max = 0.56;
			}
			// TODO: Add Check for Speed Potion
			else if (a_Player.IsSprinting())
			{
				reason = "while sprinting ";
				max = 0.65;
			}

			float speed = (float)a_Player.GetSpeed().Length();
			max += speed > 0 ? (float)a_Player.GetSpeed().Length() - 0.2f : 0;

			if (a_DiffX > max || a_DiffZ > max)
			{
				int num = cAntiCheat::increment(a_Player, a_Player.m_SpeedViolation, 3);
				a_Player.m_SpeedViolation = num;
				if (num >= 3)
				{
					cAntiCheat::LOG(a_Player.GetName() + "'s speed was too high " + reason + Printf("%i", num) + " times in a row (max=3, speed=" + Printf("%f", (a_DiffX > a_DiffZ ? a_DiffX : a_DiffZ)) + ", max speed=" + Printf("%f", max) + ")");
					return true;
				}
			}
			else
			{
				a_Player.m_SpeedViolation = 0;
			}
		}
	}

	return false;
}





void cSpeedChecker::logVelocity(cPlayer & a_Player)
{
	a_Player.m_Velocitized = time(0);
}




