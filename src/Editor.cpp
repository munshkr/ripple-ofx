#include "Editor.h"

Editor::Editor() {
    viewportX = 0;
    viewportY = 0;

    ofxEditor();

    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 96);
    //ofxEditor::loadFont("fonts/C64_Pro-STYLE.ttf", 48);

    // white text with gray shadow, on black background
    ofBackground(0);
    //getSettings().setTextShadowColor(ofColor::gray);

    // setup color scheme via xml file
    colorScheme.loadFile("colorScheme.xml");
    setColorScheme(&colorScheme);

    // setup Tidal syntax via xml file
    syntax.loadFile("tidalSyntax.xml");
    getSettings().addSyntax(&syntax);
    getSettings().printSyntaxes();

    setAutoFocus(true);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //setText(testFile.readToBuffer().getText());
    //ofLogNotice() << "num chars: " << getNumCharacters() << " num lines: " << getNumLines();
}

Editor::~Editor() {}

void Editor::setRepl(Repl* repl) {
    this->repl = repl;
}

Repl* Editor::getRepl() {
    return repl;
}

void Editor::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case 's':
                if (getColorScheme()) {
                    clearColorScheme();
                } else {
                    setColorScheme(&colorScheme);
                }
                return;
            case 'e':
                executeScript();
                return;
            case 'l':
                setLineWrapping(!getLineWrapping());
                return;
            case 'n':
                setLineNumbers(!getLineNumbers());
                return;
            case 'z':
                setAutoFocus(!getAutoFocus());
                return;
        }
    }

    ofxEditor::keyPressed(key);
}

void Editor::draw() {
    // Split screen: scale to half screen and translate to viewportX

    //ofPushStyle();
    //ofPushView();
    //ofPushMatrix();

    //ofTranslate(viewportX, 0);
    //ofScale(0.5, 1);

    ofxEditor::draw();

    //ofPopMatrix();
    //ofPopView();
    //ofPopStyle();
}

void Editor::executeScript() {
    bool selection = isSelection();
    if (selection) {
        flashSelection();
        const string s = getText();
        repl->eval(s);
    } else {
        const string s = getParagraph();
        if (repl->isRunning()) {
            ofLog() << "its running ok";
        }
        repl->evalMulti(s);
    }
}

string Editor::getParagraph() {
    unsigned int pos = getCurrentPos();
    const string text = getText();

    size_t start = text.rfind("\n\n", pos);
    if (start == string::npos) start = 0;

    size_t end = text.find("\n\n", pos);
    if (end == string::npos) end = text.size();

    size_t len = end - start;
    start = start == 0 ? start : (start + 2);

    string subs = text.substr(start, len);
    ofLog() << "substring: '" << subs << "'";

    flashText(start, end);

    return subs;
}

void Editor::setViewportX(float x) {
    viewportX = x;
}

float Editor::getViewportX() {
    return viewportX;
}

void Editor::setViewportY(float y) {
    viewportY = y;
}

float Editor::getViewportY() {
    return viewportY;
}
