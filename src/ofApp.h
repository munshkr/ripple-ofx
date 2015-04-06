#pragma once

#include "ofMain.h"
#include "ofxEditor.h"
#include "TidalRepl.h"

class ofApp : public ofBaseApp {
    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void windowResized(int w, int h);

        ofxEditor editor;
        ofxEditorColorScheme colorScheme;
        ofxEditorSyntax syntax;
        bool debug; //< show grid and fps?

    private:
        void setTidalSyntax(ofxEditorColorScheme &scheme);
        string getParagraph();
        void executeScript();

        TidalRepl repl;
};
