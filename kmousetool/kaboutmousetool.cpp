/***************************************************************************
                          kaboutmousetool.cpp  -  description
                             -------------------
    begin                : Mon May 20 2002
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

#include "kaboutmousetool.h"
#include <klocale.h>
#include <kstandarddirs.h>
#include "kmousetool.h"

// #define LAYOUT_FLAGS ( AbtTabbed | AbtTitle )
// KAboutDialog::AbtKDEStandard
//
KAboutMouseTool::KAboutMouseTool( QWidget *parent)
  :KAboutDialog( (AbtTabbed|AbtTitle), QString::fromLatin1("KMouseTool"),
		 /* KDialogBase::Help| */ KDialogBase::Close, KDialogBase::Close,
		 parent, "KMouseToolDlg", true )
{
  const QString text1 = i18n(""
		"<p><b>KMouseTool</b> is written and maintained by "
		"<a HREF=\"mailto:jeff@mousetool.com\">Jeff Roush</a>.</p>  "
	  "<p>For more information, see the man page for <b>kmousetool</b> "
	  	"or the KDE documentation, both of "
		"which should have been installed with the program, "
		"or see <a HREF=\"http://www.mousetool.com/\">www.mousetool.com</a>. </p>"
	  "<p>This program is licensed under the GPL. "
		"In addition, by using this program, you agree not to hold the author "
		"of the program responsible for any damage it may cause.</p>"
 );

  const QString text2 = i18n(""
    "If you have any ideas on how to improve this program, post them to the KDE "
    "Accessibility Mailing List, or send them directly to me, at "
		"<a HREF=\"mailto:jeff@mousetool.com\">Jeff Roush</a>.  "
		);

  const QString text3 = i18n(""
    "<p>This program is distributed under the GPL, the GNU Public License.</p>"
    "<p>A copy of this license should have been installed along with the program.</p>"
 		);

  QPixmap logo;
  QString path;
  // -----
//  path=locate("appdata", "jeff_and_trouble.png");
//	cout << "Path: " << path << "\n";
//  if(!path.isEmpty())
//		setImage(path);

  setHelp( QString::fromLatin1("khelpcenter/main.html"), QString::null );
  setTitle(i18n("KMouseTool Version %1").
	   arg(QString::fromLatin1(VERSION)) );
  addTextPage( i18n("About KMouseTool","&About"), text1, true );
  addTextPage( i18n("&Report Bugs or Wishes"), text2, true );
  addTextPage( i18n("&License Agreement"), text3, true );
  setImage( locate( "data", QString::fromLatin1("kdeui/pics/aboutkde.png")) );
//  setImageBackgroundColor( white );
}

KAboutMouseTool::~KAboutMouseTool(){
}
