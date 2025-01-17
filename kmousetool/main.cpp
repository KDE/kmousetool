/*
    SPDX-FileCopyrightText: 2002 Jeff Roush <jeff@mousetool.com>
    SPDX-FileCopyrightText: 2003 Olaf Schmidt <ojschmidt@kde.org>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kmousetool.h"

#include <KAboutData>
#include <KConfigGroup>
#include <KDBusService>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("kmousetool"));

    KAboutData aboutData(QStringLiteral("kmousetool"),
                         i18n("KMouseTool"),
                         QStringLiteral(KMOUSETOOL_VERSION),
                         i18n("KMouseTool"),
                         KAboutLicense::GPL,
                         i18n("(c) 2002-2003, Jeff Roush\n(c) 2003 Gunnar Schmidt "),
                         QString(),
                         QStringLiteral("https://www.kde.org/applications/utilities/kmousetool/"),
                         QStringLiteral("gunnar@schmi-dt.de"));

    aboutData.addAuthor(i18n("Gunnar Schmidt"), i18n("Current maintainer"), QStringLiteral("gunnar@schmi-dt.de"), QStringLiteral("http://www.schmi-dt.de"));
    aboutData.addAuthor(i18n("Olaf Schmidt"), i18n("Usability improvements"), QStringLiteral("ojschmidt@kde.org"));
    aboutData.addAuthor(i18n("Jeff Roush"), i18n("Original author"), QStringLiteral("jeff@mousetool.com"));

    aboutData.addCredit(i18n("Joe Betts"));
    aboutData.setDesktopFileName(QStringLiteral("org.kde.kmousetool"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kmousetool")));


    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique, &app);

    KMouseTool *kmousetool = new KMouseTool();

    if (!KSharedConfig::openConfig()->group(QStringLiteral("UserOptions")).readEntry("IsMinimized", false))
        kmousetool->show();

    return app.exec();
}
