#pragma once

#include "ofMain.h"
#include "ofxEditor.h"

class ofApp : public ofBaseApp {
    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void windowResized(int w, int h);

        ofxEditor editor;
        ofxEditorColorScheme colorScheme;
        bool debug; //< show grid and fps?

    private:
        void setTidalSyntax(ofxEditorColorScheme &scheme);
        string getParagraph();

        void initRepl();
        void execute();
        void eval(string s);
        void read_async();

        char buf[4096];
};
