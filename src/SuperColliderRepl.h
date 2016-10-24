#pragma once

#include "Repl.h"


class SuperColliderRepl : public Repl {
    public:
        SuperColliderRepl();
        ~SuperColliderRepl();

        void start();

        // Start REPL process and run bootload script
        void start(const string& bootPath);

        // Evaluate a string and emit input event
        void eval(string s);

        void setSclangPath(const string &path);
        string getSclangPath() const;

    protected:
        void execProcess();

    private:
        string sclangPath;
};
