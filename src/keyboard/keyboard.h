/*
 *  keyboard.h
 *  Miditron
 *
 *  Created by Exhibits on 3/28/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofExtended.h"

#define MIDI_KEYBOARD_START 36
#define MIDI_KEYBOARD_END 85

int loadProgramNames(string filename);

vector<string> getProgramNames();

void parseMidiProgramNames(string filename);

class pianoKey:public ofInterObj{
protected:
	double o_w,o_h;
	unsigned char note;
	bool bSharp,bSelected;
public:
	pianoKey(){}
	pianoKey(double _x, double _y, char note);
	~pianoKey();
	void operator=(const pianoKey & t);
	void setSharp(bool sharp);
	void draw(double _x, double _y);
	void update(bool sel);
	bool clickDown(int _x, int _y);
	bool clickUp();
	void press(bool sel);
	friend class pianoOctave;
};

class pianoOctave:public ofInterGroup{
protected:
	vector<pianoKey> keys;
	int oSelected;
public:
	pianoOctave(){}
	pianoOctave(double _w,char octave_begin_note);
	~pianoOctave();
	void setup(double _w,char octave_begin_note);
	pianoKey & operator[](int i);
	pianoKey & getKey();
	void draw(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
	void clear();
	int size(){ return keys.size(); }
};

class pianoKeyboard:public ofInterGroup{
protected:
	vector<pianoOctave> octaves;
	int oSelected;
public:
	pianoKeyboard(){};
	pianoKeyboard(double wid,double nOctaves);
	~pianoKeyboard();
	pianoKey & operator[](int i);
	void setup(double wid, double nOctaves);
	void draw(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
	void update();
	int size();
	pianoKey & getKey();
};