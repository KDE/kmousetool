/***************************************************************************
                          mtstroke.cpp  -  description
                             -------------------
    begin                : Fri Oct 11 2002
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

#include "mtstroke.h"
#include "kmousetool.h"
#include <iostream>

// these are for locating the stroke information file
#include <kstandarddirs.h>


// #include <string>

using namespace std;

const int MTStroke::DontClick        = -1;
const int MTStroke::bumped_mouse     = 0;
const int MTStroke::normal           = 1;
const int MTStroke::RightClick       = 2;
const int MTStroke::DoubleClick      = 3;
const int MTStroke::circle           = 4;
const int MTStroke::LowerLeftStroke  = 5;
const int MTStroke::UpperRightStroke = 6;
const int MTStroke::LowerRightStroke = 7;
const int MTStroke::UpperLeftStroke  = 8;

int MTStroke::delta_xy = 10;

const int MTStroke::min_num_points = 5;

Pt MTStroke::LowerLeft (0,0);
Pt MTStroke::LowerRight(0,0);
Pt MTStroke::UpperLeft (0,0);
Pt MTStroke::UpperRight(0,0);



MTStroke::MTStroke(){
  readSequence();
}
MTStroke::~MTStroke(){
}

// add the new point, but only if it's not the same as the previous point.
void MTStroke::addPt(int x, int y)
{
  if (points.size()==0) {
    points.push_back(Pt(x,y));
  }
  else {
    Pt pt(x,y);
    if (!pt.sameAs(points[points.size()-1])) {
      points.push_back(Pt(x,y));
    }
  }
}


/*
 * Loop over all the strokes;
 * return true if the given point is included
 */
bool MTStroke::pointsContain(Pt pt)
{
  std::vector<Pt>::iterator pos;
  for (pos=points.begin(); pos<points.end(); ++pos) {
    if (pt.x==pos->x && pt.y==pos->y)
      return true;
  }
  return false;
}

int MTStroke::getStrokeType()
{
  int size = points.size();

  // If the mouse moved just a bit, it was probably bumped.  Don't click.
  if (size<min_num_points)
    return normal;
//    return bumped_mouse;

  Pt lastPoint = points[points.size()-1];

  // If the mouse is pausing in a corner, then the user is either in the middle of a
  // stroke, or wants to rest the mouse.  Don't click.
  if (lastPoint.sameAs(LowerLeft) || lastPoint.sameAs(LowerRight)
   || lastPoint.sameAs(UpperLeft) || lastPoint.sameAs(UpperRight))
   return DontClick;

  // If the mouse visited a corner...
  if (pointsContain(LowerLeft)) {
    reset();
    return LowerLeftStroke;
  }
  if (pointsContain(UpperRight)) {
    reset();
    return UpperRightStroke;
  }
  scale();

  std::map<std::string, int>::iterator keypos = sequenceMap.find(sequence);
  if (keypos == sequenceMap.end()) {
    reset();
    return normal;
  }
  reset();
//  return RightClick;
  return keypos->second;
}

void MTStroke::scale()
{
  getExtent();
  int deltax = stroke_maxx - stroke_minx;
  int deltay = stroke_maxy - stroke_miny;
  int delta  = max(deltax, deltay);
  int scale = (int) delta/2;

  std::vector<Pt>::iterator pos;
  for (pos=points.begin(); pos<points.end(); ++pos) {

      // add an extra (scale/2) because the divide rounds _down_, and we want to
      // round _up_ or _down_, depending on which is closer.
    pos->x = (int) (pos->x-stroke_minx + scale/2)/scale;
    pos->y = (int) (pos->y-stroke_miny + scale/2)/scale;

    // now, get the integer representing this position and add it to the stroke sequence
    int n = 3*pos->y + pos->x + 1;
    int index = sequence.size()-1;
    n += '0';
    if (index==-1)
      sequence += n;
    else if (n!=sequence[index])
      sequence += n;
  }
}

int MTStroke::max(int n, int m)
{
  if (n>m)
    return n;
  return m;
}


/*
 * Find the bounding rectangle for the stroke
 */
void MTStroke::getExtent()
{
  stroke_minx = UpperRight.x;
  stroke_maxx = 0;
  stroke_miny = LowerLeft.y;
  stroke_maxy = 0;

  std::vector<Pt>::iterator pos;
  for (pos=points.begin(); pos<points.end(); ++pos) {
    if (stroke_minx > pos->x)
      stroke_minx   =  pos->x;
    if (stroke_maxx < pos->x)
      stroke_maxx   =  pos->x;
    if (stroke_miny > pos->y)
      stroke_miny   = pos->y;
    if (stroke_maxy < pos->y)
      stroke_maxy   = pos->y;
  }
}

// test if strokefile exists; if not, create it from defaults.
// if unable to create it,
bool MTStroke::readSequence()
{
  QString strokefilename;
  strokefilename = KStandardDirs::locate("config", QLatin1String( "kmousetool_strokes.txt" ) );
  if (strokefilename.isEmpty()) {
    // make default
    if (sequenceMap.size()==0)
      makeDefaultSequenceMap();
    writeSequence();
    return false;
  }
  ifstream infile (strokefilename.toAscii().constData());
  if (!infile) {
    // make default
    if (sequenceMap.size()==0)
      makeDefaultSequenceMap();
    writeSequence();
    return false;
  }

  while (!infile.eof()) {
    string str;
    infile >> str;
    if (str[0] == '#')
      readToEndOfLine(infile);
    else {
      // get the associated action
      string str2;
      infile >> str2;
      int n = str2[0] - '0';    // the action is a single integer digit; convert it to an int
      sequenceMap[ string(str) ] = n;
    }
  }
  return true;
}

bool MTStroke::writeSequence()
{
  QString strokefilename;
  strokefilename = KStandardDirs::locateLocal("config", QLatin1String( "kmousetool_strokes.txt" ));

  ofstream outfile (strokefilename.toAscii(), ios::out);
  if (!outfile) {
    return false;
  }

  outfile << "# This file contains definitions for valid strokes for KMouseTool\n";
  outfile << "# To make sense of the numbers: \n";
  outfile << "# The mouse path begins and ends when the mouse is paused.\n";
  outfile << "# Imagine a square enclosing the path.\n";
  outfile << "# Divide the square into 9 boxes, and number them like so:\n";
  outfile << "# 1 2 3\n";
  outfile << "# 4 5 6\n";
  outfile << "# 7 8 9\n";
  outfile << "# \n";
  outfile << "# The mouse path can then be described by a sequence of numbers:\n";
  outfile << "# for example, \"12321\" describes the mouse moving from left to right and back.\n";
  outfile << "# This general scheme follows libstroke (http://www.etla.net/~willey/projects/libstroke/)\n";
  outfile << "# although it was reimplemented from scratch for KMouseTool.\n";
  outfile << "\n";
  outfile << "# For each stroke recognized, provide an integer describing an action\n";
  outfile << "# KMouseTool can take.  At the moment, valid actions are:\n";
  outfile << "# -1     Do not click\n";
  outfile << "#  1     Normal click (use Smart Drag if that's enabled)\n";
  outfile << "#  2     Right  click\n";
  outfile << "#  3     Double click\n";
  outfile << "\n";
  outfile << "#Stroke\tAction\n";
  std::map<std::string, int>::iterator pos = sequenceMap.begin();
  while (pos != sequenceMap.end()) {
    outfile << pos->first << "\t" << pos->second << "\n";
    pos++;
  }
  return true;
}

void MTStroke::makeDefaultSequenceMap()
{
  sequenceMap[ string("12321") ] = RightClick;
  sequenceMap[ string("1321") ]  = RightClick;
  sequenceMap[ string("1231") ]  = RightClick;
  sequenceMap[ string("131") ]   = RightClick;

  sequenceMap[ string("32123") ] = DoubleClick;
  sequenceMap[ string("3213") ]  = DoubleClick;
  sequenceMap[ string("3123") ]  = DoubleClick;
  sequenceMap[ string("313") ]   = DoubleClick;
/*
  sequenceMap[ string("") ] = ;
  sequenceMap[ string("") ] = ;
  sequenceMap[ string("") ] = ;
  sequenceMap[ string("") ] = ;
  sequenceMap[ string("") ] = ;
*/
}

void MTStroke::readToEndOfLine(ifstream &infile)
{
  char ch = 'a';
  while (ch != '\n')
    infile.get(ch);
}
