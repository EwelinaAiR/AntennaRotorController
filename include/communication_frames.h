#pragma once

struct CmdMaster
{
	uint8_t cmd;
	uint8_t remoteControl;
	uint16_t desiredAngle;
};

struct CmdSlave
{
	uint16_t encoderRawData;
	uint16_t status;
};
