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

#include "gameserverchecker.h"

GameServerChecker::GameServerChecker()
{
}

GameServerChecker::~GameServerChecker()
{
	if(gameServerSocket)
		delete gameServerSocket;
}

void GameServerChecker::run()
{
	gameServerSocket = new QUdpSocket();

	connect(gameServerSocket, SIGNAL(readyRead()), this, SLOT(readServerInfoDiagrams()), Qt::DirectConnection);
	connect(gameServerSocket, SIGNAL(connected()), this, SLOT(slotConnected()), Qt::DirectConnection);
	connect(gameServerSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()), Qt::DirectConnection);
	connect(gameServerSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)), Qt::DirectConnection);

	exec();
}

void GameServerChecker::readServerInfo()
{
	teeHeader header;
	header.flags = NETWORK_PACKETFLAG_CONNLESS;
	header.sequencing = SEQUENCING;
	header.ack = ACK;
	header.token = TOKEN;
	header.dummy1 = DUMMY1;
	header.packet_id = GIEF;
	QByteArray datagram;
	QDataStream out(&datagram, QIODevice::WriteOnly);
	out << header.flags << header.sequencing << header.ack
		<< header.token << header.dummy1 << header.packet_id;
	gameServerSocket->writeDatagram(datagram, gameSrv.address, gameSrv.port);
	tm.start();
	gameServerSocket->waitForReadyRead(WAIT_FOR_READ_DATAGRAM);
}

void GameServerChecker::readServerInfoDiagrams()
{
	while ((gameServerSocket->state() != QAbstractSocket::UnconnectedState) && (gameServerSocket->hasPendingDatagrams()))
	{
		int timeElapsed = tm.elapsed();
		QString serverName;
		QString mapName;
		QByteArray datagram;
		quint8 maxPlayers = 0;
		quint8 currPlayers = 0;

		qint64 datagramSize = gameServerSocket->pendingDatagramSize();
		datagram.resize(datagramSize);
		QHostAddress sender;
		quint16 senderPort;

		gameServerSocket->readDatagram(datagram.data(), datagram.size(),
			&sender, &senderPort);

		QDataStream in(&datagram, QIODevice::ReadOnly);
		teeHeader header;
		in >> header.flags >> header.sequencing >> header.ack >> header.token >> header.dummy1 >> header.packet_id;

		if(header.packet_id == INFO)
		{
			int size = datagram.size();
			char *readString = new char[size];
			in.readRawData(readString, size);
			serverName = readString;
			while(*readString != '\0')
			{
				readString++;
			}
			readString++;
			mapName = readString;
			while(*readString != '\0')
			{
				readString++;
			}
			readString++;
			maxPlayers = (quint8)*readString;
			readString++;
			currPlayers = (quint8)*readString;
		}
		emit serverInfo(gameSrv, serverName, mapName, maxPlayers, currPlayers, timeElapsed);
		gameServerSocket->disconnectFromHost();
	}
}

void GameServerChecker::slotDisconnected()
{
	emit nextServer();
}

void GameServerChecker::slotConnected()
{
}

void GameServerChecker::slotError(QAbstractSocket::SocketError socketError)
{
	emit serverError(gameSrv);
	gameServerSocket->disconnectFromHost();
}

bool GameServerChecker::event(QEvent *event)
{
	if(event->type() == QEvent::User)
	{
		CheckServerEvent *e = (CheckServerEvent*)event;
		gameSrv = e->getGameServer();
		gameServerSocket->connectToHost(gameSrv.address, gameSrv.port);
		if(!gameServerSocket->waitForConnected(WAIT_FOR_CONNECTED))
		{
			gameServerSocket->disconnectFromHost();
			return true;
		}
		readServerInfo();
		return true;
	}
	return QThread::event(event);
}