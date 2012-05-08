/*
 *  conductor.h
 *  Miditron
 *
 *  Created by Exhibits on 4/1/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "timeline.h"
#include "band.h"

class midiSequencer : public midiConductor, public ofInterObj{
	int divsPerMeasure;
	ofScrollBar bar;
	ofButton	mark;
	ofFont		label;
	bandBar * band;
	double viewSize;
	double numMeasures;
	double measureLength;
	ofButton playBut;
	ofButton rewindBut;
	ofButton loopBut;
	digitDisplay display;
public:
	midiSequencer():midiConductor(),ofInterObj(){
	}
	void registerPlayback(bandBar * bnd);
	void setup(double nMeasures, double secondsPerMeasure, double pixPerSec, double vSize);
	void drawMark();
	void draw(int _x, int _y);
	void drawDivs(bool full); 
	void update();
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool mouseMotion(int _x, int _y);
	void snapTo(double & num);
	double getBarPosition();
	double cursor();
	void setScrollPos(double pos);
	void play();
	void pause();
};