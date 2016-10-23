#pragma once

#include "ofxEditor.h"
#include "Repl.h"

class Editor {
    public:
        Editor(Repl* repl);
        virtual ~Editor();

        void setup();
        void update();
        void draw();
        void keyPressed(int key);

        /// reshape the drawing area
        /// call this if you change the window size (fullscreen, etc)
        void resize(int width, int height);

        // REPL evaluation
        string getParagraph();
        void executeScript();

        Repl* repl;

    private:
        ofxEditor editor;
        ofxEditorColorScheme colorScheme;
        ofxEditorSyntax syntax;
};
