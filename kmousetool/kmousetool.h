/***************************************************************************
                          kmousetool.h  -  description
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

#ifndef KMOUSETOOL_H
#define KMOUSETOOL_H

#include <qdir.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "version.h"

#include <kapp.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <kaudioplayer.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include "mtstroke.h"

// #define DEBUG_MOUSETOOL

#ifdef DEBUG_MOUSETOOL
#define printmsg(str) (fprintf(stderr, str))
#define printint(str, int1) (fprintf(stderr, str, int1))
#define print2ints(str, int1, int2) (fprintf(stderr, str, int1, int2))
#define print3ints(str, int1, int2, int3) (fprintf(stderr, str, int1, int2, int3))
#else
#define printmsg(str) 
#define printint(str, int1) 
#define print2ints(str, int1, int2) 
#define print3ints(str, int1, int2, int3) 
#endif


/**
* KMouseTool is the base class of the project
*
* It is the main widget for the dialog, and also (as of version 1.0) contains 
* most of the code.  This should change in version 2.0, which will have
* enough options to split the main dialog into separate parts. 
* sdkhfsdkjf
*
*/
class KMouseTool : public QWidget
{
    Q_OBJECT

    private:
  MTStroke stroke;
  
	// boolean flags to keep track of program states
	int mouse_is_down;
	int continue_timer;
	int tick_count;
	int dwell_time;
	int drag_time;
	int max_ticks;
	bool smart_drag_on;
	bool playSound;
	bool mousetool_is_running;
	bool mousetool_just_started;
  bool strokesEnabled;
  
	QString autostartdirname; 
	QString rcfilename; 
	QString appfilename; 
	QString	mSoundFileName;
	KAudioPlayer *mplayer;

	/**
	 * Edit button widget for dwell time
	 */
	QLineEdit *mDwellTimeEdit;

	/**
	 * Edit button widget for drag time.
	 */
	QLineEdit *mDragTimeEdit;
	QLabel	*mDwellTimeLabel;
	QLabel	*mDragTimeLabel;

	QCheckBox * mcbDrag; 
	QCheckBox * mcbClick; 
	QCheckBox * mcbStart;
	QCheckBox * mcbStroke;

	QPushButton *mbuttonApply;
	QPushButton *mbuttonAbout;
	QPushButton *mbuttonStart;

	void closeEvent(QCloseEvent *e);

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
	 * Update state of widgets to show which are enabled and which aren't
	 *
	 */
	void showEnabledWidgets();
	    
	    
	public slots:

	    /**
	     *
	     */
	    void applyButtonClicked();

	/**
	 * Start/stop button clicked.
	 *
	 * This function changes text on button depending on
	 * state of time (either "start", or "stop").
	 */
	void startButtonClicked();

	/**
	 * About button clicked.
	 *
	 * This button shows the "About" dialog
	 */
	void aboutButtonClicked();

	/**
	 *	User clicked on "Smart Drag" checkbox.
	 */
	void cbDragClicked();

	/**
	 *	User clicked on "Start with KDE" checkbox.
	 */
	void cbStartClicked();

	/**
	 * User clicked on "Audible Click" checkbox.
	 */
	void cbClickClicked();

	/**
	 * User clicked on "Enable Strokes" checkbox.
	 */
	void cbStrokesClicked();
    public:

	/**
	 *		This function runs the show; it is called once every
	 *		1/10 second.
	 *		
	 *		It checks to see if SmartDrag is on, and compares the
	 *		current mouse position to its previous position to see
	 *		whether to send a down click, and up click, or wait.
	 */
	void timerEvent( QTimerEvent *e );

  /**
   * This generates a normal click event --
   * down, up, or down-up, depending on smart-drag settings and current state
   */
  void normalClick();
  
	/**
	 *  construtor
	 */
	KMouseTool(QWidget* parent=0, const char *name=0);

	/** destructor */
	~KMouseTool();
};

#endif
