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

#include "teesettings.h"
#include "common.h"
#include <QFileDialog>
#include <QSettings>

TeeSettings::TeeSettings(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	QSettings settings(COMPANYNAME, APPNAME);
	ui.lineEdit->setText(settings.value(TEEWARSPATHKEY, "").toString());
}

TeeSettings::~TeeSettings()
{

}


void TeeSettings::on_pushButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Teewars location"), "", tr("exe files (*.exe)"));
	ui.lineEdit->setText(fileName);
	QSettings settings(COMPANYNAME, APPNAME);
	settings.setValue(TEEWARSPATHKEY, fileName);
}

void TeeSettings::on_pushButton_2_clicked()
{
	hide();
}