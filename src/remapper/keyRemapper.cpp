/*
 *  keyRemapper.cpp
 *  MidiRemapper
 *
 *  Created by Exhibits on 4/5/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "keyRemapper.h"

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;
extern ofColor blue;

void remapper::setup()
{
	yDis=50;
	kb.setup(7*ofGetWidth()/8., 4);
	band.setup(parseFile("instruments.xml"));
	band.configureSize();
	pianoBackground.loadImage("images/woodPanel.jpg");
  controlBackground.loadImage("images/background.jpg");
	dinc.loadFont("fonts/DinC.ttf");
	dinc.setSize(35);
	dinc.setMode(OF_FONT_TOP);
}

void remapper::update(int num)
{
	band.configureSize();
	band.update();
}



void drawShadowsAroundRect(ofRectangle & box, double depth){
  ofShade(box.x, box.y, depth, box.width, OF_DOWN, false);
  ofShade(box.x+box.width, box.y, depth, box.height, OF_LEFT);
  ofShade(box.x, box.y+box.height, depth, box.width, OF_UP);
  ofShade(box.x, box.y, depth, box.height, OF_RIGHT, false);
}

void remapper::draw(double _x, double _y)
{
  bool band1st=false;
  
  ofPoint controlPad(20,50);
  ofPoint pad(30,30);
  
  ofRectangle controlBox(_x,_y,ofGetWidth(),band.h+controlPad.y*2);
  ofRectangle kbBox;
  ofRectangle bandBox;
  ofRectangle keyBox(controlBox.x,controlBox.y+controlBox.height,controlBox.width,kb.h+controlPad.y);
  
  if(!band1st){
    kbBox=ofRectangle(controlBox.x,controlBox.y,controlBox.width-(band.w+controlPad.x),controlBox.height);
    bandBox=ofRectangle(controlBox.x+kbBox.width,controlBox.y,band.w+40,controlBox.height);
  }
  else{
    bandBox=ofRectangle(controlBox.x,controlBox.y,band.w+40,controlBox.height);
    kbBox=ofRectangle(controlBox.x+bandBox.width,controlBox.y,controlBox.width-(bandBox.width),controlBox.height);
  }
  
  //ofRectangle & first=((band1st)?bandBox:kbBox);
  
  //_-_-_-_-_ draw the background behind the keyboard
  ofSetColor(black+255*.5);
  ofRect(keyBox);
  ofSetColor(gray.opacity(.3));
  drawHatching(keyBox.x, keyBox.y, keyBox.width, keyBox.height, 4,4);
  
	double indent=30;
	ofShadowRounded(kb.x+indent/2, kb.y+indent/2, kb.w-indent, kb.h-indent, kb.w/64., indent);
  
  //_-_-_-_-_ draw the keyboard
  kb.draw((keyBox.width-kb.w)/2, keyBox.y+controlPad.y/2);
  
  //_-_-_-_-_ draw the background for the control bar
  double controlScale=(ofGetWidth()/controlBackground.width);
  ofSetColor(255, 255, 255);
	controlBackground.draw(controlBox.x, controlBox.y+controlBox.height-controlScale*controlBackground.height,controlBox.width,controlScale*controlBackground.height);
  
  //_-_-_-_-_ draw shadows
  ofSetShadowDarkness(.5);
  drawShadowsAroundRect(kbBox, 10);
  drawShadowsAroundRect(bandBox, 10);
  
  ofShade(controlBox.x, controlBox.y+controlBox.height, 10, controlBox.width, OF_DOWN);
  ofShade(keyBox.x, keyBox.y+keyBox.height, 10, keyBox.width, OF_DOWN);
  ofShade(keyBox.x, keyBox.y+keyBox.height, 10, keyBox.width, OF_UP);
  
  //_-_-_-_-_ draw the band box contents
  band.draw(bandBox.x+controlPad.x,bandBox.y+pad.y);
	band.drawInstruments();
  
  //_-_-_-_-_ draw the keyboard info
  kb.drawKeyInfo(keyBox.x,keyBox.y+keyBox.height, keyBox.width, ofGetHeight()-(controlBox.height+keyBox.height+controlPad.y));
  
  //_-_-_-_-_ draw the keyboard controls
  kb.drawKeyboardControls(kbBox.x+pad.x, kbBox.y+controlPad.y, kbBox.width-pad.x*2,kbBox.height-controlPad.y*2);
  
  //_-_-_-_-_ draw the held block
  held.drawBackground();
  
}


bool remapper::clickDown(int _x, int _y)
{
	if(kb.clickDown(_x, _y));
	for (unsigned int i=0; i<kb.getActiveNotes().size(); i++) {
		/*if(kb.getActiveNotes()[i].over(_x,_y)){
			held=kb.getActiveNotes()[i];
			kb.getActiveNotes()[i].setDefault(true);
			held.clickDown(_x, _y);
		}*/
	}
	if(band.clickDown(_x, _y)){
		held=band.lastInstrument();
		held.clickDown(_x, _y);
	}
	kb.getKey().buttons.clickDown(_x, _y);
}

bool remapper::clickUp(int _x, int _y)
{
	kb.clickUp();
	/*if(band.clickUp(kb.getActiveNotes())){
		kb.selectButton(1);
	}*/
  if(band.clickUp()){
    kb.selectButton(1);
  }
	for (unsigned int i=0; i<kb.getActiveNotes().size(); i++) {
		kb.getActiveNotes()[i].clickUp();
	}
	if(held.isHeld()){
		bool ret=false;
		for (unsigned int i=0; i<kb.size(); i++) {
			if(kb[i].isSharp()&&!ret&&kb[i].over(_x, _y)){
				kb.getNotes(i)[0]=held;
				kb.getNotes(i)[0].setDefault(false);
				//kb.getNotes(i)[0].resizeByFont(40);
				kb.selectButton(i,1);
				ret=true;
				//kb[i].select(false);
			}
		}
		for (unsigned int i=0; i<kb.size(); i++) {
			if(!kb[i].isSharp()&&!ret&&kb[i].over(_x, _y)){
				kb.getNotes(i)[0]=held;
				kb.getNotes(i)[0].setDefault(false);
				kb.selectButton(i,1);
				//kb[i].select(false);
				ret=true;
			}
		}
		held.clickUp();
		held.clear();
	}
	//if(held.clickUp(kb.getActiveNotes()))
	//	kb.selectButton(1);
	kb.getKey().buttons.clickUp();
}

void remapper::drag(int _x, int _y)
{
	band.mouseMotion(_x, _y);
	held.mouseMotion(_x, _y);
  kb.mouseMotion(_x, _y);
	if(held.isHeld()){
		bool tmp=false;
		for (unsigned int i=0; i<kb.size(); i++) {
			if(kb[i].isSharp()&&kb[i].over(_x, _y)&&!tmp){
				tmp=true;
				kb.pressKey(i);
			}
			else kb[i].select(false);
		}
		for (unsigned int i=0; i<kb.size(); i++) {
			if(!kb[i].isSharp()&&kb[i].over(_x, _y)&&!tmp){
				tmp=true;
				kb.pressKey(i);
			}
			else if(!kb[i].over(_x, _y)) kb[i].select(false);
		}
	}
}

void remapper::resize(int _w, int _h)
{
	kb.setup(7*ofGetWidth()/8., 4);
	band.configureSize();
	band.update();
}