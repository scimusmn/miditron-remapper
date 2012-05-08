#include "testApp.h"
#include "midiConfig.h"

#define RECONNECT_TIME 400

extern ofColor white;
extern ofColor black;
extern ofColor yellow;
extern ofColor gray;
extern ofColor blue;

//--------------------------------------------------------------
void testApp::setup(){
	//Load the config file
	cfg().setup();
	
	//Attach to the MIDI devices
	keyboard.openPort(cfg().midiInName);
	midiOut.openPort(cfg().midiOutName);

	//setup the remapper. The remapper handles all of the button presses, etc.
	rmp.setup();
	
	//Load the title font for the top of the screen
	title.loadFont("fonts/Din.ttf", 35);
}

//--------------------------------------------------------------
void testApp::update(){
	rmp.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	//Draw the background hatching
	ofBackground(0x33, 0x33, 0x33);
	ofSetColor(black);
	drawHatching(0,0,ofGetWidth(),ofGetHeight(), 25,1);
	
	//Start drawing the remapper at the bottom of the title.
	rmp.draw(0, title.h);

	//Draw whatever the title is at the top of the screen.
	title.draw(cfg().progTitle, 0, 0);
  
  
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	rmp.motion(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	rmp.drag(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	rmp.clickDown(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	rmp.clickUp(x, y);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	rmp.resize(w, h);
}

void testApp::midiToSend(vector< unsigned char > message){
//if there is a midi message to send, send it on the midi out.
  midiOut.sendMessage(message);
}

//
void testApp::midiReceived(double deltatime, std::vector< unsigned char > *message, int port){
	if(message->size()>=3){  // if the message is three bytes long
		if ((message->at(0)==MIDI_NOTE_ON&&port==keyboard.getPort())||1) {				//if it is a play note command
			if(message->at(1)>=MIDI_KEYBOARD_START&&message->at(1)<MIDI_KEYBOARD_END){	// if the note which is being played fits on our keyboard
				rmp.kb[message->at(1)-MIDI_KEYBOARD_START].setPressed(message->at(2));	// press the corresponding key on the virtual keyboard
				if(!rmp.kb.getButtonChoice(message->at(1)-MIDI_KEYBOARD_START)){		// if the corresponding choice of key is not mapped, 
					vector<unsigned char> msg;
					msg.push_back(MIDI_NOTE_ON+rmp.kb.getChannel());
					msg.push_back(message->at(1));										//forward the message, but change the channel to the default keyboard channel.
					msg.push_back(message->at(2));
					midiToSend(msg);
				}
				else {																	//else, if the key is mapped,
					vector<instrument> & tmp=rmp.kb.getNotes(message->at(1)-MIDI_KEYBOARD_START);  //pick the note vector from the key,
					for (unsigned int i=0; i<tmp.size(); i++) {
						if(!tmp[i].isDefault()){										// play the note on the wall, if a note is selected.
							tmp[i].play(message->at(2));
						}
					}
				}
			}
		}
	}
}