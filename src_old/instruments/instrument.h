/*
 *  instrument.h
 *  Miditron
 *
 *  Created by Exhibits on 3/29/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofExtended.h"

//typedef rhythmBlock;

class soundBlock : public ofInterObj{
	string title;
	ofFont header;
	ofColor color;
public:
	double xDisp, yDisp;
	soundBlock():ofInterObj(){}
	soundBlock(string objName);
	~soundBlock();
	soundBlock(const soundBlock & t);
	void operator=(const soundBlock & t);
	void setup(string objName);
	void update(double x_dis, double y_dis);
	void draw(int _x, int _y);
	void draw(int _y);
	void draw();
	bool clickDown(int _x, int _y);
	bool clickUp();
	void mouseMotion(int _x, int _y);
	friend class instrument;
	friend class dragBlock;
};

class rhythmBlock : public soundBlock {
	ofBaseApp * baseApp;
	ofTimer rhythmTimer,playTimer;
	double playTime,rhythmTime;
	bool bPlaying,bRepeat;
	unsigned char note;
	unsigned char channel;
	vector<unsigned char> message;
public:
	rhythmBlock():soundBlock(){}
	rhythmBlock(const rhythmBlock & t);
	void setup(string title, unsigned char nt, unsigned char chan, bool repeat=false);
	void setMidi(unsigned char nt, unsigned char chan, bool repeat=false, double delay=1);
	void setDelay(double dly);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play(unsigned char vel);
	void play();
	void stop();
	void update();
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool isPlaying(){ return bPlaying;}
	friend class instrument;
};

class dragBlock : public soundBlock {
	ofButton sizeAdj;
	bool bResizing;
	bool bActive;
	double aPos;
public:
	dragBlock(const rhythmBlock & t);
	void draw(int _y);
	void draw();
	void mouseMotion(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool active(double xPos);
	friend class instrument;
};

class instrument : public ofInterObj{
	rhythmBlock base;
	bool bHolding;
	vector<dragBlock> blocks;
	string title;
	double xoff, yoff;
	double xDisp,yDisp;
	int lastBlock;
public:
	dragBlock & operator[](int i);
	int size(){ return blocks.size();}
	void clear(){ blocks.clear();}
	instrument():ofInterObj(){}
	instrument(string title, unsigned char chan, unsigned char nt, bool repeat=false);
	void update(int x_disp, int y_disp);
	void update(int disp,ofDirection dir);
	void draw(int _x, int _y);
	void draw();
	void drawBackground();
	bool clickDown(int _x, int _y);
	bool clickUp();
	void mouseMotion(int _x, int _y);
	void setMidi(unsigned char chan, unsigned char nt, bool repeat=false);
	void setColor(unsigned long hex);
	void setDelay(double dly);
	void setup(string objName, unsigned char chan, unsigned char nt, bool repeat);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play(unsigned char vel);
	void play();
	void stop();
	bool active(double pos);
	friend class bandBar;
};
	
class timeline {
	
};
	

