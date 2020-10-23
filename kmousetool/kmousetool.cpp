/***************************************************************************
                          kmousetool.cpp  -  description
                             -------------------
    begin                : Sun Jan 20 23:27:58 PST 2002
    copyright            : (C) 2002-2003 by Jeff Roush
    email                : jeff@mousetool.com
    copyright            : (C) 2003 by Olaf Schmidt
    email                : ojschmidt@kde.org
    copyright            : (C) 2003 by Gunnar Schmi Dt
    email                : gunnar@schmi-dt.de
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

// Needs to be be before X11/Intrinsic.h because of qtextstream.h
#include <phonon/MediaObject>

#include <X11/Xmd.h>
#include <X11/Intrinsic.h>     /* Intrinsics Definitions*/
#include <X11/StringDefs.h>    /* Standard Name-String definitions*/
#include <X11/extensions/xtestext1.h>    /* Standard Name-String definitions*/
#include <X11/extensions/XTest.h>    /* Standard Name-String definitions*/
#include <fixx11h.h>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QMenu>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>

#include <KConfig>
#include <KConfigGroup>
#include <KHelpMenu>
#include <KIconLoader>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <KStandardGuiItem>

int currentXPosition;
int currentYPosition;

#undef long

int leftButton;
int rightButton;

/**
* Gnarly X functions
*/
void queryPointer(Window *pidRoot, int *pnRx, int *pnRy, int *pnX, int *pnY, unsigned int *puMask);
int CursorHasMoved(int minMovement);
void getMouseButtons();
void LeftClick();
void RightClick();
void DoubleClick();
void LeftDn();
void LeftUp();

// Declarations
Display *display;


void KMouseTool::init_vars()
{
    continue_timer = 1;
    tick_count = 0;
    max_ticks = dwell_time+1;
    mouse_is_down = false;

    loadOptions();

    // If the ~/.mousetool directory doesn't exist, create it
    mSoundFileName = QStandardPaths::locate(QStandardPaths::DataLocation, QStringLiteral( "sounds/mousetool_tap.wav" ));
    mplayer = Phonon::createPlayer(Phonon::AccessibilityCategory);
    mplayer->setParent(this);

    // find application file
    appfilename = QStandardPaths::findExecutable(QStringLiteral( "kmousetool" ));

    // find the user's autostart directory
    autostartdirname = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/autostart-scripts/");

    QDesktopWidget *d = QApplication::desktop();
    int w = d->width();
    int h = d->height();
    MTStroke::setUpperLeft(0,0);
    MTStroke::setUpperRight(w-1,0);
    MTStroke::setLowerLeft(0,h-1);
    MTStroke::setLowerRight(w-1,h-1);

    buttonQuit = buttonBox->addButton(QString(), QDialogButtonBox::RejectRole);
}

void KMouseTool::resetSettings()
{
    cbDrag ->setChecked(smart_drag_on);
    cbStart->setChecked(isAutostart());
    cbClick->setChecked(playSound);
    cbStroke->setChecked(strokesEnabled);
    movementEdit->setValue(min_movement);
    dwellTimeEdit->setValue(dwell_time);
    dragTimeEdit->setValue(drag_time);
    settingsChanged();
}

void KMouseTool::setDefaultSettings()
{
    cbDrag ->setChecked(false);
    cbStart->setChecked(false);
    cbClick->setChecked(false);
    cbStroke->setChecked(false);
    movementEdit->setValue(5);
    dwellTimeEdit->setValue(5);
    dragTimeEdit->setValue(3);
    settingsChanged();
}


void KMouseTool::timerEvent( QTimerEvent * )
{
    if (!mousetool_is_running)
        return;

    if (!continue_timer) {
        QAbstractEventDispatcher::instance()->unregisterTimers(this);
        return;
    }

    max_ticks = dwell_time + drag_time;
    stroke.addPt(currentXPosition, currentYPosition);

    moving = moving?CursorHasMoved(1):CursorHasMoved(min_movement);
    if (moving) {
        if (mousetool_just_started) {
            mousetool_just_started = false;
            tick_count = max_ticks;
        } else {
            tick_count = 0;
        }
        return;
    }

    if (tick_count<max_ticks)
        tick_count++;

    // If the mouse has paused ...
    if (tick_count==dwell_time) {
        int strokeType = stroke.getStrokeType();
        getMouseButtons();

        // if we're dragging the mouse, ignore stroke type
        if (mouse_is_down) {
            normalClick();
            return;
        }

        if (!strokesEnabled) {
            normalClick();
            return;
        }
        if (strokeType == MTStroke::DontClick)
            return;
        if (strokeType==MTStroke::bumped_mouse)
            return;

        if (strokeType==MTStroke::RightClick || strokeType==MTStroke::UpperRightStroke)
            RightClick();
        else if (strokeType==MTStroke::DoubleClick || strokeType==MTStroke::LowerLeftStroke)
            DoubleClick();
        else
            normalClick();
    }
}

void KMouseTool::normalClick()
{
    if (smart_drag_on) {
        if (!mouse_is_down) {
            LeftDn();
            mouse_is_down = true;
            tick_count = 0;
            playTickSound();
        } else if (mouse_is_down) {
            LeftUp();
            mouse_is_down = false;
            tick_count = max_ticks;
        }
    } else {
        // not smart_drag_on
        LeftClick();
        playTickSound();
    }
}

// This function isn't happy yet.
void KMouseTool::playTickSound()
{
    if (!playSound)
        return;

    mplayer->setCurrentSource(QUrl::fromLocalFile(mSoundFileName));
    mplayer->play();
}

KMouseTool::KMouseTool(QWidget *parent, const char *name)
 : QWidget(parent), helpMenu(new KHelpMenu(nullptr))
{
    setupUi(this);
    setObjectName( QLatin1String( name ));
    init_vars();
    resetSettings();

    connect(movementEdit, SIGNAL(valueChanged(int)), this, SLOT(settingsChanged()));
    connect(dwellTimeEdit, SIGNAL(valueChanged(int)), this, SLOT(settingsChanged()));
    connect(dragTimeEdit, SIGNAL(valueChanged(int)), this, SLOT(settingsChanged()));
    connect(cbDrag, &QCheckBox::stateChanged, this, &KMouseTool::settingsChanged);
    connect(cbStroke, &QCheckBox::stateChanged, this, &KMouseTool::settingsChanged);
    connect(cbClick, &QCheckBox::stateChanged, this, &KMouseTool::settingsChanged);
    connect(cbStart, &QCheckBox::stateChanged, this, &KMouseTool::settingsChanged);

    connect(buttonStartStop, &QAbstractButton::clicked, this, &KMouseTool::startStopSelected);
    connect(buttonBoxSettings->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this, &KMouseTool::defaultSelected);
    connect(buttonBoxSettings->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &KMouseTool::resetSelected);
    connect(buttonBoxSettings->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &KMouseTool::applySelected);
    connect(buttonBox, &QDialogButtonBox::helpRequested, this, &KMouseTool::helpSelected);
    connect(buttonBox->button(QDialogButtonBox::Close), &QAbstractButton::clicked, this, &KMouseTool::closeSelected);
    KGuiItem::assign(buttonQuit, KStandardGuiItem::quit());
    connect(buttonQuit, &QAbstractButton::clicked, this, &KMouseTool::quitSelected);

    // When we first start, it's nice to not click immediately.
    // So, tell MT we're just starting
    mousetool_just_started = true;

    startTimer(100);
    trayIcon = new KMouseToolTray (this);
    updateStartStopText ();
    connect(trayIcon, &KMouseToolTray::startStopSelected, this, &KMouseTool::startStopSelected);
    connect(trayIcon, &KMouseToolTray::configureSelected, this, &KMouseTool::configureSelected);
    connect(trayIcon, &KMouseToolTray::aboutSelected, this, &KMouseTool::aboutSelected);
    connect(trayIcon, &KMouseToolTray::helpSelected, this, &KMouseTool::helpSelected);
    connect(trayIcon, SIGNAL(quitSelected()), this, SLOT(quitSelected()));
}

KMouseTool::~KMouseTool()
{
}

// *********************************************************
// Raw X functions
// Begin mouse monitoring and event generation code.
// these should be moved to a separate file.
void getMouseButtons()
{
    unsigned char buttonMap[3];
    const int buttonCount = XGetPointerMapping(display, buttonMap, 3);
    switch (buttonCount)
    {
    case 0:
    case 1:
        // ### should not happen
        leftButton = 1;
        rightButton = 1;
        break;
    case 2:
        leftButton = buttonMap[0];
        rightButton = buttonMap[1];
        break;
    default:
        leftButton = buttonMap[0];
        rightButton = buttonMap[2];
        break;
    }
}

void LeftClick()
{
    XTestFakeButtonEvent(display, leftButton, true, 0);
    XTestFakeButtonEvent(display, leftButton, false, 0);
}

void DoubleClick()
{
    XTestFakeButtonEvent(display, leftButton, true, 0);
    XTestFakeButtonEvent(display, leftButton, false, 100);
    XTestFakeButtonEvent(display, leftButton, true, 200);
    XTestFakeButtonEvent(display, leftButton, false, 300);
}

void RightClick()
{
    XTestFakeButtonEvent(display, rightButton, true, 0);
    XTestFakeButtonEvent(display, rightButton, false, 0);
}

void LeftDn()
{
    XTestFakeButtonEvent(display, leftButton, true, 0);
}

void LeftUp()
{
    XTestFakeButtonEvent(display, leftButton, false, 0);
}


void queryPointer(Window *pidRoot, int *pnRx, int *pnRy, int *pnX, int *pnY, unsigned int *puMask)
{
    Window id2, idRoot;
    int screen_num;

    screen_num = DefaultScreen(display);
    idRoot = RootWindow(display,screen_num);
    XQueryPointer(display, idRoot, &idRoot, &id2, pnRx, pnRy, pnX, pnY, puMask);

    *pidRoot = idRoot;
}

// return 1 if cursor has moved, 0 otherwise
int CursorHasMoved (int minMovement)
{
    static int nOldRootX = -1;
    static int nOldRootY = -1;

    // XEvent evButtonEvent;
    Window idRootWin;
    int nRootX, nRootY, nWinX, nWinY;
    unsigned int uintMask;

    queryPointer(&idRootWin, &nRootX, &nRootY, &nWinX, &nWinY, &uintMask);

    int nRes = 0;
    if ((nRootX>nOldRootX?nRootX-nOldRootX:nOldRootX-nRootX) >= minMovement)
        nRes = 1;
    if ((nRootY>nOldRootY?nRootY-nOldRootY:nOldRootY-nRootY) >= minMovement)
        nRes = 1;

    currentXPosition = nRootX;
    currentYPosition = nRootY;

    if (nRes) {
        nOldRootX = nRootX;
        nOldRootY = nRootY;
    }

    return nRes;
}
// End mouse monitoring and event creation code


bool KMouseTool::isAutostart()
{
    QString sym = autostartdirname;
    sym += QLatin1String( "kmousetool" ); // sym is now full path to symlink
    QFileInfo fi(sym);

    return fi.exists();
}

void KMouseTool::setAutostart (bool start)
{
    QString sym = autostartdirname;
    sym += QLatin1String( "kmousetool" ); // sym is now full path to symlink
    QFileInfo fi(sym);

    if (start) {
        if (!fi.exists()) // if it doesn't exist, make it
            QFile(appfilename).link(sym);
    } else {
        if (fi.exists()) // if it exists, delete it
            QFile(sym).remove();
    }
}

bool KMouseTool::applySettings()
{
    int drag, dwell;

    dwell = dwellTimeEdit->value();
    drag = dragTimeEdit->value() ;

    // The drag time must be less than the dwell time
    if ( dwell < drag) {
        KMessageBox::sorry(this, i18n("The drag time must be less than or equal to the dwell time."), i18n("Invalid Value"));
        return false;
    }

    // if we got here, we must be okay.
    min_movement   = movementEdit->value();
    smart_drag_on  = cbDrag->isChecked();
    playSound      = cbClick->isChecked();
    strokesEnabled = cbStroke->isChecked();
    setAutostart (cbStart->isChecked());

    dwell_time = dwell;
    drag_time  = drag;
    tick_count = max_ticks;

    saveOptions();
    settingsChanged();
    return true;
}

// Save options to kmousetoolrc file
void KMouseTool::loadOptions()
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("UserOptions");

    playSound = cfg.readEntry("AudibleClick", false);
    smart_drag_on = cfg.readEntry("SmartDrag", false);

    dwell_time = cfg.readEntry("DwellTime",5);
    drag_time = cfg.readEntry("DragTime",3);
    min_movement = cfg.readEntry("Movement",5);
    strokesEnabled = cfg.readEntry("strokesEnabled", false);

    QPoint p;
    int x = cfg.readEntry("x",0);
    int y = cfg.readEntry("y",0);
    p.setX(x);
    p.setY(y);
    move(p);

    mousetool_is_running = cfg.readEntry("MouseToolIsRunning", false);
    display = XOpenDisplay(nullptr);
}

// Save options to kmousetoolrc file
void KMouseTool::saveOptions()
{
    QPoint p = pos();
    int x = p.x();
    int y = p.y();

    KConfigGroup cfg = KSharedConfig::openConfig()->group("ProgramOptions");
    cfg.writeEntry("Version", KMOUSETOOL_VERSION);
    cfg = KSharedConfig::openConfig()->group("UserOptions");
    cfg.writeEntry("x", x);
    cfg.writeEntry("y", y);
    cfg.writeEntry("strokesEnabled", strokesEnabled);
    cfg.writeEntry("IsMinimized", isHidden());
    cfg.writeEntry("DwellTime", dwell_time);
    cfg.writeEntry("DragTime", drag_time);
    cfg.writeEntry("Movement", min_movement);
    cfg.writeEntry("SmartDrag", smart_drag_on);
    cfg.writeEntry("AudibleClick", playSound);
    cfg.writeEntry("MouseToolIsRunning", mousetool_is_running);
    cfg.sync();
}

void KMouseTool::updateStartStopText()
{
    if (mousetool_is_running) {
        buttonStartStop->setText(i18n("&Stop"));
    } else {
        buttonStartStop->setText(i18nc("Start tracking the mouse", "&Start"));
    }
    trayIcon->updateStartStopText(mousetool_is_running);
}

bool KMouseTool::newSettings()
{
    return ((dwell_time != dwellTimeEdit->value()) ||
            (drag_time != dragTimeEdit->value()) ||
            (min_movement != movementEdit->value()) ||
            (smart_drag_on != cbDrag->isChecked()) ||
            (playSound != cbClick->isChecked()) ||
            (strokesEnabled != cbStroke->isChecked()) ||
            (isAutostart() != cbStart->isChecked()));
}

bool KMouseTool::defaultSettings()
{
    return ((5 == dwellTimeEdit->value()) &&
            (3 == dragTimeEdit->value()) &&
            (5 == movementEdit->value()) &&
            !cbDrag->isChecked() &&
            !cbClick->isChecked() &&
            !cbStroke->isChecked() &&
            !cbStart->isChecked());
}

/******** SLOTS **********/

// Value or state changed
void KMouseTool::settingsChanged ()
{
    buttonBoxSettings->button(QDialogButtonBox::Reset)->setEnabled (newSettings());
    buttonBoxSettings->button(QDialogButtonBox::Apply)->setEnabled (newSettings());
    buttonBoxSettings->button(QDialogButtonBox::RestoreDefaults)->setDisabled (defaultSettings());
}

// Buttons within the dialog
void KMouseTool::startStopSelected()
{
    mousetool_is_running = !mousetool_is_running;
    updateStartStopText();
}

void KMouseTool::defaultSelected()
{
    setDefaultSettings();
}

void KMouseTool::resetSelected()
{
    resetSettings();
}

void KMouseTool::applySelected()
{
    applySettings();
}

// Buttons at the bottom of the dialog
void KMouseTool::helpSelected()
{
    helpMenu->appHelpActivated();
}

void KMouseTool::closeSelected()
{
    if (newSettings())
    {
        int answer = KMessageBox::questionYesNoCancel (this,
                                                       i18n("There are unsaved changes in the active module.\nDo you want to apply the changes before closing the configuration window or discard the changes?"),
                                                       i18n("Closing Configuration Window"),
                                                       KStandardGuiItem::apply(), KStandardGuiItem::discard(),
                                                       KStandardGuiItem::cancel(), QStringLiteral( "AutomaticSave" ));
        if (answer == KMessageBox::Yes)
            applySettings();
        else if (answer == KMessageBox::No)
            resetSettings();
        if (answer != KMessageBox::Cancel)
            hide();
    } else {
        hide();
    }
}

void KMouseTool::quitSelected()
{
    if (newSettings())
    {
        int answer = KMessageBox::questionYesNoCancel (this,
                                                       i18n("There are unsaved changes in the active module.\nDo you want to apply the changes before quitting KMousetool or discard the changes?"),
                                                       i18n("Quitting KMousetool"),
                                                       KStandardGuiItem::apply(), KStandardGuiItem::discard(),
                                                       KStandardGuiItem::cancel(), QStringLiteral( "AutomaticSave" ));
        if (answer == KMessageBox::Yes)
            applySettings();
        if (answer != KMessageBox::Cancel)
        {
            saveOptions();
            qApp->quit();
        }
    } else {
        saveOptions();
        qApp->quit();
    }
}

// Menu functions
void KMouseTool::configureSelected()
{
    show();
    raise();
    activateWindow();
}

void KMouseTool::aboutSelected()
{
    helpMenu->aboutApplication();
}



KMouseToolTray::KMouseToolTray (QWidget *parent) : KStatusNotifierItem(parent)
{
    setStatus(KStatusNotifierItem::Active);
    startStopAct = contextMenu()->addAction (i18nc("Start tracking the mouse", "&Start"), this, &KMouseToolTray::startStopSelected);
    contextMenu()->addSeparator();
    QAction* act;
    act = contextMenu()->addAction (i18n("&Configure KMouseTool..."), this, &KMouseToolTray::configureSelected);
    act->setIcon(QIcon::fromTheme(QStringLiteral( "configure" )));
    contextMenu()->addSeparator();
    act = contextMenu()->addAction (i18n("KMousetool &Handbook"), this, &KMouseToolTray::helpSelected);
    act->setIcon(QIcon::fromTheme(QStringLiteral( "help-contents" )));
    act = contextMenu()->addAction (i18n("&About KMouseTool"), this, &KMouseToolTray::aboutSelected);
    act->setIcon(QIcon::fromTheme(QStringLiteral( "kmousetool" )));
}

KMouseToolTray::~KMouseToolTray() {
}

void KMouseToolTray::updateStartStopText(bool mousetool_is_running)
{
    QIcon icon;

    if (mousetool_is_running) {
        startStopAct->setText(i18n("&Stop"));
        icon = KIconLoader::global()->loadIcon(QStringLiteral( "kmousetool_on" ), KIconLoader::Small);
    } else {
        startStopAct->setText(i18nc("Start tracking the mouse", "&Start"));
        icon = KIconLoader::global()->loadIcon(QStringLiteral( "kmousetool_off" ), KIconLoader::Small);
    }
    setIconByPixmap (icon);
}
