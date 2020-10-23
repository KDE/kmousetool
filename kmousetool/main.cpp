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
    // enable high dpi support
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("kmousetool");

    KAboutData aboutData(QStringLiteral("kmousetool"),
                         i18n("KMouseTool"),
                         QStringLiteral(KMOUSETOOL_VERSION),
                         i18n(description),
                         KAboutLicense::GPL,
                         i18n("(c) 2002-2003, Jeff Roush\n(c) 2003, Gunnar Schmidt"),
                         QString(),
                         QStringLiteral("https://www.kde.org/applications/utilities/kmousetool/"),
                         QStringLiteral("gunnar@schmi-dt.de"));

    aboutData.addAuthor(i18n("Gunnar Schmidt"), i18n("Current maintainer"), QStringLiteral("gunnar@schmi-dt.de"), QStringLiteral("http://www.schmi-dt.de"));
    aboutData.addAuthor(i18n("Olaf Schmidt"), i18n("Usability improvements"), QStringLiteral("ojschmidt@kde.org"));
    aboutData.addAuthor(i18n("Jeff Roush"), i18n("Original author"), QStringLiteral("jeff@mousetool.com"));

    aboutData.addCredit(i18n("Joe Betts"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.kmousetool"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique, &app);

    KMouseTool *kmousetool = new KMouseTool();

    if (!KSharedConfig::openConfig()->group("UserOptions").readEntry("IsMinimized", false))
        kmousetool->show();

    return app.exec();
}
