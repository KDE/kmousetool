/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jan 20 23:27:58 PST 2002
    copyright            : (C) 2002 by Jeff Roush
    email                : jeff@mousetool.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kuniqueapp.h>

#include "kmousetool.h"

static const char *description =
	I18N_NOOP("KMouseTool");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kmousetool", I18N_NOOP("KMouseTool"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2002, Jeff Roush", 0, "http://www.mousetool.com", "jeff@mousetool.com");
  aboutData.addAuthor("Jeff Roush", "jeff@mousetool.com", "http://www.mousetool.com");
  aboutData.addCredit("Joe Betts");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KUniqueApplication::addCmdLineOptions();

#ifdef DEBUG_MOUSETOOL
  fprintf(stderr, "Compiled with Debugging enabled\n");
  KApplication a;
#else
  if (!KUniqueApplication::start()) {
     fprintf(stderr, "KMouseTool is already running!\n");
     exit(0);
  }
  KUniqueApplication a;
#endif

  KMouseTool *kmousetool = new KMouseTool();
  a.setMainWidget(kmousetool);
//	QString str = locate("appdata", "mousetool_tap.wav");
  kmousetool->show();  

  return a.exec();
}
