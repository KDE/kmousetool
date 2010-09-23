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
#include <QtDBus/QtDBus>
#include <QtGui/QMessageBox>
#include <kconfig.h>
#include <kglobal.h>

#include "kmousetool.h"

static const char description[] =
    I18N_NOOP("KMouseTool");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


int main(int argc, char *argv[])
{
    KAboutData aboutData( "kmousetool", 0, ki18n("KMouseTool"),
    KMOUSETOOL_VERSION, ki18n(description), KAboutData::License_GPL,
    ki18n("(c) 2002-2003, Jeff Roush\n(c) 2003, Gunnar Schmi Dt"), KLocalizedString(), "http://www.mousetool.com", "gunnar@schmi-dt.de");

    aboutData.addAuthor(ki18n("Gunnar Schmi Dt"), ki18n("Current maintainer"), "gunnar@schmi-dt.de", "http://www.schmi-dt.de");
    aboutData.addAuthor(ki18n("Olaf Schmidt"), ki18n("Usability improvements"), "ojschmidt@kde.org");
    aboutData.addAuthor(ki18n("Jeff Roush"), ki18n("Original author"), "jeff@mousetool.com", "http://www.mousetool.com");

    aboutData.addCredit(ki18n("Joe Betts"));
    aboutData.setOrganizationDomain("kde.org");
    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
    KUniqueApplication::addCmdLineOptions();

    KUniqueApplication::setApplicationName(QLatin1String( "kmousetool" ));

    if (!KUniqueApplication::start()) {
            fprintf(stderr, "KMouseTool is already running !\n");
            exit(0);
    }
    KUniqueApplication a;

    KMouseTool *kmousetool = new KMouseTool();

    if (!KGlobal::config()->group("UserOptions").readEntry("IsMinimized", false))
        kmousetool->show();

    return a.exec();
}
