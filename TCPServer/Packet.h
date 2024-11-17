#pragma once

#ifndef __PACKET_H__
#define __PACKET_H__

#include <iostream>

unsigned short Length;

#pragma pack(push, 1)
typedef struct _packet
{
	std::string IP;
	unsigned short Port;
} Packet;
#pragma pack(pop)

#endif