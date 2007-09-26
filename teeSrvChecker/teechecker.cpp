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

#include "teechecker.h"
#include <QString>
#include <QTimer>
#include <QHeaderView>
#include <QTreeWidget>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <algorithm>

bool operator==(const gameServer &a, const gameServer &b)
{
	if((a.address == b.address) && (a.port == b.port))
		return true;
	return false;
}

teeChecker::teeChecker(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	aboutDialog.hide();
	tray = new TeeTrayIcon(this);
	masterCheckerThread = NULL;
	gameServerChecker = NULL;
	mutex = new QMutex();
	bServerChecked = false;
	connect(ui.actionClose, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui.actionAbout, SIGNAL(triggered()), &aboutDialog, SLOT(exec()));
	connect(ui.actionSettings, SIGNAL(triggered()), &settingsDialog, SLOT(exec()));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem *, int)));

	qApp->setWindowIcon(QIcon(RESOURCE_ICON));
	setWindowTitle(WINDOW_TITLE + APPVERSION);

	setupTable();
	setupStatusBar();
}

teeChecker::~teeChecker()
{
}

void teeChecker::on_buttonClose_clicked()
{
	QCoreApplication::quit();
}

void teeChecker::on_buttonConnect_clicked()
{
	if(!masterCheckerThread)
	{
		masterCheckerThread = new masterChecker();
		qRegisterMetaType<gameServer>("gameServer");
		connect(masterCheckerThread, SIGNAL(newGameServer(gameServer)), this, SLOT(slotNewGameServer(gameServer)), Qt::QueuedConnection);
		masterCheckerThread->moveToThread(masterCheckerThread);
		masterCheckerThread->start();
	}

	if(!gameServerChecker)
	{
		gameServerChecker = new GameServerChecker();
		connect(gameServerChecker, SIGNAL(serverInfo(gameServer, QString, QString, quint8, quint8, int)), 
			this, SLOT(slotServerInfo(gameServer, QString, QString, quint8, quint8, int)));
		connect(gameServerChecker, SIGNAL(nextServer()), this, SLOT(slotUpdate()));
		connect(gameServerChecker, SIGNAL(serverError(gameServer)), this, SLOT(slotServerError(gameServer)));
		gameServerChecker->moveToThread(gameServerChecker);
		gameServerChecker->start();

		iter = gameServerList.begin();
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
		timer->start(GAME_SERVER_TIMER); 
	}
}

void teeChecker::slotNewGameServer(gameServer g)
{
	mutex->lock();

	std::list<gameServer>::iterator i;
	i = std::find(gameServerList.begin(),
				gameServerList.end(), 
				g);

	if(i == gameServerList.end())
	{
		gameServerList.push_back(g);
	}

	mutex->unlock();
}

void teeChecker::slotServerInfo(gameServer g, QString serverName, QString mapName, quint8 maxPlayers, quint8 currPlayers, int ms)
{
	mutex->lock();
	std::list<gameServer>::iterator i;
	i = std::find(gameServerList.begin(),
		gameServerList.end(), 
		g);

	if(i != gameServerList.end())
	{
		(*i).serverName = serverName;
		(*i).mapName = mapName;
		(*i).maxPlayers = maxPlayers;
		(*i).currPlayers = currPlayers;
		(*i).ms = ms;
	}

	QTreeWidget *tree = ui.treeWidget;
	QString stringToFind = g.address.toString() + ":" + QString::number(g.port);
	QList<QTreeWidgetItem*> items = tree->findItems(stringToFind, Qt::MatchFixedString);
	if(items.size()>0)
	{
		TeeTreeWidgetItem *item = (TeeTreeWidgetItem*)items.at(0);
		item->setText(0, stringToFind);
		item->setText(1, serverName);
		item->setText(2, mapName);
		QString strPlayers = QString::number(currPlayers) + "/" + QString::number(maxPlayers);
		item->setText(3, strPlayers);
		item->setText(4, QString::number(ms));
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		setItemColor(item, maxPlayers, currPlayers, ms);
	}
	else
	{
		QTreeWidget *tree = ui.treeWidget;
		TeeTreeWidgetItem *item = new TeeTreeWidgetItem(tree);
		item->setText(0, g.address.toString() + ":" + QString::number(g.port));
		item->setText(1, serverName);
		item->setText(2, mapName);
		QString strPlayers = QString::number(currPlayers) + "/" + QString::number(maxPlayers);
		item->setText(3, strPlayers);
		item->setText(4, QString::number(ms));
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tree->insertTopLevelItem(0, item);
		setItemColor(item, maxPlayers, currPlayers, ms);
	}

	bServerChecked = false;
	totalSrvCount->setText(QString::number(tree->topLevelItemCount()));
	int withPlayers = 0;
	int nTotPlayers = 0;
	for(i = gameServerList.begin(); i != gameServerList.end(); i++)
	{
		if((*i).currPlayers != 0)
		{
			withPlayers++;
			nTotPlayers += (*i).currPlayers;
		}
	}
	srvWithPlayersCount->setText(QString::number(withPlayers));
	totalPlayersCount->setText(QString::number(nTotPlayers));

	mutex->unlock();
}

void teeChecker::slotUpdate()
{
	mutex->lock();
	if(!bServerChecked && iter != gameServerList.end())
	{
		qApp->postEvent(gameServerChecker, new CheckServerEvent((*iter)));
		bServerChecked = true;

		iter++;
	}
	else if(iter == gameServerList.end())
	{
		iter = gameServerList.begin();
		QTimer::singleShot(GAME_SERVER_ALTERNATE_TIMER, this, SLOT(slotUpdate()));
	}
	mutex->unlock();
}

void teeChecker::slotServerError(gameServer g)
{
	mutex->lock();
	bServerChecked = false;

	std::list<gameServer>::iterator i;
	i = std::find(gameServerList.begin(),
		gameServerList.end(), 
		g);
	if(i != gameServerList.end())
	{
		iter = gameServerList.erase(i);
		
		QTreeWidget *tree = ui.treeWidget;
		QString stringToFind = g.address.toString() + ":" + QString::number(g.port);
		QList<QTreeWidgetItem*> items = tree->findItems(stringToFind, Qt::MatchFixedString);
		for(int j=0; j<items.size(); j++)
		{
			delete items.at(j);
		}
	}
	mutex->unlock();
}

void teeChecker::setupTable()
{
	QStringList headers;
	headers << tr("IP") << tr("Server name") << tr("Map name")
		<< tr("Players") << tr("Ping");

	QTreeWidget *tree = ui.treeWidget;
	tree->setHeaderLabels(headers);
	tree->header()->setClickable(true);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setAlternatingRowColors(true);
	tree->setRootIsDecorated(false);
	tree->setSortingEnabled(true);
	tree->sortItems(3, Qt::DescendingOrder);

	connect(tree->header(), SIGNAL(sectionClicked(int)), this, SLOT(treeColumnClicked(int)));

	QFontMetrics fm = fontMetrics();
	QHeaderView *header = tree->header();
	int typicalWidth = fm.width(MAX_IP);
	header->resizeSection(0, typicalWidth);
	header->resizeSection(1, fm.width(headers.at(1)) + typicalWidth);
	header->resizeSection(2, fm.width(headers.at(2) + "      "));
	header->resizeSection(3, fm.width(headers.at(3) + "      "));
	header->resizeSection(4, fm.width(headers.at(4)));
}

void teeChecker::treeColumnClicked(int index)
{
	QTreeWidget *tree = ui.treeWidget;
	tree->sortByColumn(index);
}

void teeChecker::resizeEvent(QResizeEvent *event)
{
	setFixedSize(QSize(WINDOW_WIDTH, WINDOW_HEIGHT));
	repaint();
}

void teeChecker::setItemColor(TeeTreeWidgetItem *item, int maxPlayers, int currPlayers, int ms)
{
	for(int j=0; j<5; j++)
	{
		item->setTextColor(j, Qt::black);
		if(currPlayers != 0)
			item->setTextColor(j, Qt::darkGreen);
		if(currPlayers == maxPlayers)
			item->setTextColor(j, Qt::blue);
		if(ms>MIN_PING_TO_RED_COLOR)
			item->setTextColor(j, Qt::red);
	}
}

void teeChecker::setupStatusBar()
{
	bar = statusBar();
	totalSrv = new QLabel(bar);
	totalSrv->setText(tr("Total servers:"));
	bar->addWidget(totalSrv, totalSrv->width());

	totalSrvCount = new QLabel(bar);
	totalSrvCount->setText("");
	bar->addWidget(totalSrvCount, totalSrvCount->width());

	srvWithPlayers = new QLabel(bar);
	srvWithPlayers->setText(tr("Not empty:"));
	bar->addWidget(srvWithPlayers, srvWithPlayers->width());

	srvWithPlayersCount = new QLabel(bar);
	srvWithPlayersCount->setText("   ");
	bar->addWidget(srvWithPlayersCount, srvWithPlayersCount->width());

	totalPlayers = new QLabel(bar);
	totalPlayers->setText(tr(" Total players:"));
	bar->addWidget(totalPlayers, totalPlayers->width());

	totalPlayersCount = new QLabel(bar);
	totalPlayersCount->setText("");
	bar->addWidget(totalPlayersCount, totalPlayersCount->width());
}

void teeChecker::slotItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	QSettings settings(COMPANYNAME, APPNAME);

	QFileInfo directory(settings.value(TEEWARSPATHKEY, "").toString());
	QString program = directory.absoluteFilePath();
	QStringList arguments;
	arguments << "-c" << item->text(0);

	QProcess *myProcess = new QProcess();
	myProcess->setWorkingDirectory(directory.absolutePath());
	myProcess->start(program, arguments);
}