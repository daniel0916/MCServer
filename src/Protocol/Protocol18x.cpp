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
#include "../Root.h"
#include "../Server.h"
#include "../BlockEntities/CommandBlockEntity.h"
#include "../Entities/Pickup.h"




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
	cPacketizer Pkt(*this, 0x02);  // Chat Message packet
	Pkt.WriteString(Printf("{\"text\":\"%s\"}", EscapeString(a_Message).c_str()));
	Pkt.WriteByte(0);
}





void cProtocol180::SendWindowOpen(const cWindow & a_Window)
{
	if (a_Window.GetWindowType() < 0)
	{
		// Do not send this packet for player inventory windows
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





void cProtocol180::SendPlayerMoveLook(void)
{
	cPacketizer Pkt(*this, 0x08);  // Player Position And Look packet
	Pkt.WriteDouble(m_Client->GetPlayer()->GetPosX());

	// Protocol docs say this is PosY, but #323 says this is eye-pos
	// Moreover, the "+ 0.001" is there because otherwise the player falls through the block they were standing on.
	Pkt.WriteDouble(m_Client->GetPlayer()->GetStance() + 0.001);

	Pkt.WriteDouble(m_Client->GetPlayer()->GetPosZ());
	Pkt.WriteFloat((float)m_Client->GetPlayer()->GetYaw());
	Pkt.WriteFloat((float)m_Client->GetPlayer()->GetPitch());

	//Add Flags (currently only x)
	Pkt.WriteByte(0x01);

}





void cProtocol180::SendLogin(const cPlayer & a_Player, const cWorld & a_World)
{
	// Send the Join Game packet:
	{
		cPacketizer Pkt(*this, 0x01);  // Join Game packet
		Pkt.WriteInt(a_Player.GetUniqueID());
		Pkt.WriteByte((Byte)a_Player.GetEffectiveGameMode() | (cRoot::Get()->GetServer()->IsHardcore() ? 0x08 : 0)); // Hardcore flag bit 4
		Pkt.WriteChar((char)a_World.GetDimension());
		Pkt.WriteByte(2);  // TODO: Difficulty (set to Normal)
		Pkt.WriteByte(cRoot::Get()->GetServer()->GetMaxPlayers());
		Pkt.WriteString("default");  // Level type - wtf?
	}

	// Send the spawn position:
	{
		cPacketizer Pkt(*this, 0x05);  // Spawn Position packet
		Pkt.WritePosition((int)a_World.GetSpawnX(), (int)a_World.GetSpawnY(), (int)a_World.GetSpawnZ());
	}

	// Send player abilities:
	SendPlayerAbilities();
}





void cProtocol180::SendServerDifficulty(Byte difficulty)
{
	cPacketizer Pkt(*this, 0x41);  // Server Difficulty packet
	Pkt.WriteByte(difficulty);
}





void cProtocol180::SendUseBed(const cEntity & a_Entity, int a_BlockX, int a_BlockY, int a_BlockZ)
{
	cPacketizer Pkt(*this, 0x0a);
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WritePosition(a_BlockX, a_BlockY, a_BlockZ);
}





void cProtocol180::SendBlockChange(int a_BlockX, int a_BlockY, int a_BlockZ, BLOCKTYPE a_BlockType, NIBBLETYPE a_BlockMeta)
{
	cPacketizer Pkt(*this, 0x23);  // Block Change packet
	Pkt.WritePosition(a_BlockX, a_BlockY, a_BlockZ);
	Pkt.WriteVarInt(a_BlockType);
	Pkt.WriteByte(a_BlockMeta);
}





void cProtocol180::SendBlockAction(int a_BlockX, int a_BlockY, int a_BlockZ, char a_Byte1, char a_Byte2, BLOCKTYPE a_BlockType)
{
	cPacketizer Pkt(*this, 0x24);  // Block Action packet
	Pkt.WritePosition(a_BlockX, a_BlockY, a_BlockZ);
	Pkt.WriteByte(a_Byte1);
	Pkt.WriteByte(a_Byte2);
	Pkt.WriteVarInt(a_BlockType);
}





void cProtocol180::SendBlockBreakAnim(int a_EntityID, int a_BlockX, int a_BlockY, int a_BlockZ, char a_Stage)
{
	cPacketizer Pkt(*this, 0x25);  // Block Break Animation packet
	Pkt.WriteVarInt(a_EntityID);
	Pkt.WritePosition(a_BlockX, a_BlockY, a_BlockZ);
	Pkt.WriteChar(a_Stage);
}





void cProtocol180::SendSoundParticleEffect(int a_EffectID, int a_SrcX, int a_SrcY, int a_SrcZ, int a_Data)
{
	cPacketizer Pkt(*this, 0x28);  // Effect packet
	Pkt.WriteInt(a_EffectID);
	Pkt.WritePosition(a_SrcX, a_SrcY, a_SrcZ);
	Pkt.WriteInt(a_Data);
	Pkt.WriteBool(false);
}





void cProtocol180::SendUpdateSign(int a_BlockX, int a_BlockY, int a_BlockZ, const AString & a_Line1, const AString & a_Line2, const AString & a_Line3, const AString & a_Line4)
{
	cPacketizer Pkt(*this, 0x33);
	Pkt.WritePosition(a_BlockX, a_BlockY, a_BlockZ);
	// Need to send only up to 15 chars, otherwise the client crashes (#598)
	Pkt.WriteString(a_Line1.substr(0, 15));
	Pkt.WriteString(a_Line2.substr(0, 15));
	Pkt.WriteString(a_Line3.substr(0, 15));
	Pkt.WriteString(a_Line4.substr(0, 15));
}





void cProtocol180::SendUpdateBlockEntity(cBlockEntity & a_BlockEntity)
{
	cPacketizer Pkt(*this, 0x35);  // Update tile entity packet
	Pkt.WritePosition(a_BlockEntity.GetPosX(), a_BlockEntity.GetPosY(), a_BlockEntity.GetPosZ());

	Byte Action = 0;
	switch (a_BlockEntity.GetBlockType())
	{
	case E_BLOCK_MOB_SPAWNER:   Action = 1; break; // Update mob spawner spinny mob thing
	case E_BLOCK_COMMAND_BLOCK: Action = 2; break; // Update command block text
	default: ASSERT(!"Unhandled or unimplemented BlockEntity update request!"); break;
	}
	Pkt.WriteByte(Action);

	Pkt.WriteBlockEntity(a_BlockEntity);
}





void cProtocol180::SendEditSign(int a_BlockX, int a_BlockY, int a_BlockZ)
{
	cPacketizer Pkt(*this, 0x36);  // Sign Editor Open packet
	Pkt.WritePosition(a_BlockX, a_BlockY, a_BlockZ);
}





void cProtocol180::SendEntityEquipment(const cEntity & a_Entity, short a_SlotNum, const cItem & a_Item)
{
	cPacketizer Pkt(*this, 0x04);  // Entity Equipment packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteShort(a_SlotNum);
	Pkt.WriteItem(a_Item);
}





void cProtocol180::SendHealth(void)
{
	cPacketizer Pkt(*this, 0x06);  // Update Health packet
	Pkt.WriteFloat((float)m_Client->GetPlayer()->GetHealth());
	Pkt.WriteVarInt(m_Client->GetPlayer()->GetFoodLevel());
	Pkt.WriteFloat((float)m_Client->GetPlayer()->GetFoodSaturationLevel());
}





void cProtocol180::SendCollectPickup(const cPickup & a_Pickup, const cPlayer & a_Player)
{
	cPacketizer Pkt(*this, 0x0d);  // Collect Item packet
	Pkt.WriteVarInt(a_Pickup.GetUniqueID());
	Pkt.WriteVarInt(a_Player.GetUniqueID());
}





void cProtocol180::SendEntityVelocity(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x12);  // Entity Velocity packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	// 400 = 8000 / 20 ... Conversion from our speed in m/s to 8000 m/tick
	Pkt.WriteShort((short)(a_Entity.GetSpeedX() * 400));
	Pkt.WriteShort((short)(a_Entity.GetSpeedY() * 400));
	Pkt.WriteShort((short)(a_Entity.GetSpeedZ() * 400));
}





void cProtocol180::SendDestroyEntity(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x13);  // Destroy Entities packet
	Pkt.WriteVarInt(1);
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
}





void cProtocol180::SendEntityRelMove(const cEntity & a_Entity, char a_RelX, char a_RelY, char a_RelZ)
{
	cPacketizer Pkt(*this, 0x15);  // Entity Relative Move packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteByte(a_RelX);
	Pkt.WriteByte(a_RelY);
	Pkt.WriteByte(a_RelZ);
}





void cProtocol180::SendEntityLook(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x16);  // Entity Look packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteByteAngle(a_Entity.GetYaw());
	Pkt.WriteByteAngle(a_Entity.GetPitch());
}





void cProtocol180::SendEntityRelMoveLook(const cEntity & a_Entity, char a_RelX, char a_RelY, char a_RelZ)
{
	cPacketizer Pkt(*this, 0x17);  // Entity Look And Relative Move packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteByte(a_RelX);
	Pkt.WriteByte(a_RelY);
	Pkt.WriteByte(a_RelZ);
	Pkt.WriteByteAngle(a_Entity.GetYaw());
	Pkt.WriteByteAngle(a_Entity.GetPitch());
}





void cProtocol180::SendTeleportEntity(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x18);
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteFPInt(a_Entity.GetPosX());
	Pkt.WriteFPInt(a_Entity.GetPosY());
	Pkt.WriteFPInt(a_Entity.GetPosZ());
	Pkt.WriteByteAngle(a_Entity.GetYaw());
	Pkt.WriteByteAngle(a_Entity.GetPitch());
}





void cProtocol180::SendEntityHeadLook(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x19);  // Entity Head Look packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteByteAngle(a_Entity.GetHeadYaw());
}





void cProtocol180::SendEntityMetadata(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x1c);  // Entity Metadata packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteEntityMetadata(a_Entity);
	Pkt.WriteByte(0x7f);  // The termination byte
}





void cProtocol180::SendEntityEffect(const cEntity & a_Entity, int a_EffectID, int a_Amplifier, short a_Duration)
{
	cPacketizer Pkt(*this, 0x1D);  // Entity Effect packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteByte(a_EffectID);
	Pkt.WriteByte(a_Amplifier);
	Pkt.WriteVarInt(a_Duration);
}





void cProtocol180::SendRemoveEntityEffect(const cEntity & a_Entity, int a_EffectID)
{
	cPacketizer Pkt(*this, 0x1E);
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteByte(a_EffectID);
}





void cProtocol180::SendExperience(void)
{
	cPacketizer Pkt(*this, 0x1F); //Experience Packet
	Pkt.WriteFloat(m_Client->GetPlayer()->GetXpPercentage());
	Pkt.WriteVarInt(m_Client->GetPlayer()->GetXpLevel());
	Pkt.WriteVarInt(m_Client->GetPlayer()->GetCurrentXp());
}





void cProtocol180::SendEntityProperties(const cEntity & a_Entity)
{
	cPacketizer Pkt(*this, 0x20);  // Entity Properties packet
	Pkt.WriteVarInt(a_Entity.GetUniqueID());
	Pkt.WriteEntityProperties(a_Entity);
}





void cProtocol180::SendPlayerListItem(const cPlayer & a_Player, bool a_IsOnline)
{
	cPacketizer Pkt(*this, 0x38);  // Playerlist Item packet
	Pkt.WriteString(a_Player.GetName());
	Pkt.WriteBool(a_IsOnline);
	Pkt.WriteVarInt(a_IsOnline ? a_Player.GetClientHandle()->GetPing() : 0);
}





void cProtocol180::SendScoreUpdate(const AString & a_Objective, const AString & a_Player, cObjective::Score a_Score, Byte a_Mode)
{
	cPacketizer Pkt(*this, 0x3C);
	Pkt.WriteString(a_Player);
	Pkt.WriteByte(a_Mode);

	if (a_Mode != 1)
	{
		Pkt.WriteString(a_Objective);
		Pkt.WriteVarInt((int)a_Score);
	}
}





void cProtocol180::SendCombat(void)
{
	cPacketizer Pkt(*this, 0x42);

	// TODO: Add Combat Event

}





void cProtocol180::SendKeepAlive(int a_PingID)
{
	cPacketizer Pkt(*this, 0x00);  // Keep Alive packet
	Pkt.WriteInt(a_PingID);
}





void cProtocol180::SendPlayerMaxSpeed(void)
{
	cPacketizer Pkt(*this, 0x20);  // Entity Properties
	Pkt.WriteVarInt(m_Client->GetPlayer()->GetUniqueID());
	Pkt.WriteInt(1);  // Count
	Pkt.WriteString("generic.movementSpeed");
	Pkt.WriteDouble(0.1);
	if (m_Client->GetPlayer()->IsSprinting())
	{
		Pkt.WriteVarInt(1);  // Modifier count
		Pkt.WriteInt64(0x662a6b8dda3e4c1c);
		Pkt.WriteInt64(0x881396ea6097278d);  // UUID of the modifier
		Pkt.WriteDouble(0.3);
		Pkt.WriteByte(2);
	}
	else
	{
		Pkt.WriteVarInt(0);  // Modifier count
	}
}





void cProtocol180::HandlePacketClientSettings(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadVarUTF8String, AString, Locale);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ViewDistance);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ChatFlags);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, ChatColors);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, DisplayedSkinParts);
	// TODO: handle in m_Client
}





void cProtocol180::HandlePacketBlockDig(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, Status);
	HANDLE_READ(a_ByteBuffer, ReadBEInt64, Int64, Position);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, Face);

	int BlockX = Position >> 38;
	int BlockY = Position << 26 >> 52;
	int BlockZ = Position << 38 >> 38;

	m_Client->HandleLeftClick(BlockX, BlockY, BlockZ, Face, Status);
}





void cProtocol180::HandlePacketBlockPlace(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadBEInt64, Int64, Position);
	int BlockX = Position >> 38;
	int BlockY = Position << 26 >> 52;
	int BlockZ = Position << 38 >> 38;

	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, Face);
	cItem Item;
	ReadItem(a_ByteBuffer, Item);

	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, CursorX);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, CursorY);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, CursorZ);
	m_Client->HandleRightClick(BlockX, BlockY, BlockZ, Face, CursorX, CursorY, CursorZ, m_Client->GetPlayer()->GetEquippedItem());
}





void cProtocol180::HandlePacketUpdateSign(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadBEInt64, Int64, Position);
	int BlockX = Position >> 38;
	int BlockY = Position << 26 >> 52;
	int BlockZ = Position << 38 >> 38;

	HANDLE_READ(a_ByteBuffer, ReadVarUTF8String, AString, Line1);
	HANDLE_READ(a_ByteBuffer, ReadVarUTF8String, AString, Line2);
	HANDLE_READ(a_ByteBuffer, ReadVarUTF8String, AString, Line3);
	HANDLE_READ(a_ByteBuffer, ReadVarUTF8String, AString, Line4);
	m_Client->HandleUpdateSign(BlockX, BlockY, BlockZ, Line1, Line2, Line3, Line4);
}





void cProtocol180::HandlePacketKeepAlive(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadVarInt, UInt32, KeepAliveID);
	m_Client->HandleKeepAlive(KeepAliveID);
}





void cProtocol180::HandlePacketUseEntity(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadVarInt, UInt32, EntityID);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, MouseButton);
	m_Client->HandleUseEntity(EntityID, (MouseButton == 1));
}





void cProtocol180::HandlePacketAnimation(cByteBuffer & a_ByteBuffer)
{
	// Removed all from it (made with 14w04a)
}





void cProtocol180::HandlePacketEntityAction(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadVarInt, UInt32, PlayerID);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, Action);
	HANDLE_READ(a_ByteBuffer, ReadVarInt, UInt32, JumpBoost);
	m_Client->HandleEntityAction18(PlayerID, Action);
}





void cProtocol180::HandlePacketSteerVehicle(cByteBuffer & a_ByteBuffer)
{
	HANDLE_READ(a_ByteBuffer, ReadBEFloat, float, Forward);
	HANDLE_READ(a_ByteBuffer, ReadBEFloat, float, Sideways);
	HANDLE_READ(a_ByteBuffer, ReadByte, Byte, Flags);

	if (Flags == 0x2)
	{
		m_Client->HandleUnmount();
	}
	else
	{
		m_Client->HandleSteerVehicle(Forward, Sideways);
	}
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cProtocol172::cPacketizer:
void cProtocol172::cPacketizer::WriteEntityProperties18(const cEntity & a_Entity)
{
	if (!a_Entity.IsMob())
	{
		// No properties for anything else than mobs
		WriteVarInt(0);
		return;
	}
	const cMonster & Mob = (const cMonster &)a_Entity;

	// TODO: Send properties and modifiers based on the mob type

	WriteVarInt(0);  // NumProperties
}





