#pragma once

#include "Editor.h"
#include "TidalRepl.h"
#include "SuperColliderRepl.h"

class Workspace {
    public:
        Workspace();
        virtual ~Workspace();

        void draw();
        void update();
        void keyPressed(int key);
        void resize(int w, int h);

    private:
        TidalRepl repl;
        SuperColliderRepl screpl;

        vector<Editor*> editors;
        unsigned int currentEditor;
};
