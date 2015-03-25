#include "ofApp.h"

void ofApp::setup() {

    ofSetVerticalSync(true);

    // handle ESC internally since we use it to exit selection
    ofSetEscapeQuitsApp(false);

    // make sure to load editor font before anything else!
    ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);

    // sample lua syntax
    colorScheme.setWordColor("function", ofColor::fuchsia);
    colorScheme.setWordColor("end", ofColor::fuchsia);
    colorScheme.setSingleLineComment("--");
    colorScheme.setMultiLineComment("--[[", "]]");

    // syntax highlighter colors
    colorScheme.setStringColor(ofColor::yellow);
    colorScheme.setNumberColor(ofColor::orangeRed);
    colorScheme.setCommentColor(ofColor::gray);

    // open test file
    ofFile testFile;
    testFile.open("test.txt", ofFile::ReadOnly);
    editor.setText(testFile.readToBuffer().getText());
    ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    // default: white text on black background
    ofBackground(0);

    // or black text on white background
    //ofBackground(255);
    //editor.getSettings().setTextColor(ofColor::black);
    //editor.getSettings().setTextShadowColor(ofColor::gray);

    debug = false;
}

void ofApp::draw() {

    editor.draw();

    if(debug) {
        //editor.drawGrid();

        ofSetColor(255);
        ofDrawBitmapString("fps: "+ofToString((int)ofGetFrameRate()), ofGetWidth()-70, ofGetHeight()-10);
    }
}

void ofApp::keyPressed(int key) {

    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);
    if(modifierPressed) {
        switch(key) {
            case 's':
                if(editor.getColorScheme()) {
                    editor.clearColorScheme();
                }
                else {
                    editor.setColorScheme(&colorScheme);
                }
                return;
            case 'd':
                debug = !debug;
                return;
            case 'f':
                ofToggleFullscreen();
                return;
            case 'l':
                editor.setLineWrapping(!editor.getLineWrapping());
                return;
            case 'n':
                editor.setLineNumbers(!editor.getLineNumbers());
                return;
            case 'z':
                editor.setAutoFocus(!editor.getAutoFocus());
                return;
            case 'w':
                ofxEditor::setTextShadow(!ofxEditor::getTextShadow());
                return;
            case '1':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(1, 5);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
            case '2':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(5, 2);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
        }
    }
    editor.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
    editor.resize(w, h);
}
