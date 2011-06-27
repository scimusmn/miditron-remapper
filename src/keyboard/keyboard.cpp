/*
 *  keyboard.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/28/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "keyboard.h"

static vector<string> programNames;

int loadProgramNames(string filename){
	programNames.clear();
	ifstream k(ofToDataPath(filename).c_str());
	while(k.peek()!=EOF){
		string temp;
		getline(k, temp);
		programNames.push_back(temp);
	}
	return programNames.size();
}

vector<string> getProgramNames(){
	return programNames;
}

void parseMidiProgramNames(string filename){
	ifstream k(ofToDataPath(filename).c_str());
	while(k.peek()!=EOF){
		string temp;
		getline(k, temp);
		vector<string> tokend=ofSplitString(temp, "\t");
		if(tokend.size()>1)
			programNames.push_back(tokend[1]);
		else
			programNames.push_back(tokend[0]);
	}
	k.close();
	ofstream out(ofToDataPath(filename+"_parsed").c_str());
	for (unsigned int i=0; i<programNames.size(); i++) {
		out << programNames[i] << endl;
	}
	out.close();
}

pianoKey::pianoKey(double _w, double _h, char nt):ofInterObj(0,0,_w,_h)
{
	note=nt;
	o_w=_w;
	o_h=_h;
	bSharp=false;
	bSelected=false;
}

pianoKey::~pianoKey(){}

void pianoKey::setSharp(bool sharp)
{
	bSharp=sharp;
	if (bSharp) {
		w=o_w*2./3.;
		h=o_h*9./14.;
	}
}

void pianoKey::operator=(const pianoKey & t)
{
	note=t.note;
	x=t.x,y=t.y,w=t.w,h=t.h,o_w=t.o_w,o_h=t.o_h,bSharp=t.bSharp;
}

void pianoKey::draw(double _x, double _y)
{
	x=_x, y=_y;
	ofSetColor(0, 0, 0);
	ofRoundBox(x, y, w, h, w/8., .02);
	if(bPressed) ofSetColor(50, 150, 255);
	else if(bSelected) ofSetColor(25, 75, 190);
	else if(bSharp) ofSetColor(0, 0, 0);
	else ofSetColor(255, 255, 255);
	ofRoundBox(x+1, y+1, w-2, h-2, w/8., .3);
	ofShade(x+1, y+1, 6, h-2, OF_RIGHT, .3, false);
}

void pianoKey::press(bool sel)
{
	bPressed=sel;
}

bool pianoKey::clickDown(int _x, int _y)
{
	bool ret=false;
	if (over(_x, _y)) {
		ret=bSelected=bPressed=true;
	}
	return ret;
}

bool pianoKey::clickUp()
{
	if(bPressed) bPressed=false;
}

pianoOctave::pianoOctave(double width,char octave_begin_note):ofInterGroup()
{
	w=width,h=width/1.2;
	double wwid=w/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<12; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(pianoKey(wwid,h,nt));
	}
	for (unsigned int i=0; i<5; i++) {
		keys[sharps[i]].setSharp(true);
	}
}

void pianoOctave::setup(double width,char octave_begin_note)
{
	w=width,h=width/1.2;
	float wwid=w/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<12; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(pianoKey(wwid,h,nt));
	}
	for (unsigned int i=0; i<5; i++) {
		keys[sharps[i]].setSharp(true);
	}
}

pianoOctave::~pianoOctave()
{
	keys.clear();
}

pianoKey & pianoOctave::operator[](int i)
{
	return keys[i];
}

void pianoOctave::draw(int _x, int _y)
{
	double xpos=_x;
	if(keys.size()) keys[0].draw(_x,_y),xpos+=keys[0].w;
	for (unsigned int i=1; i<keys.size(); i++) {
		if (!keys[i].bSharp) {
			keys[i].draw(xpos,_y);
			xpos+=keys[i].w;
		}
	}
	for (unsigned int i=1; i<keys.size(); i++) {
		if (keys[i].bSharp) {
			keys[i].draw(keys[i-1].x+keys[i-1].w-keys[i].w/2,_y);
		}
	}
}

bool pianoOctave::clickDown(int _x, int _y)
{
	bool ret=false;
	for (unsigned int i=0; i<keys.size(); i++) {
		if(keys[i].bSharp&&keys[i].clickDown(_x,_y)){
			ret=true;
			oSelected=i;
			for (unsigned int j=0; j<keys.size(); j++) {
				if(j!=i) keys[j].bSelected=false;
			}
			break;
		}
	}
	if(!ret) for (unsigned int i=0; i<keys.size(); i++) {
		if(!keys[i].bSharp&&keys[i].clickDown(_x,_y)){
			ret=true;
			oSelected=i;
			for (unsigned int j=0; j<keys.size(); j++) {
				if(j!=i) keys[j].bSelected=false;
			}
			break;
		}
	}
	return ret;
}

pianoKey & pianoOctave::getKey()
{
	return keys[oSelected];
}

bool pianoOctave::clickUp()
{
	for (unsigned int i=0; i<keys.size(); i++) {
		keys[i].clickUp();
	}
}

void pianoOctave::clear()
{
	for (unsigned int i=0; i<keys.size(); i++) {
		keys[i].bSelected=false;
	}
}

pianoKeyboard::pianoKeyboard(double wid, double nOctaves):ofInterGroup()
{
	w=wid;
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(pianoOctave(wid/nOctaves,i*12));
	}
}

pianoKeyboard::~pianoKeyboard()
{
	octaves.clear();
}

void pianoKeyboard::setup(double wid,double nOctaves)
{
	w=wid;
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(pianoOctave(wid/nOctaves,i*12));
	}
	h=octaves[0].h;
}

pianoKey & pianoKeyboard::operator[](int i)
{
	return octaves[i/12][i%12];
}

void pianoKeyboard::draw(int _x, int _y)
{
	double xpos=_x;
	ofSetColor(175,112,75);
	ofRoundBox(_x-w/32., _y-w/32., w+w/16., h+w/32, w/64., .2);
	ofRoundShadow(_x-w/64., _y-w/64., w+w/32., h+w/64., h/8., .4);
	for (unsigned int i=0; i<octaves.size(); i++) {
		octaves[i].draw(xpos,_y);
		xpos+=octaves[i].w;
	}
	ofSetColor(0, 0, 0);
	ofRect(_x, _y, w, w/64.);
}

bool pianoKeyboard::clickDown(int _x, int _y)
{
	for (unsigned int i=0; i<octaves.size(); i++) {
		if (octaves[i].clickDown(_x,_y)) {
			oSelected=i;
			for (unsigned int j=0; j<octaves.size(); j++) {
				if(i!=j) octaves[j].clear();
			}
		}
	}
}

int pianoKeyboard::size()
{
	return octaves.size()*12;
}

bool pianoKeyboard::clickUp()
{
	for (unsigned int i=0; i<size(); i++) {
		octaves[i/12][i%12].setPressed(false);
	}
}

void pianoKeyboard::update()
{
}

pianoKey & pianoKeyboard::getKey()
{
	return octaves[oSelected].getKey();
}
