/***************************************************************************
			mymaindialog.h  -  description
			-------------------
begin                : lun sep 26 2005
copyright            : (C) 2005 by houssem
email                : houssem@localhost
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef MYMAINDIALOG_H
#define MYMAINDIALOG_H

#include <maindialog.h>
#include <qtextstream.h>

#include "tts.h"

class AIMLParser;

/**
*@author houssem bdioui
*/

class MyMainDialog : public MainDialog
{
    Q_OBJECT
public:
    MyMainDialog(const QString&, QTextStream*, bool,
       bool, const QString&);
    ~MyMainDialog();
private:
    AIMLParser *parser;
    TTS *tts;
public slots:
  void validateInput();
};

#endif


