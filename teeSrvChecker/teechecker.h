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

#ifndef TEECHECKER_H
#define TEECHECKER_H

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMutex>
#include <QTimer>
#include <QTime>
#include <QStatusBar>
#include <QLabel>
#include <list>
#include "masterchecker.h"
#include "gameserverchecker.h"
#include "ui_teechecker.h"
#include "common.h"
#include "teeabout.h"
#include "teesettings.h"
#include "teetrayicon.h"
#include "teetreewidgetitem.h"

class teeChecker : public QMainWindow
{
	Q_OBJECT

public:
	teeChecker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~teeChecker();

private:
	Ui::teeCheckerClass ui;
	void readServerInfo(QHostAddress addr, qint32 port);

	void setupTable();
	void setupStatusBar();
	void setItemColor(TeeTreeWidgetItem *item, int maxPlayers, int currPlayers, int ms);

	TeeTrayIcon *tray;
	QMenu *menu;
	QAction *closeAction;
	QMutex *mutex;
	QTimer *timer;
	bool bServerChecked;
	teeAbout aboutDialog;
	TeeSettings settingsDialog;

	QStatusBar *bar;
	QLabel *totalSrv;
	QLabel *totalSrvCount;
	QLabel *srvWithPlayers;
	QLabel *srvWithPlayersCount;
	QLabel *totalPlayers;
	QLabel *totalPlayersCount;

	std::list<gameServer> gameServerList;
	std::list<gameServer>::iterator iter;

	masterChecker *masterCheckerThread;
	GameServerChecker *gameServerChecker;

protected:
	void resizeEvent(QResizeEvent *event);

private slots:
	void on_buttonConnect_clicked();
	void on_buttonClose_clicked();
	void slotNewGameServer(gameServer g);
	void slotServerInfo(gameServer g, QString serverName, QString mapName, quint8 maxPlayers, quint8 currPlayers, int ms);
	void slotServerError(gameServer g);
	void slotUpdate();
	void treeColumnClicked(int index);
	void slotItemDoubleClicked(QTreeWidgetItem *, int);
};

#endif // TEECHECKER_H
