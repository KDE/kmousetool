/***************************************************************************
                          kaboutmousetool.h  -  description
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

#ifndef KABOUTMOUSETOOL_H
#define KABOUTMOUSETOOL_H

#include <kaboutdialog.h>

/**Display an About dialog
  *@author Jeff Roush
  */

class KAboutMouseTool : public KAboutDialog  {
public: 
	KAboutMouseTool( QWidget *parent);
	~KAboutMouseTool();
};

#endif
