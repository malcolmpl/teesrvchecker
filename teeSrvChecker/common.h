/*
The MIT License

Copyright (c) 2007 Dariusz Mikulski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __COMMON_H
#define __COMMON_H

#include <QtNetwork/QUdpSocket>
#include <QString>
#include <QEvent>
#include "VersionNo.h"

const QString APPVERSION				= TSVN_VERMAJOR_STR "." TSVN_VERMINOR_STR "." TSVN_VERMICRO_STR " rev. " TSVN_VERBUILD_STR;
const QString COMPANYNAME				= "sourcecode.pl";
const QString APPNAME					= "teeWars Server Checker";
const QString TEEWARSPATHKEY			= "application/teewarspath";
const QString RESOURCE_ICON				= "res\\icon.png";
const QString WINDOW_TITLE				= "teeWars Servers Checker ver: ";
const QString MAX_IP					= "255.255.255.255:65535";

const QString MASTERSERVERIP			= "82.196.163.133";
const quint16 MASTERSERVERPORT			= 8383;
const quint32 DUMMY1					= 0x00000000;
const quint32 DUMMY2					= 0xffffffff;

const quint32 GIEF						= 0x67696566;
const quint32 INFO						= 0x696e666f;
const quint32 REQT						= 0x72657174;
const quint32 LIST						= 0x6c697374;
const quint16 PROTO_ID					= 0x2000;

const int GAME_SERVER_TIMER				= 2000;
const int GAME_SERVER_ALTERNATE_TIMER	= 1000;
const int WINDOW_WIDTH					= 599;
const int WINDOW_HEIGHT					= 400;
const int MIN_PING_TO_RED_COLOR			= 100;
const int MASTER_SERVER_TIMER			= 30000;
const int WAIT_FOR_READ_DATAGRAM		= 1000;
const int WAIT_FOR_CONNECTED			= 100;

struct teeHeader
{
	quint16 proto_id;
	quint32 dummy1;
	quint32 dummy2;
	quint32 packet_id;
};

struct teeInfo
{
	char *serverName;
	char *mapName;
};

struct gameServer
{
	gameServer()
		: serverName(""), mapName(""), maxPlayers(0), currPlayers(0), ms(0)
	{

	}

	QHostAddress address;
	quint16 port;

	QString serverName;
	QString mapName;
	quint8 maxPlayers;
	quint8 currPlayers;
	int ms;
};


class CheckServerEvent : public QEvent
{
public:
	CheckServerEvent(gameServer g)
		: QEvent(QEvent::User)
	{
		gameSrv = g;
	}

	gameServer getGameServer()
	{
		return gameSrv;
	}

private:
	gameServer gameSrv;
};


#endif // __COMMON_H