/***************************************************************************
                          kmousetool.cpp  -  description
                             -------------------
    begin                : Sun Jan 20 23:27:58 PST 2002
    copyright            : (C) 2002-2003 by Jeff Roush
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
#include <kmessagebox.h>
#include <kaudioplayer.h>
#include <kstandarddirs.h>
#include <qsound.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <ksystemtray.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <knuminput.h>
#include <kpopupmenu.h>
#include <kaboutapplication.h>

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
int CursorHasMoved(int minMovement);
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

void KMouseTool::resetValues()
{
    cbDrag ->setChecked(smart_drag_on);
    cbStart->setChecked(isAutostart());
    cbClick->setChecked(playSound);
    cbStroke->setChecked(strokesEnabled);
    movementEdit->setValue(min_movement);
    dwellTimeEdit->setValue(dwell_time);
    dragTimeEdit->setValue(drag_time);
}

void KMouseTool::setDefaultValues()
{
    cbDrag ->setChecked(false);
    cbStart->setChecked(false);
    cbClick->setChecked(false);
    cbStroke->setChecked(false);
    movementEdit->setValue(5);
    dwellTimeEdit->setValue(5);
    dragTimeEdit->setValue(3);
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

    moving = moving?CursorHasMoved(1):CursorHasMoved(min_movement);
    if (moving) {
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

KMouseTool::KMouseTool(QWidget *parent, const char *name) : KMouseToolUI(parent, name)
{
    init_vars();
    resetValues();

    connect(buttonStartStop, SIGNAL(clicked()), this, SLOT(startStopSelected()));
    connect(buttonDefault, SIGNAL(clicked()), this, SLOT(defaultSelected()));
    connect(buttonReset, SIGNAL(clicked()), this, SLOT(resetSelected()));
    connect(buttonApply, SIGNAL(clicked()), this, SLOT(applySelected()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(helpSelected()));
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(closeSelected()));
    connect(buttonQuit, SIGNAL(clicked()), this, SLOT(quitSelected()));

    // When we first start, it's nice to not click immediately.
    // So, tell MT we're just starting
    mousetool_just_started = true;

    startTimer(100);
    trayIcon = new KMouseToolTray (this, "systemTrayIcon");
    updateStartStopText ();
    connect(trayIcon, SIGNAL(startStopSelected()), this, SLOT(startStopSelected()));
    connect(trayIcon, SIGNAL(configureSelected()), this, SLOT(configureSelected()));
    connect(trayIcon, SIGNAL(aboutSelected()), this, SLOT(aboutSelected()));
    connect(trayIcon, SIGNAL(helpSelected()), this, SLOT(helpSelected()));
    connect(trayIcon, SIGNAL(quitSelected()), this, SLOT(quitSelected()));

    aboutDlg = new KAboutApplication (0, "KMouseToolDlg", false);
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
int CursorHasMoved (int minMovement)
{
    static int nOldRootX = -1;
    static int nOldRootY = -1;

    //	XEvent evButtonEvent;
    Window idRootWin, idChild;
    int nRootX, nRootY, nWinX, nWinY;
    unsigned int uintMask;

    queryPointer(&idRootWin, &idChild, &nRootX, &nRootY, &nWinX, &nWinY, &uintMask);

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
    sym += "kmousetool";			// sym is now full path to symlink
    QFileInfo fi(sym);

    return fi.exists();
}

void KMouseTool::setAutostart (bool start)
{
    QString sym = autostartdirname;
    sym += "kmousetool";			// sym is now full path to symlink
    QFileInfo fi(sym);
    QString cmd;

    if (start) {
       if (!fi.exists())  			// if it doesn't exist, make it
	cmd.sprintf("ln -s %s %s", appfilename.latin1(), autostartdirname.latin1());
    }
    else {
       if (fi.exists()) 			// if it exists, delete it
	    cmd.sprintf("rm -f %s", sym.latin1());
    }
    system(cmd.ascii());
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
    return true;
}

// Save options to kmousetoolrc file
void KMouseTool::loadOptions()
{
    kapp->config()->setGroup("UserOptions");

    playSound = kapp->config()->readBoolEntry("AudibleClick",false);
    smart_drag_on = kapp->config()->readBoolEntry("SmartDrag",false);

    dwell_time = kapp->config()->readNumEntry("DwellTime",5);
    drag_time = kapp->config()->readNumEntry("DragTime",3);
    min_movement = kapp->config()->readNumEntry("Movement",5);
    strokesEnabled = kapp->config()->readBoolEntry("strokesEnabled",false);

    QPoint p;
    int x = kapp->config()->readNumEntry("x",0);
    int y = kapp->config()->readNumEntry("y",0);
    p.setX(x);
    p.setY(y);
    move(p);

    mousetool_is_running = kapp->config()->readBoolEntry("MouseToolIsRunning",false);
    display = XOpenDisplay(NULL);
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
    kapp->config()->writeEntry("IsMinimized", isHidden());
    kapp->config()->writeEntry("DwellTime", dwell_time);
    kapp->config()->writeEntry("DragTime", drag_time);
    kapp->config()->writeEntry("Movement", min_movement);
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

void KMouseTool::updateStartStopText()
{
  if (mousetool_is_running)
    buttonStartStop->setText(i18n("&Stop"));
  else
    buttonStartStop->setText(i18n("&Start"));
  trayIcon->updateStartStopText(mousetool_is_running);
}

void KMouseTool::closeEvent(QCloseEvent *e)
{
	hide();
    e->ignore();
}

/******** SLOTS **********/

// Buttons within the dialog
void KMouseTool::startStopSelected()
{
   mousetool_is_running = !mousetool_is_running;
   updateStartStopText();
}

void KMouseTool::defaultSelected()
{
    setDefaultValues();
}

void KMouseTool::resetSelected()
{
    resetValues();
}

void KMouseTool::applySelected()
{
    applySettings();
}

// Buttons at the bottom of the dialog
void KMouseTool::helpSelected()
{
  kapp->invokeHelp();
}

void KMouseTool::closeSelected()
{
  hide();
}

void KMouseTool::quitSelected()
{
  saveOptions();
  kapp->quit();
}

// Menu functions
void KMouseTool::configureSelected()
{
  show();
  raise();
  setActiveWindow();
}

void KMouseTool::aboutSelected()
{
  aboutDlg->show();
}



KMouseToolTray::KMouseToolTray (QWidget *parent, const char *name)
 : KSystemTray (parent, name)
{
   startStopId = contextMenu()->insertItem (i18n("&Start"), this, SIGNAL(startStopSelected()));
   contextMenu()->insertItem (i18n("&Configure KMouseTool..."), this, SIGNAL(configureSelected()));
   contextMenu()->insertItem (i18n("&About KMouseTool"), this, SIGNAL(aboutSelected()));
   contextMenu()->insertItem (i18n("&Help"), this, SIGNAL(helpSelected()));
}

KMouseToolTray::~KMouseToolTray() {
}

void KMouseToolTray::updateStartStopText(bool mousetool_is_running)
{
  QPixmap icon;

  if (mousetool_is_running) {
    contextMenu()->changeItem (startStopId, i18n("&Stop"));
    icon = KGlobal::iconLoader()->loadIcon("kmousetool_on", KIcon::Small);
  }
  else {
    contextMenu()->changeItem (startStopId, i18n("&Start"));
    icon = KGlobal::iconLoader()->loadIcon("kmousetool_off", KIcon::Small);
  }
  setPixmap (icon);
  show();
}
