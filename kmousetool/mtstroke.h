/***************************************************************************
                          mtstroke.h  -  description
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

#ifndef MTSTROKE_H
#define MTSTROKE_H
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <cstdlib>
#include <cmath>

/**Implements stroke recording for MouseTool.
  *@author Jeff Roush
  */

class Pt {

  public:
  int x,y;
  Pt () { } 
  Pt (const int xx, const int yy) { x=xx; y=yy; }
  bool sameAs(Pt pt) { return (x==pt.x&&y==pt.y); }
  bool nearTo(Pt pt, int delta) { return ( (std::abs(x-pt.x)<delta) && (std::abs(y-pt.y)<delta) ); }

  void dump();
};

class MTStroke {
  std::vector<Pt>  points;
//  std::vector<int> sequence;
  std::string sequence;
  std::map<std::string, int> sequenceMap;
  
  int stroke_minx;
  int stroke_maxx;
  int stroke_miny;
  int stroke_maxy;

  void makeDefaultSequenceMap();
  
public:
  // stroke types
  static const int bumped_mouse;
  static const int normal;
  static const int RightClick;
  static const int DoubleClick;
  static const int circle;
  static const int DontClick;
  static const int LowerLeftStroke;
  static const int UpperRightStroke;
  static const int LowerRightStroke;
  static const int UpperLeftStroke;

  // Static ints
  static int delta_xy;
  static Pt LowerLeft;
  static Pt LowerRight;
  static Pt UpperLeft;
  static Pt UpperRight;

  // min points before it can be considered a "stroke"  
  static const int min_num_points;

  static void setLowerLeft (int x, int y) { LowerLeft.x  = x;  LowerLeft.y  = y; }
  static void setLowerRight(int x, int y) { LowerRight.x = x;  LowerRight.y = y; }
  static void setUpperLeft (int x, int y) { UpperLeft.x  = x;  UpperLeft.y  = y; }
  static void setUpperRight(int x, int y) { UpperRight.x = x;  UpperRight.y = y; }
  
  void dump();
  void scale();
  void addPt(int, int);
  int  max(int, int);
  bool pointsContain(Pt pt);
  int  getStrokeType();
  void getExtent();
//  void getSequence();
  bool readSequence();
  bool writeSequence();
  void readToEndOfLine(std::ifstream &infile);

  void reset() { points.clear(); sequence = ""; }
  
	MTStroke();
	~MTStroke();
};

#endif
