#include "Workspace.h"

Workspace::Workspace() {
    Editor* ed;

    currentEditor = 0;

    // create Tidal editor
    ed = new Editor(&repl);
    ed->setup();
    editors.push_back(ed);

    // create SC editor
    ed = new Editor(&screpl);
    ed->setup();
    editors.push_back(ed);
}

Workspace::~Workspace() {
    editors.clear();
}

void Workspace::draw() {
    editors[currentEditor]->draw();
}

void Workspace::update() {
    editors[currentEditor]->update();
}

void Workspace::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case '1':
                currentEditor = 0;
                return;
            case '2':
                currentEditor = 1;
                return;
        }
    }

    editors[currentEditor]->keyPressed(key);
}

void Workspace::resize(int w, int h) {
    editors[currentEditor]->resize(w, h);
}
