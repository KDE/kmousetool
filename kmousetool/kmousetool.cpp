/***************************************************************************
                          kmousetool.cpp  -  description
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


#include "Xmd_kmousetool.h"
#include "kmousetool.h"
#include "kmousetool.moc"
#include <kconfig.h>
#include <X11/Intrinsic.h>     /* Intrinsics Definitions*/
#include <X11/StringDefs.h>    /* Standard Name-String definitions*/
#include <X11/extensions/xtestext1.h>    /* Standard Name-String definitions*/
#include <X11/extensions/XTest.h>    /* Standard Name-String definitions*/
//#include <X11/Xaw/Label.h>     /* Athena Label Widget */
//#include <X11/Xatom.h>     /* For XA_WINDOW */
#include <kdialog.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qpoint.h>
#include <qnamespace.h>
#include <kaboutdialog.h>
#include <kmessagebox.h>
#include <kaudioplayer.h>
#include <kstandarddirs.h>
#include <qsound.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include "kaboutmousetool.h"
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>


#include <arts/soundserver.h>
#include <kwin.h>		// for kwin::info
//#include <netwm_def.h>
#include <netwm.h>
#include <unistd.h> 		// for pid_t

#include <iostream>

#include "mtstroke.h"

using namespace Arts;

int currentXPosition;
int currentYPosition;

//using namespace std;

#undef long

/**
* Gnarly X functions
*/
void queryPointer(Window *pidRoot, Window *pidWin, int *pnRx, int *pnRy, int *pnX, int *pnY, unsigned int *puMask);
int CursorHasMoved();
void LeftClick();
void RightClick();
void DoubleClick();
void LeftDn();
void LeftUp();

// Declarations
Display *display;

//SimpleSoundServer sound_server(Reference("global:Arts_SimpleSoundServer"));


void KMouseTool::init_vars()
{
    continue_timer = 1;
    tick_count = 0;
    max_ticks = dwell_time+1;
    mouse_is_down = FALSE;

    loadOptions();

    // If the ~/.mousetool directory doesn't exist, create it
    //	mSoundFileName = QDir::homeDirPath();
    mSoundFileName = locate("sound", "mousetool_tap.wav");
    mplayer = new KAudioPlayer(mSoundFileName);


    // find application file
    appfilename = locate("exe", "kmousetool");
//    cout << "appfilename: " << appfilename << "\n";

    // find (possibly not-yet-created) resource file
//    rcfilename = locateLocal("config", "kmousetoolrc");
//    std::cerr << "rcfilename: " << rcfilename << "\n";

    // find the user's autostart directory
    autostartdirname = KGlobalSettings::autostartPath();
//    cout << "autostartPath: " << autostartdirname << "\n";

    //	SimpleSoundServer server(Reference("global:Arts_SimpleSoundServer"));
    //	sound_server(Reference("global:Arts_SimpleSoundServer"));

    //	QDir cfgdir (QDir::homeDirPath());
    //	if (!cfgdir.exists()) {
    //		KMessageBox::sorry(this,
    //								i18n("MouseTool configuration directory doesn't exist.\nI'm creating it now.",
    //								i18n("Need to Create Configuration Directory"));
    //		QDir::mkdir(mSoundFileName);
    //	}

  MTStroke::setUpperLeft(0,0);
  MTStroke::setUpperRight(1023,0);
  MTStroke::setLowerLeft(0,767);
  MTStroke::setLowerRight(1023,767);
}


void KMouseTool::timerEvent( QTimerEvent * )
{
    if (!mousetool_is_running)
	return;

    if (!continue_timer) {
	killTimers();
	return;
    }

    max_ticks = dwell_time + drag_time;
    stroke.addPt(currentXPosition, currentYPosition);

    if (CursorHasMoved()) {
			if (mousetool_just_started) {
				mousetool_just_started = false;
				tick_count = max_ticks;
			}
			else
				tick_count = 0;
			return;
    }


    // The function CursorHasMoved() sets the global position variables.
//	QWidget * pw = QApplication::widgetAt(currentXPosition, currentYPosition);
//	printf("%s\n", pw->name());
//	if (pw)
//		printf("%s\n", pw->name());
//	else
//		printf("false\n");

    if (tick_count<max_ticks)
	tick_count++;


  // If the mouse has paused ...
	if (tick_count==dwell_time) {
    int strokeType = stroke.getStrokeType();

    // if we're dragging the mouse, ignore stroke type
    if (mouse_is_down) {
        normalClick();
        return;
    }

    printint ("stroketype: %d\n", strokeType);
    if (!strokesEnabled) {
        normalClick();
        return;
    }
    if (strokeType == MTStroke::DontClick) {
//      fprintf(stderr, "DontClick\n");
      return;
    }
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
		    mouse_is_down = TRUE;
    		tick_count = 0;
		    playTickSound();
	    } else if (mouse_is_down) {
    		LeftUp();
		    mouse_is_down = FALSE;
    		tick_count = max_ticks;
	    }
    }
    else {               // not smart_drag_on
	    LeftClick();
	    playTickSound();
    }
}

// This function isn't happy yet.
void KMouseTool::playTickSound()
{
    if (!playSound)
	return;

    mplayer->play();
    // This is a bit slow.
    //	KAudioPlayer::play(mSoundFileName);
    return;

    // the solution seems to be to use MCOP,
    // but SimpleSoundServer is not cooperating

    // When using the following code, make sure to link with
    // Put " -lsoundserver_idl" in the linker options in KDevelop.

//    cout << "About to load server\n";
    //  SimpleSoundServer server = SimpleSoundServer::_fromString("global:Arts_SimpleSoundServer");
    //	SimpleSoundServer *ss = new SimpleSoundServer(Reference("global:Arts_SimpleSoundServer"));
    //	SimpleSoundServer sound_server2(Reference("global:Arts_SimpleSoundServer"));
//    cout << "loaded server\n";

    //	if(sound_server.isNull())
    //		return;
    //	cout << "About to play sound\n";
    //	sound_server.play(mSoundFileName.latin1());
    //	cout << "playing '" << mSoundFileName << "'\n";

    //	Another approach is to try using QSound.
    //	QSound depends on Network Audio Server, which doesn't seem to work on my Debian Woody system.
    // 	I haven't spent much time working on it, though.
    //	QSound::play(mSoundFileName);
    //	cout << "available: " << QSound::available() << "'\n";
}

KMouseTool::KMouseTool(QWidget *parent, const char *name) : QWidget(parent, name)
{
    init_vars();

    // Options grid
    int rows = 2;
    int cols = 2;
//    int border = 4;
    int spacing = -1;
    // each button is 8 chars wide by 2 high
    int w = fontMetrics().maxWidth()*8;
    int h = fontMetrics().height()*2;

    ///////////////////////////////////////////////////////////
    // topLayout is the top-level widget that contains everything else
    QVBoxLayout *topLayout = new QVBoxLayout( this, 0, KDialog::spacingHint() );
    QGridLayout *checkboxGrid = new QGridLayout(2);

    QGridLayout *optionsLayout = new QGridLayout( rows, cols, spacing, "OptionsLayout" );
//    QGridLayout *optionsLayout = new QGridLayout( topLayout, rows, cols, spacing, "OptionsLayout" );

//    topLayout->addWidget( optionsLayout );
//    QGridLayout *optionsLayout = new QGridLayout( this, rows, cols, border, spacing, "OptionsLayout" );

//    QLabel *label1 = new QLabel( this, "label1" );
//    label1->setText( i18n("MouseTool for KDE") );

    ///////////////////////////////////////////////////////////
    // First item added to topLayout
    // Label: "MouseTool for KDE"
//    topLayout->addWidget( label1 );

    // Checkbox options

    ///////////////////////////////////////////////////////////
    // Second item added to topLayout
    // Grid of check boxes
    topLayout->addLayout(checkboxGrid);

    mcbDrag = new QCheckBox(i18n("Smart drag"), this, "smartDragCheckbox");
    mcbDrag ->setChecked(smart_drag_on);
    checkboxGrid->addWidget( mcbDrag, 0,0);

    mcbStart = new QCheckBox(i18n("Start with KDE"), this, "startCheckbox");
//    mcbStart->setChecked();
    checkboxGrid->addWidget( mcbStart, 0,1);

    mcbClick = new QCheckBox(i18n("Audible click"), this, "clickCheckbox");
    mcbClick->setChecked(playSound);
    checkboxGrid->addWidget( mcbClick, 1,0);

    mcbStroke = new QCheckBox(i18n("Enable strokes"), this, "strokeCheckbox");
    mcbStroke->setChecked(strokesEnabled);
    checkboxGrid->addWidget( mcbStroke, 1,1);

    //	checkboxGrid->addWidget( new QCheckBox("box 4", this, "Checkbox4"), 1,1);

    connect(mcbDrag,  SIGNAL(clicked()), this, SLOT(cbDragClicked()));
    connect(mcbStart, SIGNAL(clicked()), this, SLOT(cbStartClicked()));
    connect(mcbClick, SIGNAL(clicked()), this, SLOT(cbClickClicked()));
    connect(mcbStroke,SIGNAL(clicked()), this, SLOT(cbStrokesClicked()));

    ///////////////////////////////////////////////////////////
    // Third item added to topLayout
    // Grid of Dwell time/Drag time boxes
//    topLayout->addLayout(optionsLayout);

    QHBoxLayout *hlayTimes = new QHBoxLayout( topLayout, 10 );

    hlayTimes->addLayout(optionsLayout);

    // Apply button
    mbuttonApply = new QPushButton( i18n("Apply\nTimes"), this, "buttonApply" );
    mbuttonApply->setFixedSize(QSize(w,static_cast<int>(1.5*h)));
    // hlayApplyStart->addWidget( mbuttonApply );
    hlayTimes->addWidget( mbuttonApply );

    // Buttons to apply settings and to start/stop

    ///////////////////////////////////////////////////////////
    // Fourth item added to topLayout
    // Button
    QHBoxLayout *hlayApplyStart = new QHBoxLayout( topLayout, 10 );

    // About button
    mbuttonAbout = new QPushButton( i18n("About"), this, "buttonAbout" );
    mbuttonAbout->setDefault(FALSE);
    mbuttonAbout->setFixedSize(QSize(w,h));
    hlayApplyStart->addWidget( mbuttonAbout );

    // Start/stop button
    mbuttonStart = new QPushButton( i18n("Start"), this, "buttonStart" );
    mbuttonStart->setDefault(TRUE);
    mbuttonStart->setFixedSize(QSize(w,h));
    hlayApplyStart->addWidget( mbuttonStart );



    connect(mbuttonAbout, SIGNAL(clicked()), this, SLOT(aboutButtonClicked()));
    connect(mbuttonStart, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
    connect(mbuttonApply, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));

    // Two text boxes for dwell and drag times

    // Dwell Time

    //	QLabel *dwellLabel = new QLabel( "dwellLabel", dwellLabel );
    //  dwellLabel->setText( i18n("Dwell time (1/10 sec):") );

    //	QLabel *dwellLabel = new QLabel( this, "dwellLabel" );
    //  dwellLabel->setText( i18n("Dwell time (1/10 sec):") );
    //  optionsLayout->addWidget( dwellLabel, 0,0);

    mDwellTimeLabel = new QLabel(i18n("Dwell time (1/10 sec):"), this);
    optionsLayout->addWidget( mDwellTimeLabel, 0,0);

    w = fontMetrics().maxWidth()*3;
    h = static_cast<int>(fontMetrics().height()*1.5);

    mDwellTimeEdit = new QLineEdit( this, "dwellTimeEdit" );
    mDwellTimeEdit->setFixedSize(QSize(w,h));
    QString dwellString;
    dwellString.setNum(dwell_time);
    mDwellTimeEdit->setText(dwellString);
    optionsLayout->addWidget( mDwellTimeEdit, 0,1, AlignLeft);

    // Drag Time

    mDragTimeLabel = new QLabel(i18n("Drag time (1/10 sec):"), this);
    optionsLayout->addWidget( mDragTimeLabel, 1,0);

    mDragTimeEdit = new QLineEdit( this, "dragTimeEdit" );
    mDragTimeEdit->setFixedSize(QSize(w,h));
    QString dragString;
    dragString.setNum(drag_time);
    mDragTimeEdit->setText(dragString);
    optionsLayout->addWidget( mDragTimeEdit, 1,1, AlignLeft);

    showEnabledWidgets();

		// When we first start, it's nice to not click immediately.
		// So, tell MT we're just starting
		mousetool_just_started = true;

    startTimer(100);
}

KMouseTool::~KMouseTool()
{
}

// *********************************************************
// Raw X functions
// Begin mouse monitoring and event generation code.
// these should be moved to a separate file.
void LeftClick()
{
#ifndef DEBUG_MOUSETOOL
    XTestFakeButtonEvent(display, 1, TRUE, 0);
    XTestFakeButtonEvent(display, 1, FALSE, 0);
#else
    fprintf (stderr, "LeftClick()\n");
#endif
}

void DoubleClick()
{
#ifndef DEBUG_MOUSETOOL
    XTestFakeButtonEvent(display, 1, TRUE, 0);
    XTestFakeButtonEvent(display, 1, FALSE, 100);
    XTestFakeButtonEvent(display, 1, TRUE, 200);
    XTestFakeButtonEvent(display, 1, FALSE, 300);
#else
    fprintf (stderr, "DoubleClick()\n");
#endif
}

void RightClick()
{
#ifndef DEBUG_MOUSETOOL
    XTestFakeButtonEvent(display, 3, TRUE, 0);
    XTestFakeButtonEvent(display, 3, FALSE, 0);
#else
    fprintf (stderr, "RightClick()\n");
#endif
}


void LeftDn()
{
#ifndef DEBUG_MOUSETOOL
    XTestFakeButtonEvent(display, 1, TRUE, 0);
    // cout << "down\n";
#else
    fprintf (stderr, "LeftDn()\n");
#endif
}


void LeftUp()
{
#ifndef DEBUG_MOUSETOOL
    XTestFakeButtonEvent(display, 1, FALSE, 0);
//    cout << "up\n";
#else
    fprintf (stderr, "LeftUp()\n");
#endif
}

/*
Window get_client_leader(Window window)
{
      Window client_leader = 0;
      Atom atom;
      Atom actual_type;
      int actual_format;
      unsigned long nitems;
      unsigned long bytes_after;
      unsigned char *prop = NULL;

      atom=XInternAtom(wglobal.dpy, "WM_CLIENT_LEADER", False);

      if(XGetWindowProperty(wglobal.dpy, window, atom,
                        0L, 1L, False, AnyPropertyType, &actual_type,
                        &actual_format, &nitems, &bytes_after,
                        &prop) == Success)
      {
              if(actual_type == XA_WINDOW && actual_format == 32
                                      && nitems == 1  && bytes_after == 0)
                      client_leader=*((Window *)prop);
              XFree(prop);
      }
      return client_leader;
}

char *get_window_cmd(Window window)
{
      char **argv, *command=NULL;
      int id, i, len=0, argc=0;

      if(XGetCommand(wglobal.dpy, window, &argv, &argc) && (argc > 0))
              ;
      else if((id=get_client_leader(window)))
              XGetCommand(wglobal.dpy, id, &argv, &argc);

      if(argc > 0){
              for(i=0; i < argc; i++)
                      len+=strlen(argv[i])+1;
              command=ALLOC_N(char, len+1);
              strcpy(command, argv[0]);
              for(i=1; i < argc; i++){
                      strcat(command, " ");
                      strcat(command, argv[i]);
              }
      }

      return command;
}
*/

void queryPointer(Window *pidRoot, Window *pidWin, int *pnRx, int *pnRy, int *pnX, int *pnY, unsigned int *puMask)
{
    Window id1, id2, idRoot;
    int screen_num;
    char *win_name;

    Bool bTest;          /* for testing only */

    //	printf("get display\n");

    screen_num = DefaultScreen(display);
    //	printf("Get root window\n");

    idRoot = RootWindow(display,screen_num);

    id1 = 0;
    //	printf("Query pointer\n");
    bTest = XQueryPointer(display, idRoot, &idRoot, &id2, pnRx, pnRy, pnX, pnY, puMask);
    	static Window old_id2 = 0;
	if (old_id2 != id2) {
//		printf("queried pointer\n");
//        	printf("root: %x, id1: %x, id2: %x -- ", idRoot, id1, id2);
//		XFetchName(display, id1, &win_name);
//        	printf("win_name(id1): %s: ", win_name);
//        	Try looking at XGetWindowProperty
		XFetchName(display, id2, &win_name);
		XTextProperty prop;
		XGetWMName(display, id2, &prop);
//		XGetCommand(display, id2, &argv, &argc);

    // try to read WM_COMMAND
    int argc;
    char **argv;
    QString command = "(null)";
    Status ok;
    ok = XGetCommand(display, id2, &argv, &argc);

    if (0 != ok) {
	if (argc > 0 && argv != NULL) {
	    printf("\nGot a command: argc = %d\n", argc);
	    command = argv[0];
	    int i = 1;
	    while (i < argc) {
		command += " ";
		command += argv[i];
		++i;
	    }

	    XFreeStringList(argv);
	}
    }
    XClassHint hint;
    ok = XGetClassHint(display, id2, &hint);

    QString resName = "(null)";
    QString resClass= "(null)";

    if (0 != ok) { // we managed to read the class hint
        resName =  hint.res_name;
        resClass = hint.res_class;
    }
    // testing kde/QT methods
//    KWin::Info inf;
//    inf = KWin::info(id2);
     NETWinInfo inf( display, id2, idRoot,
             NET::WMState |
             NET::WMStrut |
             NET::WMWindowType |
             NET::WMName |
             NET::WMVisibleName |
             NET::WMDesktop |
             NET::WMPid |
             NET::WMKDEFrameStrut |
             NET::XAWMState
             );
//    Net::Windowtype type
//        	printf("prop: (%s, %d, %d) -- ", prop.value, prop.format, prop.nitems);
//        	printf("win_name(%x): %s -- ", id2, win_name);
//        	printf("resname: %s, resclass: %s -- ", resName.latin1(), resClass.latin1());
//        	printf("visibleName: %s, name: %s -- ", inf.visibleName.latin1(), inf.name.latin1());
//        	printf("command: %s\n", command.latin1());
//        	printf("type: %d: pid = %d\n", inf.windowType, inf.pid());
//        	printf("display: %d: id: %x: type: %d: pid = %d\n", display, id2, inf.windowType(), inf.pid());
	}
	old_id2 = id2;

    //	while (0 != id2)
    //	  {
    //	    id1 = id2;
    //	    bTest = XQueryPointer(display, id1, &idRoot, &id2, pnRx, pnRy, pnX, pnY, puMask);
    //	    /*       	    printf("root: %x, id1: %x, id2: %x\n", idRoot, id1, id2);     */
    //	  }
    *pidRoot = idRoot;
    if (0 == id1)
	*pidWin = idRoot;
    else
	*pidWin  = id1;
}

// return 1 if cursor has moved, 0 otherwise
int CursorHasMoved()
{
    static int nOldRootX = -1;
    static int nOldRootY = -1;
    //	static int nOldWinX = -1;
    //	static int nOldWinY = -1;

    //	XEvent evButtonEvent;
    Window idRootWin, idChild;
    int nRootX, nRootY, nWinX, nWinY;
    unsigned int uintMask;

    //	printf ("nRootX, nRootY", );
    int nRes;
    nRes = 0;
    //	printf("querying pointer\n");
    queryPointer(&idRootWin, &idChild, &nRootX, &nRootY, &nWinX, &nWinY, &uintMask);
    //	printf("end query pointer\n");
    if (nRootX!=nOldRootX)
	nRes = 1;
    if (nRootY!=nOldRootY)
	nRes = 1;
    //	if (nWinX!=nOldWinX)
    //	  nRes = 1;
    //	if (nWinY!=nOldWinY)
    //	  nRes = 1;

    nOldRootX = nRootX;
    nOldRootY = nRootY;
    //	nOldWinX = nWinX;
    //	nOldWinY = nWinY;
    currentXPosition = nOldRootX;
    currentYPosition = nOldRootY;

//If child is FALSE and there is a child widget at position (
    return nRes;
}
// End mouse monitoring and event creation code


// Slots

// Checkboxes
void KMouseTool::cbDragClicked()
{
    smart_drag_on = !smart_drag_on;
    mDragTimeEdit->setEnabled(smart_drag_on);
    mDragTimeLabel->setEnabled(smart_drag_on);
    saveOptions();
}

void KMouseTool::cbStartClicked()
{
    QString sym = autostartdirname;;
    sym += "kmousetool";			// sym is now full path to symlink
//    cout << "link path: " << sym << "\n";
    QFileInfo fi(sym);
    QString cmd;
    if (fi.exists()) 			// if it exists, delete it
	cmd.sprintf("rm -f %s", sym.latin1());
    else  			// if it doesn't exist, make it
	cmd.sprintf("ln -s %s %s", appfilename.latin1(), autostartdirname.latin1());
    system(cmd.ascii());
//    cout << "command: " << cmd << "\n";
}

void KMouseTool::cbClickClicked()
{
    playSound = !playSound;
    //	KMessageBox::sorry(this, i18n("Sorry, this hasn't been implemented yet"), i18n("Pay no attention to the man behind the curtain"));
    saveOptions();
}

void KMouseTool::cbStrokesClicked()
{
  strokesEnabled = !strokesEnabled;
}
// Buttons at the bottom of the dialog
void KMouseTool::applyButtonClicked()
{
    bool ok = true;
    int drag, dwell;

    drag = (mDragTimeEdit->text()).toInt( &ok ) ;
    if (!ok) {
	KMessageBox::sorry(this, i18n("Please enter a number for the drag time."), i18n("Invalid Value"));
	return;
    }
    dwell = (mDwellTimeEdit->text()).toInt( &ok );
    if (!ok) {
	KMessageBox::sorry(this, i18n("Please enter a number for the dwell time."), i18n("Invalid Value"));
	return;
    }

    // minimum and maximum values for drag and dwell times
    int min = 1;
    int max = 40;
    // verify that values are between min and max
    if (drag<min || drag>max) {
	KMessageBox::sorry(this, i18n("Please enter a number between %1 and %2 for the drag time.").arg(min).arg(max), i18n("Invalid Value"));
	return;
    }
    if (dwell<min || dwell>max) {
	KMessageBox::sorry(this, i18n("Please enter a number between %1 and %2 for the dwell time.").arg(min).arg(max), i18n("Invalid Value"));
	return;
    }

    // The drag time must be less than the dwell time
    if ( dwell < drag) {
	KMessageBox::sorry(this, i18n("The drag time must be less than or equal to the dwell time."), i18n("Invalid Value"));
	return;
    }

    // if we got here, we must be okay.
    dwell_time = dwell;
    drag_time  = drag;
    tick_count = max_ticks;
    saveOptions();
}

// Update state of widgets to show which are enabled and which aren't
void KMouseTool::showEnabledWidgets()
{
    mcbStart->setEnabled(mousetool_is_running);
    mcbClick->setEnabled(mousetool_is_running);
    mcbDrag->setEnabled(mousetool_is_running);
    mcbStroke->setEnabled(mousetool_is_running);
    mDwellTimeLabel->setEnabled(mousetool_is_running);
    mDwellTimeEdit->setEnabled(mousetool_is_running);
    mbuttonApply->setEnabled(mousetool_is_running);
    if (mousetool_is_running) {
	mDragTimeLabel->setEnabled(smart_drag_on);
	mDragTimeEdit->setEnabled(smart_drag_on);
	mbuttonStart->setText(i18n("Stop"));
    }
    else {
	mDragTimeLabel->setEnabled(false);
	mDragTimeEdit->setEnabled(false);
	mbuttonStart->setText(i18n("Start"));
    }
}

// Save options to kmousetoolrc file
void KMouseTool::loadOptions()
{
    kapp->config()->setGroup("UserOptions");

    playSound = kapp->config()->readBoolEntry("AudibleClick",false);
    smart_drag_on = kapp->config()->readBoolEntry("SmartDrag",false);

    dwell_time = kapp->config()->readNumEntry("DwellTime",5);
    drag_time = kapp->config()->readNumEntry("DragTime",3);
    strokesEnabled = kapp->config()->readBoolEntry("strokesEnabled",false);

    QPoint p;
    int x = kapp->config()->readNumEntry("x",0);
    int y = kapp->config()->readNumEntry("y",0);
    p.setX(x);
    p.setY(y);
    move(p);

    mousetool_is_running = kapp->config()->readBoolEntry("MouseToolIsRunning",false);
    display = XOpenDisplay(NULL);

    // Okay, restoring as minimized is not done this way ...
    // (I get an icon on the tasbar, but I also get a window -- with no widgets)
    // bool b = kapp->config()->readBoolEntry("IsMinimized",false);
//    if (b)
//	showMinimized();
}

// Save options to kmousetoolrc file
void KMouseTool::saveOptions()
{
    QPoint p = pos();
    int x = p.x();
    int y = p.y();

    kapp->config()->setGroup("ProgramOptions");
    kapp->config()->writeEntry("Version", KMOUSETOOL_VERSION);
    kapp->config()->setGroup("UserOptions");
    kapp->config()->writeEntry("x", x);
    kapp->config()->writeEntry("y", y);
    kapp->config()->writeEntry("strokesEnabled", strokesEnabled);
    kapp->config()->writeEntry("IsMinimized", isMinimized());
    kapp->config()->writeEntry("DwellTime", dwell_time);
    kapp->config()->writeEntry("DragTime", drag_time);
    kapp->config()->writeEntry("SmartDrag", smart_drag_on);
    kapp->config()->writeEntry("AudibleClick", playSound);
    kapp->config()->writeEntry("MouseToolIsRunning", mousetool_is_running);
    kapp->config()->sync();
//    kapp->config()->writeEntry("", );
//    kapp->config()->writeEntry("", );
    /*
    if (rcfilename.isEmpty())
	return;
    ofstream rcfile(rcfilename);
    if (!rcfile)
	return;
    rcfile << "# Config file created by KMouseTool, version " << KMOUSETOOL_VERSION << "\"\n\n";
    rcfile << "[UserOptions]\n";
    rcfile << "DwellTime:    " << dwell_time    << "\n";
    rcfile << "DragTime:     " << drag_time     << "\n";
    rcfile << "SmartDrag:    " << smart_drag_on << "\n";
    rcfile << "AudibleClick: " << playSound << "\n";
    */
}

void KMouseTool::startButtonClicked()
{
    mousetool_is_running = !mousetool_is_running;
    showEnabledWidgets();
}

void KMouseTool::aboutButtonClicked()
{
  KAboutMouseTool dlg(this);
  dlg.exec();

}


void KMouseTool::closeEvent(QCloseEvent *e)
{
    saveOptions();
    QWidget::closeEvent(e);
}


