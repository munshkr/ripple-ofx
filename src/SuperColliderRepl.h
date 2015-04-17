#pragma once

#include "Repl.h"


class SuperColliderRepl : public Repl {
    public:
        SuperColliderRepl();
        ~SuperColliderRepl();

        void start();

        // Start REPL process and run bootload script
        void start(const string& bootPath);

        void setSclangPath(const string &path);
        string getSclangPath() const;

    protected:
        void execProcess();

    private:
        string sclangPath;
};
