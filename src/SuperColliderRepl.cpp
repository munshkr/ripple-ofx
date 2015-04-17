#include "SuperColliderRepl.h"

#include <fstream>
#include <sstream>

#include <ofLog.h>

const string DEFAULT_SCLANG_PATH = "/usr/bin/sclang";


SuperColliderRepl::SuperColliderRepl() {
    sclangPath = DEFAULT_SCLANG_PATH;
}

SuperColliderRepl::~SuperColliderRepl() {}

void SuperColliderRepl::start() {
    Repl::start();
}

void SuperColliderRepl::start(const string& bootPath) {
    start();

    ifstream f(bootPath.c_str());

    if (f.is_open()) {
        stringstream f_buf;
        f_buf << f.rdbuf();
        eval(f_buf.str());
    } else {
        ofLogWarning() << "SuperColliderRepl: Unable to open bootstrap file at " << bootPath;
    }
}

void SuperColliderRepl::setSclangPath(const string &path) {
    sclangPath = path;
}

string SuperColliderRepl::getSclangPath() const {
    return sclangPath;
}

void SuperColliderRepl::execProcess() {
    char* argv[] = {
        const_cast<char*>(sclangPath.c_str()),
        0
    };
    execv(argv[0], argv);
}
