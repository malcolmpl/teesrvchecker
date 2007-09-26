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

#include "masterchecker.h"
#include <QTimer>

masterChecker::masterChecker()
{

}

masterChecker::~masterChecker()
{
	if(masterSocket)
		masterSocket->disconnectFromHost();
}

void masterChecker::run()
{
	masterSocket = new QUdpSocket();
	connect(masterSocket, SIGNAL(readyRead()), this, SLOT(readMasterDiagrams()), Qt::DirectConnection);
	connect(masterSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()), Qt::DirectConnection);

	masterSocket->connectToHost(MASTERSERVERIP, MASTERSERVERPORT);
	masterSocket->waitForConnected();

	QTimer::singleShot(0, this, SLOT(updateServersList()));
	exec();
}

void masterChecker::updateServersList()
{
	teeHeader header;
	header.proto_id = PROTO_ID;
	header.dummy1 = DUMMY1;
	header.dummy2 = DUMMY2;
	header.packet_id = REQT;
	QByteArray datagram;
	QDataStream out(&datagram, QIODevice::WriteOnly);
	out << header.proto_id << header.dummy1 << header.dummy2 << header.packet_id;
	masterSocket->writeDatagram(datagram, QHostAddress(MASTERSERVERIP), MASTERSERVERPORT);
	QTimer::singleShot(MASTER_SERVER_TIMER, this, SLOT(updateServersList()));
}

void masterChecker::readMasterDiagrams()
{
	while (masterSocket->hasPendingDatagrams())
	{
		QByteArray datagram;

		qint64 datagramSize = masterSocket->pendingDatagramSize();
		datagram.resize(datagramSize);
		QHostAddress sender;
		quint16 senderPort;

		masterSocket->readDatagram(datagram.data(), datagram.size(),
			&sender, &senderPort);

		QDataStream in(&datagram, QIODevice::ReadOnly);
		teeHeader header;
		in >> header.proto_id >> header.dummy1 >> header.dummy2 >> header.packet_id;

		if(header.packet_id == LIST)
		{
			int size = datagram.size();
			while(size>0)
			{
				gameServer game;
				qint32 addr;
				quint16 port;
				in >> addr;
				in.setByteOrder(QDataStream::LittleEndian);
				in >> port;
				in.setByteOrder(QDataStream::BigEndian);
				size = size - sizeof(addr) - sizeof(port);

				game.address = QHostAddress(addr);
				game.port = port;

				emit newGameServer(game);
			}
		}
	}
}

void masterChecker::slotDisconnected()
{
	quit();
}