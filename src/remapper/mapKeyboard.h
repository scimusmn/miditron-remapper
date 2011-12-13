/*
 *  keyMapper.h
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "keyboard.h"
#include "instrument.h"

class remapKey : public pianoKey{
protected:
public:
	string noteName;
	remapKey(double _w, double _h, char nt);
	void draw(double _x, double _y);
	void press(bool sel);
	friend class remapOctave;
	friend class remapKeyboard;
};

class remapOctave : public pianoOctave{
public:
	remapOctave(double width,char octave_begin_note);
	friend class remapKeyboard;
};

class remapKeyboard : public pianoKeyboard{
protected:
	ofFont printOut;
	dallasDrop programs;
	dallasButton clearMapped;
public:
  ofRectangle info;
	remapKeyboard(){};
	void setup(double wid,double nOctaves, unsigned char chan=1);
	void draw(double _x, double _y);
  void draw(double _x, double _y, double _w, double _h);
	bool clickDown(int _x, int _y);
	bool clickUp();
	int getButtonChoice(int num=-1);
	bool selectButton(int num){getKey().buttons.select(num);}
	bool selectButton(int key, int num){(*this)[key].buttons.select(num);}
	vector<instrument> & getActiveNotes();
	vector<instrument> & getNotes(int num);
	void drawKeyInfo();
	void drawKeyboardControls(int _x, int _y, int _w, int _h);
	friend class remapper;
};