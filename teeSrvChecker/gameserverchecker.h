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

#ifndef GAMESERVERCHECKER_H
#define GAMESERVERCHECKER_H

#include <QtNetwork/QUdpSocket>
#include <QThread>
#include <QEvent>
#include <QTime>
#include "common.h"

class GameServerChecker : public QThread
{
	Q_OBJECT

public:
	GameServerChecker();
	~GameServerChecker();

	void run();
	bool event(QEvent* event);

signals:
	void serverInfo(gameServer g, QString serverName, QString mapName, quint8 maxPlayers, quint8 currPlayers, int ms);
	void serverError(gameServer g);
	void nextServer();

private:
	gameServer gameSrv;
	QUdpSocket *gameServerSocket;
	std::list<gameServer> gameServerList;
	QTime tm;

	void readServerInfo();

private slots:
	void readServerInfoDiagrams();
	void slotDisconnected();
	void slotConnected();
	void slotError(QAbstractSocket::SocketError socketError);
};

#endif // GAMESERVERCHECKER_H
