#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);

    // handle ESC internally since we use it to exit selection
    ofSetEscapeQuitsApp(false);

    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    //ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 48);
    ofxEditor::loadFont("fonts/C64_Pro-STYLE.ttf", 48);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //editor.setText(testFile.readToBuffer().getText());

    ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    // white text with gray shadow, on black background
    ofBackground(0);
    //editor.getSettings().setTextShadowColor(ofColor::gray);

    // enable syntax highlighting by default
    setTidalSyntax(colorScheme);
    editor.setColorScheme(&colorScheme);

    editor.setAutoFocus(true);

    debug = false;
}

void ofApp::draw() {
    editor.draw();

    if (debug) {
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString((int) ofGetFrameRate()), ofGetWidth() - 70, ofGetHeight() - 10);
    }
}

void ofApp::update() {
    repl.read_async();
}

void ofApp::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case 'q':
                ofExit();
                return;
            case 's':
                if (editor.getColorScheme()) {
                    editor.clearColorScheme();
                } else {
                    editor.setColorScheme(&colorScheme);
                }
                return;
            case 'e':
                executeScript();
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

void ofApp::setTidalSyntax(ofxEditorColorScheme &scheme) {
    for (int i = 1; i < 10; i++) {
        scheme.setWordColor("d" + ofToString(i), ofColor::orange);
    }
    scheme.setWordColor("$", ofColor::green);
    scheme.setWordColor("|+|", ofColor::orangeRed);
    scheme.setWordColor("<$>", ofColor::orangeRed);
    scheme.setWordColor("<*>", ofColor::orangeRed);

    scheme.setSingleLineComment("--");
    scheme.setMultiLineComment("{-", "-}");

    scheme.setStringColor(ofColor::yellow);
    scheme.setNumberColor(ofColor::orangeRed);
    scheme.setCommentColor(ofColor::gray);
}

void ofApp::executeScript() {
    bool selection = editor.isSelection();
    if (selection) {
        repl.eval(editor.getText());
    } else {
        cerr << "not done yet" << endl;
        //repl.eval(getParagraph());
    }
}

string ofApp::getParagraph() {
    // TODO
    return "";
}
