// Protocol18x.cpp

/*
Implements the 1.8.x protocol classes:
- cProtocol180
*/

#include "Globals.h"
#include "Protocol18x.h"
#include "../ClientHandle.h"
#include "../UI/Window.h"
#include "../Entities/Player.h"




#define HANDLE_READ(ByteBuf, Proc, Type, Var) \
	Type Var; \
	ByteBuf.Proc(Var);





#define HANDLE_PACKET_READ(ByteBuf, Proc, Type, Var) \
	Type Var; \
	{ \
if (!ByteBuf.Proc(Var)) \
		{ \
		ByteBuf.CheckValid(); \
		return false; \
		} \
		ByteBuf.CheckValid(); \
	}





const int MAX_ENC_LEN = 512;  // Maximum size of the encrypted message; should be 128, but who knows...





// fwd: main.cpp:
extern bool g_ShouldLogCommIn, g_ShouldLogCommOut;





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cProtocol180:

cProtocol180::cProtocol180(cClientHandle * a_Client, const AString & a_ServerAddress, UInt16 a_ServerPort, UInt32 a_State) :
	super(a_Client, a_ServerAddress, a_ServerPort, a_State)
{
}





void cProtocol180::SendChat(const AString & a_Message)
{
	LOGWARN("SendChat");
	cPacketizer Pkt(*this, 0x02);  // Chat Message packet
	Pkt.WriteString(Printf("{\"text\":\"%s\"}", EscapeString(a_Message).c_str()));
	Pkt.WriteByte(0);
}





void cProtocol180::SendWindowOpen(const cWindow & a_Window)
{
	LOGWARN("Inventory");
	if (a_Window.GetWindowType() < 0)
	{
		// Do not send this packet for player inventory windows
		LOGWARN("Player Inventory");
		return;
	}

	cPacketizer Pkt(*this, 0x2d);
	Pkt.WriteChar(a_Window.GetWindowID());
	Pkt.WriteString(std::to_string(a_Window.GetWindowType()));
	Pkt.WriteString(a_Window.GetWindowTitle());
	Pkt.WriteChar(a_Window.GetNumNonInventorySlots());
	Pkt.WriteBool(true);
	if (a_Window.GetWindowType() == cWindow::wtAnimalChest)
	{
		Pkt.WriteInt(0);  // TODO: The animal's EntityID
	}
}





void cProtocol180::HandlePacketClientSettings(cByteBuffer & a_ByteBuffer)
{
	LOGWARN("Client Settings");
	HANDLE_READ(a_ByteBuffer, ReadVarUTF8String, AString, Locale);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ViewDistance);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ChatFlags);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ChatColors);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ShowCape);
	// TODO: handle in m_Client
}





