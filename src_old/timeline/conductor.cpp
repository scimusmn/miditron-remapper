/*
 *  conductor.cpp
 *  Miditron
 *
 *  Created by Exhibits on 4/1/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "conductor.h"

void midiSequencer::setup(double nMeasures, double secondsPerMeasure, double pixPerSec, double vSize)
{
	mark.w=mark.h=20;
	h=30;
	divsPerMeasure=4;
	w=vSize;
	label.loadFont("Arial.ttf");
	label.setSize(12);
	midiConductor::setup(nMeasures*secondsPerMeasure,pixPerSec);
	pps=pixPerSec;
	numMeasures=nMeasures;
	measureLength=secondsPerMeasure*pps;
	bar.setup(20, vSize, OF_HOR);
	bar.registerArea(vSize, playTime*pps);
	playBut.setup(64, 64, "images/play.png");
	rewindBut.setup( 48, 48, "images/rewind.png");
	loopBut.setup(48, 48, "images/repeat.png");
	display.setup(200, 6);
}

void midiSequencer::registerPlayback(bandBar * bnd)
{
	band=bnd;
	bar.x=band->w;
	setScrollPos(0);
}

void midiSequencer::drawMark()
{
	ofSetColor(255, 0, 0);
	ofTriangle(mark.x-mark.w/2, mark.y, mark.x+mark.w/2, mark.y, mark.x, mark.y+mark.h);
	ofSetColor(255, 0, 0,128);
	ofLine(mark.x, y, mark.x, ofGetHeight());
}

void midiSequencer::drawDivs(bool full)
{
	for (int i=0; i<numMeasures; i++) {
		if(!full){
			ofSetColor(0xE0C39B);
			label.drawString(ssprintf("%i",i), x+measureLength*i+1-bar.getScrollPosition(), y+label.stringHeight(ssprintf("%i",i)));
		}
		for (int j=0; j<divsPerMeasure; j++) {
			if(j%2) ofSetColor(0x60,0x43,0x1B,64);
			else ofSetColor(0x60,0x43,0x1B,128);
			ofLine(x+measureLength*i+measureLength*j/divsPerMeasure-bar.getScrollPosition(), \
				   (full)?y:(abs(j-double(divsPerMeasure)/2)<=.5)?y+10:(j==0)?y+5:y+15,\
				   x+measureLength*i+measureLength*j/divsPerMeasure-bar.getScrollPosition(), (!full)?y+h:ofGetHeight());
		}
	}
	ofSetColor(0xE0C39B);
	label.drawString(ofToString(int(numMeasures)), x+measureLength*numMeasures+1-bar.getScrollPosition(), y+label.stringHeight(ssprintf("%i",numMeasures)));
	ofSetColor(0x60,0x43,0x1B,128);
	ofLine(x+measureLength*numMeasures-bar.getScrollPosition(),y+5, x+measureLength*numMeasures-bar.getScrollPosition(), (!full)?y+h:ofGetHeight());
}

void midiSequencer::draw(int _x, int _y)
{
	x=_x;
	w=ofGetWidth()-x;
	y=_y;
	mark.y=y+10;
	
	band->drawBackground();
	//Draw measure lines
	
	drawDivs(true);
	
	band->drawInstruments();
	
	ofSetColor(0xA0835B);
	ofRect(x, y, w, h);
	ofShade(x, y+h, 15, w, OF_UP, .4);
	ofShade(x, y+h, 5, w, OF_DOWN, .3);
	
	drawDivs(false);
	
	drawMark();
	
	// Bottom scroll bar
	ofShade(_x, band->getBottomPos(), 5, w, OF_UP, .3);
	
	int botY=band->getBottomPos();
	if(bar.available()){
		bar.draw(_x, band->getBottomPos());
		botY=bar.y+bar.h+2;
	}
	
	int botH=ofGetHeight()-botY;
	//Task bar on bottom
	ofSetColor(0xA0835B);
	ofShadeBox(x,botY, w, botH, OF_DOWN, .3);
	//ofShade(x, bar.y+bar.h+2, 5, w, OF_UP, .3);
	ofShade(x,botY, 10, w, OF_DOWN, .3);
	
	double indent=8;
	ofRoundShadow(x+w/2-playBut.w/2-indent/2, botY+(botH/2-playBut.h/2)-indent/2, playBut.w+indent, playBut.h+indent, playBut.w/2+indent/2, 1);
	playBut.draw(x+w/2-playBut.w/2, botY+(botH-playBut.h)/2);
	ofRoundShadow(x+w/2-playBut.w/2-indent/2-50, botY+(botH/2-rewindBut.h/2)-indent/2,\
				  rewindBut.w+indent, rewindBut.h+indent, rewindBut.w/2+indent/2, 1);
	rewindBut.draw(x+w/2-playBut.w/2-50, botY+(botH-rewindBut.h)/2);
	ofRoundShadow(x+w/2-playBut.w/2-indent/2-100, botY+(botH/2-loopBut.h/2)-indent/2, loopBut.w+indent, loopBut.h+indent, loopBut.w/2+indent/2, 1);
	loopBut.draw(x+w/2-playBut.w/2-100, botY+(botH-loopBut.h)/2);
	
	ofRoundShadow(display.x-10-indent/2, display.y-10-indent/2,\
				  display.w+20+indent, display.h+20+indent, 5, 1);
	ofSetColor(0, 0, 0);
	ofRoundBox(display.x-10, display.y-10, display.w+20, display.h+20, 5, .2);
	ofSetColor(0, 128, 200);
	int secs=metronome.getElapsed();
	display.draw(ssprintf("%02i:%02i.%02i",(secs/1000/60),(secs/1000)%60,(secs%1000/10)), x+w/2+playBut.w/2+20, botY+(botH-display.h)/2);
}

void midiSequencer::update()
{
	band->update(-getBarPosition(),OF_HOR);
	if(isPlaying())
		band->checkActives(cursor()+band->w);
	if(loopBut.pressed()&&cursor()>band->farthestPoint()-x)
		reset();
	midiConductor::update();
	double temp=metronome.getElapsedf()*pps-w/2;
	if (bPlaying&&cursorPos>=w/2+bar.getScrollPosition()&&bar.setScrollPosition(temp));
	mark.x=cursor()+x;
	//bar.update();
}

bool midiSequencer::clickDown(int _x, int _y)
{
	if(mark.clickDown(_x, _y));
	else if(_y<mark.y+mark.h){
		mark.setPressed(true);
		setCursorPosition(_x-x+getBarPosition());
		mark.x=cursorPos+x;
	}
	else if(bar.clickDown(_x, _y));
	else if(playBut.toggle(_x, _y)){
		bPlaying=!playBut.pressed();
		if(isPlaying()) midiConductor::pause(),band->stopAll();
		else midiConductor::play();
	}
	else if(rewindBut.clickDown(_x, _y)) reset(), setScrollPos(0);
	else if(loopBut.toggle(_x, _y));
		
}

void midiSequencer::play()
{
	playBut.setPressed(true);
	midiConductor::play();
}

void midiSequencer::pause()
{
	playBut.setPressed(false);
	midiConductor::pause();
}

bool midiSequencer::clickUp()
{
	mark.clickUp();
	rewindBut.clickUp();
	bar.clickUp();
}

bool midiSequencer::mouseMotion(int _x, int _y)
{
	if(mark.pressed()){
		int newPos=_x-x+getBarPosition();
		if(_x<x) newPos=0;
		else if(_x>ofGetWidth()) newPos=ofGetWidth();
		setCursorPosition(newPos);
		mark.x=cursorPos+x;
	}
	else if(bar.mouseMotion(_x, _y));
}

void midiSequencer::snapTo(double & num)
{
	for (int i=0; i<numMeasures; i++) {
		for (int j=0; j<divsPerMeasure; j++) {
			if (abs((x+measureLength*i+measureLength*j/divsPerMeasure)-num)<=measureLength/(divsPerMeasure*2)) {
				num=x+measureLength*i+measureLength*j/divsPerMeasure;
			}
		}
	}
}

double midiSequencer::getBarPosition()
{
	return bar.getScrollPosition();
}

double midiSequencer::cursor()
{
	return cursorPos-getBarPosition();
}

void midiSequencer::setScrollPos(double pos)
{
	bar.setScrollPosition(pos);
}