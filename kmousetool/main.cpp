/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jan 20 23:27:58 PST 2002
    copyright            : (C) 2002 by Jeff Roush
    email                : jeff@mousetool.com
    copyright            : (C) 2003 by Olaf Schmidt
    email                : ojschmidt@kde.org
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
#include <kstandarddirs.h>
#include <kuniqueapplication.h>
#include <dcopclient.h>
#include <qmessagebox.h>
#include <kconfig.h>

#include "kmousetool.h"

static const char description[] =
	I18N_NOOP("KMouseTool");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


static KCmdLineOptions options[] =
{
	KCmdLineLastOption
	// INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
	KAboutData aboutData( "kmousetool", I18N_NOOP("KMouseTool"),
	KMOUSETOOL_VERSION, description, KAboutData::License_GPL,
	"(c) 2002-2003, Jeff Roush\n(c) 2003, Gunnar Schmi Dt", 0, "http://www.mousetool.com", "gunnar@schmi-dt.de");

	aboutData.addAuthor("Gunnar Schmi Dt", I18N_NOOP("Current maintainer"), "gunnar@schmi-dt.de", "http://www.schmi-dt.de");
	aboutData.addAuthor("Olaf Schmidt", I18N_NOOP("Usability improvements"), "ojschmidt@kde.org");
	aboutData.addAuthor("Jeff Roush", I18N_NOOP("Original author"), "jeff@mousetool.com", "http://www.mousetool.com");

	aboutData.addCredit("Joe Betts");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
	KUniqueApplication::addCmdLineOptions();

	if (!KUniqueApplication::start()) {
		DCOPClient *client = new DCOPClient();
		client->attach();
		QByteArray  data;
		QCString    replyType;
		QByteArray  replyData;
		if ( !client->call("kmousetool", "qt/KMouseToolUI", "show()",
					data, replyType, replyData, true) ||
			!client->call("kmousetool", "qt/KMouseToolUI", "raise()",
					data, replyType, replyData, true) )
		fprintf(stderr, "The DCOP calls failed\n");
		delete client;
		exit(0);
	}
	KUniqueApplication a;

	KMouseTool *kmousetool = new KMouseTool();

	if (!kapp->config()->readBoolEntry("IsMinimized", false))
		kmousetool->show();

	return a.exec();
}
