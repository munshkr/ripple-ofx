#pragma once

#include "ofMain.h"
#include "Workspace.h"
#include "SubscribeThread.h"

class ofApp : public ofBaseApp {
    public:
        void setup();
        void update();
        void draw();
        void keyPressed(int key);
        void windowResized(int w, int h);
        void quit();

        bool debug; //< show grid and fps?

    private:
        Workspace workspace;

        SubscribeThread subThread;
};
