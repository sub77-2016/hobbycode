/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : mar mar 8 2005
    copyright            : (C) 2005 by houssem bdioui
    email                : houssem.bdioui@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qapplication.h>
#include <qfile.h>
#include <qdir.h>
#include <qcombobox.h>
#include <qtextstream.h>
#include "mymaindialog.h"
#include "aimlsetdialog.h"
#if QT_VERSION >= 0x040000
#include <QPlastiqueStyle>
#endif

char helpMessage[] = "Usage: %s [options]\nWhere options can be:\n"
          "  -h, --help                  display this help message\n"
          "  -d, --display-tree          display the nodes tree in debug.log\n"
          "  -r, --run-regression        run a test suite regression & exit (see debug.log)\n"
          "  -s, --subset \"subset name\"  use the specified AIML subset\n"
          "  -i, --input \"input string\"  replies to input & exit (subset must be set)\n";

//The main entry
int main( int argc, char ** argv )
{
    QApplication a( argc, argv);
#if QT_VERSION >= 0x040000
    a.setWindowIcon( qPixmapFromMimeSource( "programq.png" ));
    QPlastiqueStyle *plastique = new QPlastiqueStyle;
    a.setStyle(plastique);
#endif
    bool displayTree = false, runRegression = false;
    QString input(""), subset("");
    for (uint i = 1; i < argc; i++)
    {
       if ((QString(argv[i]) == "-h") || (QString(argv[i]) == "--help"))
       {
          qWarning(helpMessage, argv[0]);
          return 0;
       }
       else if ((QString(argv[i]) == "-d") || (QString(argv[i]) == "--display-tree"))
          displayTree = true;
       else if ((QString(argv[i]) == "-r") || (QString(argv[i]) == "--run-regression"))
          runRegression = true;
       else if ((QString(argv[i]) == "-i") || (QString(argv[i]) == "--input"))
       {
          if (argc > i+1)
          {
             input = argv[i+1];
             i++;
          }
	      else
	      {
	         qWarning(helpMessage, argv[0]);
	         return -1;
	      }
       }
       else if ((QString(argv[i]) == "-s") || (QString(argv[i]) == "--subset"))
       {
          if (argc > i+1)
          {
             subset = argv[i+1];
             i++;
          }
	      else
	      {
	         qWarning(helpMessage, argv[0]);
	         return -1;
	      }
       }
       else
       {
          qWarning(helpMessage, argv[0]);
          return -1;
       }
    }
    if (!input.isEmpty() && subset.isEmpty())
    {
       qWarning(helpMessage, argv[0]);
       return -1;
    }
    //Create the log file
    QFile file( "debug.log" );
    if ( !file.open( IO_WriteOnly ) )
        return false;
    QTextStream logStream;
    logStream.setDevice(&file);
    logStream.setEncoding(QTextStream::Unicode);
    
    if(!runRegression && subset.isEmpty())
    {
	    //choose which aiml set to use!
	    QDir dir("aiml_set");
	    QStringList subdirs = dir.entryList("*", QDir::Dirs);
	    subdirs.remove(".");
	    subdirs.remove("..");
	    AIMLSetDialog aimlDialog;
	    aimlDialog.aiml_set->insertStringList(subdirs);
	    aimlDialog.exec();
	    subset = aimlDialog.aiml_set->currentText();
    }
    //Create and show the main window
    int val = 0;
    MyMainDialog md(subset, &logStream,
       displayTree, runRegression, input);
    if (!runRegression && input.isEmpty())
    {
	    a.setMainWidget(&md);
	    md.show();
        val = a.exec();
    }
    file.close();
    return val;
}
