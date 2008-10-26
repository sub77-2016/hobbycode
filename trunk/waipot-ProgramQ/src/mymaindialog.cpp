/***************************************************************************
                          mymaindialog.cpp  -  description
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

#include "mymaindialog.h"
#include "aimlparser.h"

#include <qcombobox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qapplication.h>
#if QT_VERSION >= 0x040000
#include <q3progressdialog.h>
#define QPROGRESSDIALOG_CLASSNAME Q3ProgressDialog
#define QTEXTEDIT_CLASSNAME Q3TextEdit
#else
#include <qprogressdialog.h>
#define QPROGRESSDIALOG_CLASSNAME QProgressDialog
#define QTEXTEDIT_CLASSNAME QTextEdit
#endif

MyMainDialog::MyMainDialog(const QString& aimlSet, QTextStream* logStream, bool displayTree,
   bool runRegression, const QString& input)
{
#if QT_VERSION < 0x040000
    setIcon( QPixmap::fromMimeSource( "programq.png" ));
#endif
    parser = new AIMLParser(logStream);
    parser->loadVars("utils/vars.xml", false);
    parser->loadVars("utils/bot.xml", true);
    parser->loadSubstitutions("utils/substitutions.xml");
    
    if (runRegression)
    {
        parser->runRegression();
        return;
    }
        
    QString dirname = "aiml_set/" + aimlSet;
    QDir dir(dirname);
    QStringList files = dir.entryList("*.aiml");
    QPROGRESSDIALOG_CLASSNAME pd(this, 0, true);
    pd.setCaption(QString("\"%1\" AIML set").arg(aimlSet));
    pd.setCancelButton(NULL);
    pd.setTotalSteps(files.count());
    uint i = 0;
    for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it )
    {
        pd.setLabelText("parsing " + *it + "...");
        pd.setProgress( i );

        qApp->processEvents();
        if (pd.wasCanceled())
            break;
        parser->loadAiml(dirname + "/" + *it);
        i++;
    }
    if (displayTree)
        parser->displayTree();
    
    if (!input.isEmpty())
        printf("%s\n", parser->getResponse(input).ascii());
 
    comboBox->setFocus();
    connect(pushButton1, SIGNAL(clicked()), SLOT(validateInput()));
}

MyMainDialog::~MyMainDialog()
{
    parser->saveVars("utils/vars.xml");
}

void MyMainDialog::validateInput()
{
    QString input = comboBox->currentText();
    if (input.simplifyWhiteSpace().isEmpty())
       return;
    //Get input direction
    bool right2left = false;
    for (int i = 0; i < input.length(); i++)
    {
        if (input.at(i).direction() == QChar::DirAL)
        {
            right2left = true;
            break;
        }
    }

    QString output;
    textEdit->moveCursor(QTEXTEDIT_CLASSNAME::MoveEnd, false);
    if (right2left)
        textEdit->setAlignment(Qt::AlignRight);
    else
        textEdit->setAlignment(Qt::AlignLeft);
    textEdit->setColor(Qt::red);
    if (right2left)
    {
        ushort unicode[] = {1571, 1606, 1578};
        output.setUnicodeCodes(unicode, 3);
        output += "< ";
    }
    else
        output = "You> ";
    textEdit->insert(output);
    textEdit->setColor(Qt::black);
    textEdit->insert(input + "\n");
    if (!input.isEmpty())
    {
        QString result = parser->getResponse(input);
        textEdit->setColor(Qt::blue);
        if (right2left)
        {
            ushort unicode[] = {1607, 1608};
            output.setUnicodeCodes(unicode, 2);
            output += "< ";
        }
        else
            output = "Bot> ";
        textEdit->insert(output);
        textEdit->setColor(Qt::black);
        textEdit->insert(result + "\n\n");
    }
    comboBox->clearEdit();
}
