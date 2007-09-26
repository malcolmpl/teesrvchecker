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

#include "teetrayicon.h"
#include "common.h"
#include <QCoreApplication>

TeeTrayIcon::TeeTrayIcon(QObject *parent)
: QObject(parent)
{
	mainWindow = (QMainWindow*)parent;
	tray = new QSystemTrayIcon(this);
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), 
		this, SLOT(activatedTriggered(QSystemTrayIcon::ActivationReason)));

	menu = new QMenu();
	closeAction = new QAction(tr("Close"), this);
	connect(closeAction, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));
	menu->addAction(closeAction);
	
	tray->setIcon(QIcon(RESOURCE_ICON));
	tray->setContextMenu(menu);
	tray->show();
}

TeeTrayIcon::~TeeTrayIcon()
{
	tray->hide();
}

void TeeTrayIcon::activatedTriggered(QSystemTrayIcon::ActivationReason reason)
{
	switch(reason)
	{
		case QSystemTrayIcon::DoubleClick:
		{
			mainWindow->show();
		}
		break;
	}
}