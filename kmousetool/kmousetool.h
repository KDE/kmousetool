/*
    SPDX-FileCopyrightText: 2002-2003 Jeff Roush <jeff@mousetool.com>
    SPDX-FileCopyrightText: 2003 Olaf Schmidt <ojschmidt@kde.org>
    SPDX-FileCopyrightText: 2003 Gunnar Schmi Dt <gunnar@schmi-dt.de>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMOUSETOOL_H
#define KMOUSETOOL_H

#include <QLabel>
#include <QTimerEvent>
#include <QWidget>

#include "mtstroke.h"
#include "ui_kmousetoolui.h"
#include <KStatusNotifierItem>

class QLabel;
class QAction;
class QMediaPlayer;
class KAudioPlayer;
class KHelpMenu;
class KMouseToolTray;

/**
 * KMouseTool is the base class of the project
 *
 * It is the main widget for the dialog
 *
 */

class KMouseTool : public QWidget, public Ui::KMouseToolUI
{
    Q_OBJECT

private:
    MTStroke mStroke;

    // boolean flags to keep track of program states
    int mMouse_is_down;
    int mContinue_timer;
    int mTick_count;
    int mDwell_time;
    int mDrag_time;
    int mMax_ticks;
    int mMin_movement;
    bool mSmart_drag_on;
    bool mPlaySound;
    bool mMousetool_is_running;
    bool mMousetool_just_started;
    bool mMoving;
    bool mStrokesEnabled;

    QString mAutostartdirname;
    QString mAppfilename;
    QString mSoundFileName;
    QMediaPlayer *mPlayer = nullptr;
    KMouseToolTray *mTrayIcon = nullptr;

    KHelpMenu *mHelpMenu = nullptr;
    QPushButton *mButtonQuit = nullptr;

    /**
     * Initialize all variables
     */
    void init_vars();

    /**
     * Take care of details of playing .wav file
     *
     * Currently uses KAudioPlayer::play(), which has an annoying delay.
     *
     * The solution seems to be to use MCOP, but I haven't been able to get that to work yet.
     */
    void playTickSound();

    /**
     * Load state of program from "kmousetool.rc" in the user's local .kde folder,
     *
     */
    void loadOptions();

    /**
     * Save state of program under the user's local .kde folder,
     * in a file named "kmousetool.rc"
     *
     */
    void saveOptions();

    /**
     * This function changes text on button depending on
     * state of time (either "start", or "stop").
     **/
    void updateStartStopText();

    /**
     * Returns true if the current values in the settings window are different
     * from the settings currently used
     **/
    bool newSettings();

    /**
     * Returns true if the current values in the settings window are identical
     * with the default settings
     **/
    bool defaultSettings();

    /**
     * Resets the values in the settings window to the settings currently used
     **/
    void resetSettings();

    /**
     * Sets the values in the settings window to the default settings
     **/
    void setDefaultSettings();

    /**
     * Applies the current values in the settings window
     **/
    bool applySettings();

    bool isAutostart();
    void setAutostart(bool start);

public Q_SLOTS:
    /**
     * This slot is called whenever a value in the settings window was changed.
     * It enabled and disables the three buttons "Defaults", "Reset" and "Apply".
     **/
    void settingsChanged();

    void startStopSelected();

    void defaultSelected();
    void resetSelected();
    void applySelected();

    void helpSelected();
    void closeSelected();
    void quitSelected();

    void aboutSelected();
    void configureSelected();

public:
    /**
     * This function runs the show; it is called once every
     * 1/10 second.
     *
     * It checks to see if SmartDrag is on, and compares the
     * current mouse position to its previous position to see
     * whether to send a down click, and up click, or wait.
     */
    void timerEvent(QTimerEvent *e) override;

    /**
     * This generates a normal click event --
     * down, up, or down-up, depending on smart-drag settings and current state
     */
    void normalClick();

    /**
     *  construtor
     */
    explicit KMouseTool(QWidget *parent = nullptr, const char *name = nullptr);

    /** destructor */
    ~KMouseTool() override;
};

class KMouseToolTray : public KStatusNotifierItem
{
    Q_OBJECT
public:
    explicit KMouseToolTray(QWidget *parent = nullptr);
    ~KMouseToolTray() override;

    void updateStartStopText(bool mousetool_is_running);
Q_SIGNALS:
    void startStopSelected();
    void configureSelected();
    void aboutSelected();
    void helpSelected();

private:
    QAction *mStartStopAct = nullptr;
};
#endif
