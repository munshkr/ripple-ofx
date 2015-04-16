#pragma once

#include "ofMain.h"
#include "Editor.h"

class ofApp : public ofBaseApp {
    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void windowResized(int w, int h);

        bool debug; //< show grid and fps?

    private:
        Editor editor;
};
