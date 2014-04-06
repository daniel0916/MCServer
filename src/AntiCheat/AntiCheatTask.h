
#pragma once

#include "../World.h"





class cAntiCheatTask :
	public cWorld::cTask
{
public:
	cAntiCheatTask(cPlayer & a_Player) :
		m_Player(a_Player)
	{
	}

protected:
	cPlayer & m_Player;

	virtual void Run(cWorld & a_World) override
	{
		m_Player.m_VelocityTrack = 0;
	}
};




