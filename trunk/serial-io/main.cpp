/**
 * Serial-IO
 * Copyright (C) 2007  Wilfried Holzke
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License only.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <QApplication>

#include "mainwindow.h"


int main(int argc,char **argv)
{
	QApplication application(argc,argv);
	
	QCoreApplication::setOrganizationName("XiTech");
    QCoreApplication::setOrganizationDomain("serial-io.sourceforge.net");
    QCoreApplication::setApplicationName("serial-io");	
	
    MainWindow mainWin;
    mainWin.show();

	return application.exec();
}

