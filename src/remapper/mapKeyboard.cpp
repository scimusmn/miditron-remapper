/*
 *  keyMapper.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "mapKeyboard.h"

remapKey::remapKey(double _w, double _h, char nt):pianoKey()
{
	note=nt;
	w=_w;
	h=_h;
	o_w=_w;
	o_h=_h;
	bSharp=false;
	bSelected=false;
	for (int j=0; j<3; j++) {
		int cur=notes.size();
		notes.push_back(instrument("holder",1,60,false));
		notes[cur].setDefault(true);
	}
	buttons.setup(2,25,OF_VERT,500);
	clearNotes.setup("clear mapped note", "fonts/Arial.ttf", 24);
}

remapOctave::remapOctave(double width,char octave_begin_note):pianoOctave()
{
	w=width,h=width/1.2;
	double wwid=w/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<12; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(remapKey(wwid,h,nt));
	}
	for (unsigned int i=0; i<5; i++) {
		keys[sharps[i]].setSharp(true);
	}
}

void remapKeyboard::setup(double wid,double nOctaves, unsigned char chan)
{
	w=wid;
	channel=chan;
	int numProgs=loadProgramNames("midiPrograms.ini");
	programs.setTextSize(20);
	for (unsigned int i=0; i<getProgramNames().size(); i++) {
		programs.setValue(getProgramNames()[i]);
	}
	programs.setMode(false);
	printOut.loadFont("fonts/DinC.ttf");
	printOut.setSize(30);
	printOut.setMode(OF_FONT_LEFT);
	printOut.setMode(OF_FONT_TOP);
	octaves.clear();
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(remapOctave(wid/nOctaves,i*12));
	}
	pressKey(0);
	xDis=wid/16.;
	w=w+xDis;
	h=octaves[0].h+xDis/2.;
	clearMapped.setup("reset", "Arial.ttf", 24);
}

void remapKeyboard::draw(double _x, double _y){
	pianoKeyboard::draw(_x,_y);
}

void remapKeyboard::drawActiveNotes(double _x, double _y, double span, double _w, double _h)
{
	//double width=_w/double(3*getKey().notes.size()/2+1);
	string print="drag block here";
	double width=printOut.stringWidth(print)+10;
	double height=printOut.stringHeight("K")+10;
	getKey().buttons.draw(_x,_y,span/2);
	_x=getKey().buttons[1].x+50;
	_y=getKey().buttons[1].y+50;
	for (unsigned int i=0; i<getKey().notes.size(); i++) {
		if(getKey().notes[i].isDefault()){
			getKey().notes[i].cSetup(_x+i*1.5*width, _y, width, height);
			ofSetColor(0xDD, 0xDD, 0xDD);
			ofRoundBox(_x+i*1.5*width, _y, width, height, height/4, .2);
			ofSetColor(0, 0, 0);
			printOut.drawString(print, _x+width/2+i*1.5*width, _y+3*height/4);
		}
		else {
			getKey().notes[i].draw(_x+i*1.5*width,_y);
		}
	}
}

string getNoteName(unsigned char note)
{
	string ret;
	string octave=ofToString((MIDI_KEYBOARD_START+note)/12);
	int k=note%12;
	ret=(k<2)?"C":(k<4)?"D":(k==4)?"E":(k<7)?"F":(k<9)?"G":(k<11)?"A":"B";
	if(k==1||k==3||k==6||k==8||k==10) ret+="#";
	return	ret+octave;
}

void remapKeyboard::drawNoteSelect(double _x, double _y, double span)
{
	
	ofSetColor(0,0,0,64);
	ofRectangle k(_x-50, 75, ofGetWidth()-_x+50,span);
	ofRect(k.x,k.y,k.width,k.height);
	ofSetColor(0,0,0,128);
	ofBeginShape();
	ofVertex(getKey().x+getKey().w/2, getKey().y+getKey().h/2);
	double keyX=getKey().x+getKey().w/2;
	double keyY=getKey().y+getKey().h/2;
	ofBezierVertex(keyX, keyY, (keyX<k.x)?k.x:(k.x+getKey().x+getKey().w/2)/2,
				   (keyX<k.x)?(k.y+k.height+getKey().y+getKey().h/2)/2:k.y+k.height,k.x, k.y+k.height);
	ofVertex(k.x+k.width, k.y+k.height);
	ofBezierVertex(k.x+k.width, k.y+k.height, (k.x+k.width+getKey().x+getKey().w/2)/2,
				   k.y+k.height,keyX, keyY);
	ofEndShape(true);
	
	ofSetColor(0, 0, 0, 128);
	int chose=getKey().buttons.getChoice();
	ofRoundShape(_x-20, getKey().buttons[chose].y+getKey().buttons[chose].h/2-75, ofGetWidth()-_x, 175, 20, 1);
	//buttons[activeKey()].draw(_x,_y,span/2);
	
	//Print out the titles for the different notes
	printOut.setSize(30);
	ofSetColor(255, 255, 255);
	printOut.setMode(OF_FONT_LEFT);
	printOut.setMode(OF_FONT_TOP);
	printOut.drawString("choose default sound", getKey().buttons[0].x+100, getKey().buttons[0].y-50);
	printOut.drawString("define custom sound", getKey().buttons[1].x+100,getKey().buttons[1].y-50);
	printOut.drawString(getNoteName(getKey().getNote()), programs.x+programs.w+30, programs.y);
	printOut.setMode(OF_FONT_CENTER);
	printOut.setMode(OF_FONT_BOT);
	printOut.setSize(18);
	
	//Draw the "drag block here" boxes
	//drawActiveNotes(buttons[activeKey()][1].x+100, buttons[activeKey()][1].y+50,span);
	drawActiveNotes(_x,_y,span);
	programs.draw(getKey().buttons[0].x+100, getKey().buttons[0].y+50);
}

void remapKeyboard::drawKeyboardControls(int _x, int _y, int _w, int _h)
{
	double offset=50;
	double binLeft=_x+offset/2;
	double binTop=_y+offset/2;
	double binW=_w-offset;
	double binH=_h-offset;
	ofRoundShadow(binLeft-10, binTop-10, binW+20, binH+20, 15, .4);
	ofRect(binLeft, binTop, binW, binH);
	
	double yPos=_y+offset*1/2+10;
	double xPos=_x+offset*1/2+10;
	ofSetColor(255, 255, 255);
	printOut.drawString("Select Default Instrument", xPos, yPos);
	ofSetColor(255, 255, 255);
	printOut.drawString("Clear Mapped Instruments", xPos, yPos+offset*2);
	clearMapped.draw(xPos+offset, yPos+offset*3);
	programs.draw(xPos+offset, yPos+offset);
}

void remapKeyboard::drawKeyInfo(int _x, int _y, int _w, int _h)
{
	pianoKey * key=0;
	for (unsigned int i=0; i<size(); i++) {
		if ((*this)[i].isSelected()) {
			key=&((*this)[i]);
		}
	}
	if(key){
		
		int offset=50;
		ofSetColor(0, 0, 0,192);
		ofBeginShape();
		ofVertex(key->x, key->y+key->h);
		ofVertex(_x+offset/2+20, _y+offset/2);
		ofVertex(_x+offset/2+_w-offset-20, _y+offset/2);
		ofVertex(key->x+key->w, key->y+key->h);
		ofEndShape(true);
		ofRoundShape(_x+offset/2, _y+offset/2, _w-offset, _h-offset*2, 20, true);
		ofSetColor(255,255,255);
		string intro="when this key is pressed...";
		printOut.drawString(intro, _x+offset, _y+offset);
		int nextLine=_y+offset*2+printOut.stringHeight(intro);
		int xDrawPosition=_x+2*offset;
		key->buttons[0].draw(xDrawPosition,nextLine);
		xDrawPosition+=key->buttons[0].w+offset/2;
		string dflt="play default instrument";
		ofSetColor(255,255,255);
		printOut.drawString(dflt, xDrawPosition, nextLine);
		xDrawPosition+=printOut.stringWidth(dflt)+offset*3;
		key->buttons[1].draw(xDrawPosition,nextLine);
		xDrawPosition+=key->buttons[1].w+offset/2;
		string mpd="play mapped note";
		ofSetColor(255,255,255);
		printOut.drawString(mpd, xDrawPosition, nextLine);
		xDrawPosition+=printOut.stringWidth(mpd)+offset;
		if(!key->notes[0].isDefault()){
			key->notes[0].draw(xDrawPosition,nextLine);
			key->clearNotes.draw(xDrawPosition+key->notes[0].w+offset, nextLine);
		}
		else {
			printOut.drawString("[no note is mapped, drag block to key]", xDrawPosition, nextLine);
		}

	}
}

bool remapKeyboard::clickDown(int _x, int _y)
{
	bool ret=false;
	if(programs.clickDown(_x, _y)){
		if(programs.isSelected()){
			vector<unsigned char> msg;
			msg.push_back(MIDI_PROGRAM_CHANGE+channel);
			msg.push_back(programs.getChoiceNumber());
			ofGetAppPtr()->midiToSend(msg);
		}
	}
	else if(programs.justSelected()){
		vector<unsigned char> msg;
		msg.push_back(MIDI_PROGRAM_CHANGE+channel);
		msg.push_back(programs.getChoiceNumber());
		ofGetAppPtr()->midiToSend(msg);
	}
	else if(clearMapped.clickDown(_x, _y)){
		for (unsigned int i=0; i<(*this).size(); i++) {
			(*this)[i].resetInstruments();
			selectButton(i,0);
		}
	}
	else ret=pianoKeyboard::clickDown(_x,_y);
	return ret;
}

bool remapKeyboard::clickUp()
{
	programs.clickUp();
	clearMapped.clickUp();
	return pianoKeyboard::clickUp();
}

int remapKeyboard::getButtonChoice(int num)
{
	if(num<0) num=activeKey();
	return (*this)[num].buttons.getChoice();
}

vector<instrument> & remapKeyboard::getActiveNotes()
{
	return getKey().notes;
}

vector<instrument> & remapKeyboard::getNotes(int num)
{
	return (*this)[num].notes;
}
