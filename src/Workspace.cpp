#include "Workspace.h"

Workspace::Workspace() {
    Editor* ed;

    // create log viewers for each REPL
    this->replLog = new ReplLog(&repl);
    //this->screplLog = new ReplLog(&screpl);

    // create Tidal editor
    ed = new Editor(&repl);
    this->editors.push_back(ed);
    ed->setup();
    repl.start("data/boot.hss");

    // create SC editor
    ed = new Editor(&repl);
    this->editors.push_back(ed);
    ed->setup();
    repl.start();

    this->currentEditor = 0;
    this->showReplBuffer = true;
}

Workspace::~Workspace() {
    this->editors.clear();
    delete this->replLog;
    //delete this->screplLog;
}

void Workspace::draw() {
    if (showReplBuffer) {
        if (editors[currentEditor]->repl == &repl) {
            replLog->draw();
        } else {
            screplLog->draw();
        }
    }

    editors[currentEditor]->draw();
}

void Workspace::update() {
    editors[currentEditor]->update();
    editors[currentEditor]->repl->readAsync();
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
            case 'o':
                setReplBuffer(!getReplBuffer());
                return;
        }
    }

    editors[currentEditor]->keyPressed(key);
}

void Workspace::resize(int w, int h) {
    editors[currentEditor]->resize(w, h);
}

void Workspace::setReplBuffer(bool value) {
    showReplBuffer = value;
}

bool Workspace::getReplBuffer() const {
    return showReplBuffer;
}
