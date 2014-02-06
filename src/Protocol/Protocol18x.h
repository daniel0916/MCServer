
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
	virtual void SendPlayerMoveLook(void) override;
	virtual void SendServerDifficulty(Byte difficulty);

	virtual void SendLogin(const cPlayer & a_Player, const cWorld & a_World) override;
	virtual void SendUseBed(const cEntity & a_Entity, int a_BlockX, int a_BlockY, int a_BlockZ) override;
	virtual void SendBlockChange(int a_BlockX, int a_BlockY, int a_BlockZ, BLOCKTYPE a_BlockType, NIBBLETYPE a_BlockMeta) override;
	virtual void SendBlockAction(int a_BlockX, int a_BlockY, int a_BlockZ, char a_Byte1, char a_Byte2, BLOCKTYPE a_BlockType) override;
	virtual void SendBlockBreakAnim(int a_EntityID, int a_BlockX, int a_BlockY, int a_BlockZ, char a_Stage) override;
	virtual void SendSoundParticleEffect(int a_EffectID, int a_SrcX, int a_SrcY, int a_SrcZ, int a_Data) override;
	virtual void SendUpdateSign(int a_BlockX, int a_BlockY, int a_BlockZ, const AString & a_Line1, const AString & a_Line2, const AString & a_Line3, const AString & a_Line4) override;
	virtual void SendUpdateBlockEntity(cBlockEntity & a_BlockEntity) override;
	virtual void SendEditSign(int a_BlockX, int a_BlockY, int a_BlockZ) override;

	virtual void SendEntityEquipment(const cEntity & a_Entity, short a_SlotNum, const cItem & a_Item) override;
	virtual void SendHealth(void) override;
	virtual void SendCollectPickup(const cPickup & a_Pickup, const cPlayer & a_Player) override;
	virtual void SendEntityVelocity(const cEntity & a_Entity) override;
	virtual void SendDestroyEntity(const cEntity & a_Entity) override;
	virtual void SendEntityRelMove(const cEntity & a_Entity, char a_RelX, char a_RelY, char a_RelZ) override;
	virtual void SendEntityLook(const cEntity & a_Entity) override;
	virtual void SendEntityRelMoveLook(const cEntity & a_Entity, char a_RelX, char a_RelY, char a_RelZ) override;
	virtual void SendTeleportEntity(const cEntity & a_Entity) override;
	virtual void SendEntityHeadLook(const cEntity & a_Entity) override;
	virtual void SendEntityMetadata(const cEntity & a_Entity) override;
	virtual void SendEntityEffect(const cEntity & a_Entity, int a_EffectID, int a_Amplifier, short a_Duration) override;
	virtual void SendRemoveEntityEffect(const cEntity & a_Entity, int a_EffectID) override;
	virtual void SendExperience(void) override;
	virtual void SendEntityProperties(const cEntity & a_Entity) override;
	virtual void SendPlayerListItem(const cPlayer & a_Player, bool a_IsOnline) override;
	virtual void SendScoreUpdate(const AString & a_Objective, const AString & a_Player, cObjective::Score a_Score, Byte a_Mode) override;
	virtual void SendCombat(void);
	virtual void SendKeepAlive(int a_PingID) override;
	virtual void SendPlayerMaxSpeed(void) override;

protected:

	virtual void HandlePacketClientSettings(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketBlockDig(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketBlockPlace(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketUpdateSign(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketKeepAlive(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketUseEntity(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketAnimation(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketEntityAction(cByteBuffer & a_ByteBuffer) override;
	virtual void HandlePacketSteerVehicle(cByteBuffer & a_ByteBuffer) override;

};




