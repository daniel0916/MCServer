
// Protocol18x.h

/*
Declares the 1.8.x protocol classes:
	- cProtocol180
*/





#pragma once

#include "Protocol17x.h"





class cProtocol180 :
	public cProtocol172
{
	typedef cProtocol172 super;

public:
	cProtocol180(cClientHandle * a_Client, const AString & a_ServerAddress, UInt16 a_ServerPort, UInt32 a_State);

	virtual void SendWindowOpen(const cWindow & a_Window) override;
	virtual void SendChat(const AString & a_Message) override;

protected:

	void HandlePacketClientSettings(cByteBuffer & a_ByteBuffer);

};




