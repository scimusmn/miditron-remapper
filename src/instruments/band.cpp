/*
 *  band.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "band.h"

bandBar::~bandBar()
{
	
}

void bandBar::setup()
{
	
}

void bandBar::setHeight(double hgt)
{
	h=ofGetHeight();
	yoff=30;
	yBot=95;
	viewSize=h-(yoff+yBot);
	xoff=30;
	for (unsigned int i=0; i<instruments.size(); i++) {
		w=max(w,instruments[i].w+xoff*2);
	}
	double fullSize=50*instruments.size();
	bar.setup(20, viewSize, OF_VERT);
	bar.registerArea(viewSize,fullSize);
}

void bandBar::setup(xmlParse * config)
{
	clearBut.setTitle("clear blocks");
	clearBut.setTextSize(24);
	clearBut.setAvailable(true);
	sideBarBack.loadImage("images/sidebar.jpg");
	string font=config->prop;
	for (unsigned int i=0; i<config->size(); i++) {
		xmlParse xml=config->child[i];
		if (xml.label=="instrument") {
			string col=xml.prop;
			string title=xml.name;
			cout << title << " current instrument" << endl;
			long color=strtol(col.c_str(),NULL,0);
			int curInst=instruments.size();
			unsigned char note, channel;
			bool repeat=false;
			double delay=0;
			map<string,int> list;
			list["note"]=0;
			list["channel"]=1;
			list["repeat"]=2;
			list["delay"]=3;
			list["dropdown"]=4;
			cout << xml.size() << " number of children" << endl;
			for(int j=0;j<xml.size();j++){
				string * node=xml.getSubnode(j);
				switch (list.find(node[0])->second) {
					case 0:
						note=ofToInt(node[1]);
						break;
					case 1:
						channel=ofToInt(node[1]);
						break;
					case 2:
						repeat=true;
						break;
					case 3:
						delay=ofToFloat(node[1]);
						break;
					case 8:
						//ddGroup.push_back(ofDropDown(&xml->child[i]));
						break;
					default:
						break;
				}
			}
			instruments.push_back( instrument(title,channel,note,repeat));
			instruments[curInst].setColor(color);
			instruments[curInst].setDelay(delay);
		}
	}
	setHeight();
}

void bandBar::addInstrument(string title, unsigned char channel, unsigned char nt, bool repeat)
{
	instruments.push_back( instrument(title,channel,nt,repeat));
	setHeight();
}

void bandBar::drawBackground()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(i%2) ofSetColor(0x80,0x63,0x3B,128);
		else ofSetColor(0xA0,0x83,0x5B,128);
		ofRect(instruments[i].x, instruments[i].y-3+instruments[i].yoff, ofGetWidth(), instruments[i].h+6);
	}
}

void bandBar::drawInstruments()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].drawBackground();
	}
}

void bandBar::draw(int _x, int _y)
{
	x=_x,y=_y;
	
	//sidebar background
	ofSetColor(255, 255, 255);
	sideBarBack.draw(x+w-(sideBarBack.height/h)*sideBarBack.width, y,(sideBarBack.height/h)*sideBarBack.width,h);
	
	//
	ofSetColor(0x80633B);
	ofRect(x+xoff-10, y+yoff, w-xoff-10, viewSize);
	ofShade(x+w, y+yoff, 10, viewSize, OF_LEFT, .3);
	
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].draw(x+xoff,y+yoff+5 +50*(i));
		double tmpY=y+instruments[i].y+instruments[i].h+instruments[i].yoff+7.5;
		ofShade(x, tmpY, 3, w-20, OF_UP, .3);
		ofShade(x, tmpY, 3, w-20, OF_DOWN, .3,false);
	}
	
	ofShade(x+w-20, y+yoff, 10, viewSize, OF_LEFT, .3);
	
	//Box at top of sidebar
	ofSetColor(230, 230, 230);
	ofShadeBox(x, y, w, yoff, OF_DOWN, .4);
	ofShade(x, y+yoff, 10, w, OF_DOWN, .3);
	
	//box at bottom of sidebar
	ofRoundBox(x-10, getBottomPos(), w+10, ofGetHeight()-(y+yoff+viewSize), 10, .4);
	ofShade(x, getBottomPos(), 10, w, OF_UP, .3);
	ofShade(x, getBottomPos(), 15, w, OF_DOWN, .2);
	
	bar.draw(x,yoff);
	
	//Shades over ends of the scroll bar
	ofShade(x, y+yoff, 5, bar.w+4, OF_DOWN, .3);
	ofShade(x, y+yoff+viewSize, 5, bar.w+4, OF_UP, .3);
	
	//shade to right of scrollbar
	ofShade(x+bar.w+2, y+yoff, 5, viewSize, OF_RIGHT, .3);
	
	clearBut.draw((w-clearBut.w)/2, ofGetHeight()-((yBot)*2-clearBut.h)/2);
}

bool bandBar::clickDown(int _x, int _y)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(!bHolding&&_y>yoff&&_y<yoff+viewSize&&instruments[i].clickDown(_x,_y)){
			bHolding=true;
		}
	}
	if(clearBut.clickDown(_x, _y))
		clear();
	bar.clickDown(_x, _y);
}

bool bandBar::clickUp()
{
	bool ret=false;
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(instruments[i].clickUp()){
			ret=true;
			lastInst=i;
		}
	}
	bHolding=false;
	clearBut.clickUp();
	bar.clickUp();
	return ret;
}

void bandBar::update(int xDisp, int yDisp)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].update(xDisp,yDisp);
	}
	bar.update();
}

void bandBar::update()
{
	bar.update();
}

void bandBar::update(int disp, ofDirection dir)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].update(disp,dir);
	}
	update();
}

void bandBar::mouseMotion(int _x, int _y)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].mouseMotion(_x,_y);
	}
	if(bar.mouseMotion(_x,_y)){
		for (unsigned int i=0; i<instruments.size(); i++) {
			instruments[i].update(-bar.getScrollPosition(),OF_VERT);
		}
	}
}

double bandBar::farthestPoint()
{
	double ret=0;
	for (unsigned int i=0; i<instruments.size(); i++) {
		for (unsigned int j=0; j<instruments[i].size(); j++) {
			ret=max(ret,instruments[i][j].x+instruments[i][j].w);
		}
	}
	return ret;
}

void bandBar::clear()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].clear();
	}
}

void bandBar::drag(int _x, int _y)
{
	
}

void bandBar::stopAll()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i].stop();
	}
}

void bandBar::checkActives(double xPos)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(instruments[i].active(xPos))
			instruments[i].play();
		else instruments[i].stop();
	}
}

dragBlock & bandBar::lastBlock()
{
	return instruments[lastInst][instruments[lastInst].lastBlock];
}