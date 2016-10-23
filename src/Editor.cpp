#include "Editor.h"

Editor::Editor(Repl* repl) {
    this->repl = repl;
}

Editor::~Editor() {}

void Editor::setup() {
    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 96);
    //ofxEditor::loadFont("fonts/C64_Pro-STYLE.ttf", 48);

    // white text with gray shadow, on black background
    ofBackground(0);
    //editor.getSettings().setTextShadowColor(ofColor::gray);

    // setup color scheme via xml file
    colorScheme.loadFile("colorScheme.xml");
    editor.setColorScheme(&colorScheme);

    // setup Tidal syntax via xml file
    syntax.loadFile("tidalSyntax.xml");
    editor.getSettings().addSyntax(&syntax);
    editor.getSettings().printSyntaxes();

    editor.setAutoFocus(true);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //editor.setText(testFile.readToBuffer().getText());
    //ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();
}

void Editor::draw() {
    editor.draw();
}

void Editor::update() {}

void Editor::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
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
            case 'l':
                editor.setLineWrapping(!editor.getLineWrapping());
                return;
            case 'n':
                editor.setLineNumbers(!editor.getLineNumbers());
                return;
            case 'z':
                editor.setAutoFocus(!editor.getAutoFocus());
                return;
        }
    }

    editor.keyPressed(key);
}

void Editor::resize(int w, int h) {
    editor.resize(w, h);
}

void Editor::executeScript() {
    bool selection = editor.isSelection();
    if (selection) {
        editor.flashSelection();
        const string s = editor.getText();
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
    unsigned int pos = editor.getCurrentPos();
    const string text = editor.getText();

    size_t start = text.rfind("\n\n", pos);
    if (start == string::npos) start = 0;

    size_t end = text.find("\n\n", pos);
    if (end == string::npos) end = text.size();

    size_t len = end - start;
    start = start == 0 ? start : (start + 2);

    string subs = text.substr(start, len);
    ofLog() << "substring: '" << subs << "'";

    editor.flashText(start, end);

    return subs;
}
