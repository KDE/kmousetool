/*
    SPDX-FileCopyrightText: 2002-2003 Jeff Roush <jeff@mousetool.com>
    SPDX-FileCopyrightText: 2003 Olaf Schmidt <ojschmidt@kde.org>
    SPDX-FileCopyrightText: 2003 Gunnar Schmi Dt <gunnar@schmi-dt.de>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kmousetool.h"

#include <X11/Intrinsic.h> /* Intrinsics Definitions*/
#include <X11/StringDefs.h> /* Standard Name-String definitions*/
#include <X11/Xmd.h>
#include <X11/extensions/XTest.h> /* Standard Name-String definitions*/
#include <X11/extensions/xtestext1.h> /* Standard Name-String definitions*/
#include <fixx11h.h>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QAudioOutput>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QMediaPlayer>
#include <QMenu>
#include <QScreen>
#include <QStandardPaths>

#include <KConfig>
#include <KConfigGroup>
#include <KHelpMenu>
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
    mContinue_timer = 1;
    mTick_count = 0;
    mMax_ticks = mDwell_time + 1;
    mMouse_is_down = false;

    loadOptions();

    // If the ~/.mousetool directory doesn't exist, create it
    mSoundFileName = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("sounds/mousetool_tap.wav"));
    mPlayer = new QMediaPlayer();
    QAudioOutput *output = new QAudioOutput();
    mPlayer->setAudioOutput(output);
    mPlayer->setParent(this);

    // find application file
    mAppfilename = QStandardPaths::findExecutable(QStringLiteral("kmousetool"));

    // find the user's autostart directory
    mAutostartdirname = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/autostart-scripts/");

    auto screenSize = screen()->availableGeometry();
    int w = screenSize.width();
    int h = screenSize.height();
    MTStroke::setUpperLeft(0, 0);
    MTStroke::setUpperRight(w - 1, 0);
    MTStroke::setLowerLeft(0, h - 1);
    MTStroke::setLowerRight(w - 1, h - 1);

    mButtonQuit = buttonBox->addButton(QString(), QDialogButtonBox::RejectRole);
}

void KMouseTool::resetSettings()
{
    cbDrag->setChecked(mSmart_drag_on);
    cbStart->setChecked(isAutostart());
    cbClick->setChecked(mPlaySound);
    cbStroke->setChecked(mStrokesEnabled);
    movementEdit->setValue(mMin_movement);
    dwellTimeEdit->setValue(mDwell_time);
    dragTimeEdit->setValue(mDrag_time);
    settingsChanged();
}

void KMouseTool::setDefaultSettings()
{
    cbDrag->setChecked(false);
    cbStart->setChecked(false);
    cbClick->setChecked(false);
    cbStroke->setChecked(false);
    movementEdit->setValue(5);
    dwellTimeEdit->setValue(5);
    dragTimeEdit->setValue(3);
    settingsChanged();
}

void KMouseTool::timerEvent(QTimerEvent *)
{
    if (!mMousetool_is_running)
        return;

    if (!mContinue_timer) {
        QAbstractEventDispatcher::instance()->unregisterTimers(this);
        return;
    }

    mMax_ticks = mDwell_time + mDrag_time;
    mStroke.addPt(currentXPosition, currentYPosition);

    mMoving = mMoving ? CursorHasMoved(1) : CursorHasMoved(mMin_movement);
    if (mMoving) {
        if (mMousetool_just_started) {
            mMousetool_just_started = false;
            mTick_count = mMax_ticks;
        } else {
            mTick_count = 0;
        }
        return;
    }

    if (mTick_count < mMax_ticks)
        mTick_count++;

    // If the mouse has paused ...
    if (mTick_count == mDwell_time) {
        int strokeType = mStroke.getStrokeType();
        getMouseButtons();

        // if we're dragging the mouse, ignore stroke type
        if (mMouse_is_down) {
            normalClick();
            return;
        }

        if (!mStrokesEnabled) {
            normalClick();
            return;
        }
        if (strokeType == MTStroke::DontClick)
            return;
        if (strokeType == MTStroke::bumped_mouse)
            return;

        if (strokeType == MTStroke::RightClick || strokeType == MTStroke::UpperRightStroke)
            RightClick();
        else if (strokeType == MTStroke::DoubleClick || strokeType == MTStroke::LowerLeftStroke)
            DoubleClick();
        else
            normalClick();
    }
}

void KMouseTool::normalClick()
{
    if (mSmart_drag_on) {
        if (!mMouse_is_down) {
            LeftDn();
            mMouse_is_down = true;
            mTick_count = 0;
            playTickSound();
        } else if (mMouse_is_down) {
            LeftUp();
            mMouse_is_down = false;
            mTick_count = mMax_ticks;
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
    if (!mPlaySound)
        return;

    mPlayer->setSource(QUrl::fromLocalFile(mSoundFileName));
    mPlayer->play();
}

KMouseTool::KMouseTool(QWidget *parent, const char *name)
    : QWidget(parent)
    , mHelpMenu(new KHelpMenu(nullptr))
{
    setupUi(this);
    setObjectName(QLatin1String(name));
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
    KGuiItem::assign(mButtonQuit, KStandardGuiItem::quit());
    connect(mButtonQuit, &QAbstractButton::clicked, this, &KMouseTool::quitSelected);

    // When we first start, it's nice to not click immediately.
    // So, tell MT we're just starting
    mMousetool_just_started = true;

    startTimer(100);
    mTrayIcon = new KMouseToolTray(this);
    updateStartStopText();
    connect(mTrayIcon, &KMouseToolTray::startStopSelected, this, &KMouseTool::startStopSelected);
    connect(mTrayIcon, &KMouseToolTray::configureSelected, this, &KMouseTool::configureSelected);
    connect(mTrayIcon, &KMouseToolTray::aboutSelected, this, &KMouseTool::aboutSelected);
    connect(mTrayIcon, &KMouseToolTray::helpSelected, this, &KMouseTool::helpSelected);
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
    switch (buttonCount) {
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
    idRoot = RootWindow(display, screen_num);
    XQueryPointer(display, idRoot, &idRoot, &id2, pnRx, pnRy, pnX, pnY, puMask);

    *pidRoot = idRoot;
}

// return 1 if cursor has moved, 0 otherwise
int CursorHasMoved(int minMovement)
{
    static int nOldRootX = -1;
    static int nOldRootY = -1;

    // XEvent evButtonEvent;
    Window idRootWin;
    int nRootX, nRootY, nWinX, nWinY;
    unsigned int uintMask;

    queryPointer(&idRootWin, &nRootX, &nRootY, &nWinX, &nWinY, &uintMask);

    int nRes = 0;
    if ((nRootX > nOldRootX ? nRootX - nOldRootX : nOldRootX - nRootX) >= minMovement)
        nRes = 1;
    if ((nRootY > nOldRootY ? nRootY - nOldRootY : nOldRootY - nRootY) >= minMovement)
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
    QString sym = mAutostartdirname;
    sym += QLatin1String("kmousetool"); // sym is now full path to symlink
    QFileInfo fi(sym);

    return fi.exists();
}

void KMouseTool::setAutostart(bool start)
{
    QString sym = mAutostartdirname;
    sym += QLatin1String("kmousetool"); // sym is now full path to symlink
    QFileInfo fi(sym);

    if (start) {
        if (!fi.exists()) // if it doesn't exist, make it
            QFile(mAppfilename).link(sym);
    } else {
        if (fi.exists()) // if it exists, delete it
            QFile(sym).remove();
    }
}

bool KMouseTool::applySettings()
{
    int drag, dwell;

    dwell = dwellTimeEdit->value();
    drag = dragTimeEdit->value();

    // The drag time must be less than the dwell time
    if (dwell < drag) {
        KMessageBox::error(this, i18n("The drag time must be less than or equal to the dwell time."), i18n("Invalid Value"));
        return false;
    }

    // if we got here, we must be okay.
    mMin_movement = movementEdit->value();
    mSmart_drag_on = cbDrag->isChecked();
    mPlaySound = cbClick->isChecked();
    mStrokesEnabled = cbStroke->isChecked();
    setAutostart(cbStart->isChecked());

    mDwell_time = dwell;
    mDrag_time = drag;
    mTick_count = mMax_ticks;

    saveOptions();
    settingsChanged();
    return true;
}

// Save options to kmousetoolrc file
void KMouseTool::loadOptions()
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group(QStringLiteral("UserOptions"));

    mPlaySound = cfg.readEntry("AudibleClick", false);
    mSmart_drag_on = cfg.readEntry("SmartDrag", false);

    mDwell_time = cfg.readEntry("DwellTime", 5);
    mDrag_time = cfg.readEntry("DragTime", 3);
    mMin_movement = cfg.readEntry("Movement", 5);
    mStrokesEnabled = cfg.readEntry("strokesEnabled", false);

    QPoint p;
    int x = cfg.readEntry("x", 0);
    int y = cfg.readEntry("y", 0);
    p.setX(x);
    p.setY(y);
    move(p);

    mMousetool_is_running = cfg.readEntry("MouseToolIsRunning", false);
    display = XOpenDisplay(nullptr);
}

// Save options to kmousetoolrc file
void KMouseTool::saveOptions()
{
    QPoint p = pos();
    int x = p.x();
    int y = p.y();

    KConfigGroup cfg = KSharedConfig::openConfig()->group(QStringLiteral("ProgramOptions"));
    cfg = KSharedConfig::openConfig()->group(QStringLiteral("UserOptions"));
    cfg.writeEntry("x", x);
    cfg.writeEntry("y", y);
    cfg.writeEntry("strokesEnabled", mStrokesEnabled);
    cfg.writeEntry("IsMinimized", isHidden());
    cfg.writeEntry("DwellTime", mDwell_time);
    cfg.writeEntry("DragTime", mDrag_time);
    cfg.writeEntry("Movement", mMin_movement);
    cfg.writeEntry("SmartDrag", mSmart_drag_on);
    cfg.writeEntry("AudibleClick", mPlaySound);
    cfg.writeEntry("MouseToolIsRunning", mMousetool_is_running);
    cfg.sync();
}

void KMouseTool::updateStartStopText()
{
    if (mMousetool_is_running) {
        buttonStartStop->setText(i18n("&Stop"));
    } else {
        buttonStartStop->setText(i18nc("Start tracking the mouse", "&Start"));
    }
    mTrayIcon->updateStartStopText(mMousetool_is_running);
}

bool KMouseTool::newSettings()
{
    return ((mDwell_time != dwellTimeEdit->value()) || (mDrag_time != dragTimeEdit->value()) || (mMin_movement != movementEdit->value())
            || (mSmart_drag_on != cbDrag->isChecked()) || (mPlaySound != cbClick->isChecked()) || (mStrokesEnabled != cbStroke->isChecked())
            || (isAutostart() != cbStart->isChecked()));
}

bool KMouseTool::defaultSettings()
{
    return ((5 == dwellTimeEdit->value()) && (3 == dragTimeEdit->value()) && (5 == movementEdit->value()) && !cbDrag->isChecked() && !cbClick->isChecked()
            && !cbStroke->isChecked() && !cbStart->isChecked());
}

/******** SLOTS **********/

// Value or state changed
void KMouseTool::settingsChanged()
{
    buttonBoxSettings->button(QDialogButtonBox::Reset)->setEnabled(newSettings());
    buttonBoxSettings->button(QDialogButtonBox::Apply)->setEnabled(newSettings());
    buttonBoxSettings->button(QDialogButtonBox::RestoreDefaults)->setDisabled(defaultSettings());
}

// Buttons within the dialog
void KMouseTool::startStopSelected()
{
    mMousetool_is_running = !mMousetool_is_running;
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
    mHelpMenu->appHelpActivated();
}

void KMouseTool::closeSelected()
{
    if (newSettings()) {
        int answer =
            KMessageBox::questionTwoActionsCancel(this,
                                                  i18n("There are unsaved changes in the active module.\nDo you want to apply the changes before closing "
                                                       "the configuration window or discard the changes?"),
                                                  i18n("Closing Configuration Window"),
                                                  KStandardGuiItem::apply(),
                                                  KStandardGuiItem::discard(),
                                                  KStandardGuiItem::cancel(),
                                                  QStringLiteral("AutomaticSave"));
        if (answer == KMessageBox::PrimaryAction)
            applySettings();
        else if (answer == KMessageBox::SecondaryAction)
            resetSettings();
        if (answer != KMessageBox::Cancel)
            hide();
    } else {
        hide();
    }
}

void KMouseTool::quitSelected()
{
    if (newSettings()) {
        int answer = KMessageBox::questionTwoActionsCancel(
            this,
            i18n("There are unsaved changes in the active module.\nDo you want to apply the changes before quitting KMousetool or discard the changes?"),
            i18n("Quitting KMousetool"),
            KStandardGuiItem::apply(),
            KStandardGuiItem::discard(),
            KStandardGuiItem::cancel(),
            QStringLiteral("AutomaticSave"));
        if (answer == KMessageBox::PrimaryAction)
            applySettings();
        if (answer != KMessageBox::Cancel) {
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
    mHelpMenu->aboutApplication();
}

KMouseToolTray::KMouseToolTray(QWidget *parent)
    : KStatusNotifierItem(parent)
{
    setStatus(KStatusNotifierItem::Active);
    mStartStopAct = contextMenu()->addAction(i18nc("Start tracking the mouse", "&Start"), this, &KMouseToolTray::startStopSelected);
    contextMenu()->addSeparator();
    QAction *act = contextMenu()->addAction(i18n("&Configure KMouseTool..."), this, &KMouseToolTray::configureSelected);
    act->setIcon(QIcon::fromTheme(QStringLiteral("configure")));
    contextMenu()->addSeparator();
    act = contextMenu()->addAction(i18n("KMousetool &Handbook"), this, &KMouseToolTray::helpSelected);
    act->setIcon(QIcon::fromTheme(QStringLiteral("help-contents")));
    act = contextMenu()->addAction(i18n("&About KMouseTool"), this, &KMouseToolTray::aboutSelected);
    act->setIcon(QIcon::fromTheme(QStringLiteral("kmousetool")));
}

KMouseToolTray::~KMouseToolTray()
{
}

void KMouseToolTray::updateStartStopText(bool mousetool_is_running)
{
    QIcon icon;

    if (mousetool_is_running) {
        mStartStopAct->setText(i18n("&Stop"));
        icon = QIcon::fromTheme(QStringLiteral("kmousetool_on"));
    } else {
        mStartStopAct->setText(i18nc("Start tracking the mouse", "&Start"));
        icon = QIcon::fromTheme(QStringLiteral("kmousetool_off"));
    }
    setIconByPixmap(icon);
}

#include "moc_kmousetool.cpp"
