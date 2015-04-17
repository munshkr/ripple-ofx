#pragma once

#include "Repl.h"


class TidalRepl : public Repl {
    public:
        TidalRepl();
        ~TidalRepl();

        void start();

        // Start REPL process and run bootload script
        void start(const string& bootPath);

        void setGhciPath(const string &path);
        string getGhciPath() const;

        void setTidalHost(const string &host);
        string getTidalHost() const;

        void setTidalPort(unsigned int port);
        unsigned int getTidalPort() const;

    protected:
        void execProcess();

    private:
        string ghciPath;
        string tidalHost;
        unsigned int tidalPort;
};
