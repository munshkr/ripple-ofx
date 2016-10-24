#include "TidalRepl.h"

#include <fstream>
#include <sstream>

#include <ofLog.h>

const string DEFAULT_GHCI_PATH = "/usr/bin/ghci";

const string DEFAULT_TIDAL_HOST = "127.0.0.1";
const unsigned int DEFAULT_TIDAL_PORT = 9160;


TidalRepl::TidalRepl() {
    ghciPath = DEFAULT_GHCI_PATH;
    tidalHost = DEFAULT_TIDAL_HOST;
    tidalPort = DEFAULT_TIDAL_PORT;
}

TidalRepl::~TidalRepl() {}

void TidalRepl::start() {
    Repl::start();
}

void TidalRepl::start(const string& bootPath) {
    start();

    ifstream f(bootPath.c_str());

    if (f.is_open()) {
        stringstream f_buf;
        f_buf << f.rdbuf();
        eval(f_buf.str());
    } else {
        ofLogWarning() << "TidalRepl: Unable to open bootstrap file at " << bootPath;
    }
}

void TidalRepl::evalMulti(string s) {
    eval(":{\n" + s + "\n:}");
}

void TidalRepl::setGhciPath(const string &path) {
    ghciPath = path;
}

string TidalRepl::getGhciPath() const {
    return ghciPath;
}

void TidalRepl::setTidalHost(const string &host) {
    tidalHost = host;
}

string TidalRepl::getTidalHost() const {
    return tidalHost;
}

void TidalRepl::setTidalPort(unsigned int port) {
    tidalPort = port;
}

unsigned int TidalRepl::getTidalPort() const {
    return tidalPort;
}

void TidalRepl::execProcess() {
    const string ghciArg = "-XOverloadedStrings";

    char* argv[] = {
        const_cast<char*>(ghciPath.c_str()),
        const_cast<char*>(ghciArg.c_str()),
        0
    };

    ostringstream port;
    port << tidalPort;

    setenv("TIDAL_TEMPO_IP", tidalHost.c_str(), 1);
    setenv("TIDAL_TEMPO_PORT", port.str().c_str(), 1);

    int ret = execv(argv[0], argv);

    if (ret) {
      ofLogError() << "failed to execv " << argv[0] << " " << argv[1];
    }
}
