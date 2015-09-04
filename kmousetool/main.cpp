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


#include "kmousetool.h"

#include <KAboutData>
#include <KConfigGroup>
#include <KDBusService>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QCommandLineParser>

static const char description[] = I18N_NOOP("KMouseTool");

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(QLatin1String("kmousetool"));
    QApplication::setApplicationVersion(QLatin1String(KMOUSETOOL_VERSION));
    QApplication::setOrganizationDomain(QLatin1String("kde.org"));
    KLocalizedString::setApplicationDomain("kmousetool");
    QApplication::setApplicationDisplayName(i18n("kmousetool"));
    QApplication app(argc, argv);

    KAboutData aboutData(QLatin1Literal("kmousetool"),
                         i18n("KMouseTool"),
                         QLatin1Literal(KMOUSETOOL_VERSION),
                         i18n(description),
                         KAboutLicense::GPL,
                         i18n("(c) 2002-2003, Jeff Roush\n(c) 2003, Gunnar Schmidt"),
                         QString(),
                         QLatin1Literal("http://www.mousetool.com"),
                         QLatin1Literal("gunnar@schmi-dt.de"));

    aboutData.addAuthor(i18n("Gunnar Schmidt"), i18n("Current maintainer"), QLatin1String("gunnar@schmi-dt.de"), QLatin1String("http://www.schmi-dt.de"));
    aboutData.addAuthor(i18n("Olaf Schmidt"), i18n("Usability improvements"), QLatin1String("ojschmidt@kde.org"));
    aboutData.addAuthor(i18n("Jeff Roush"), i18n("Original author"), QLatin1String("jeff@mousetool.com"), QLatin1String("http://www.mousetool.com"));

    aboutData.addCredit(i18n("Joe Betts"));
    aboutData.setOrganizationDomain("kde.org");
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    //PORTING SCRIPT: adapt aboutdata variable if necessary
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique, &app);

    KMouseTool *kmousetool = new KMouseTool();

    if (!KSharedConfig::openConfig()->group("UserOptions").readEntry("IsMinimized", false))
        kmousetool->show();

    return app.exec();
}
