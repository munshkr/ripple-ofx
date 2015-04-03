#include "TidalRepl.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

const int NUM_PIPES = 3;

const int PARENT_WRITE_PIPE = 0;
const int PARENT_READ_PIPE  = 1;
const int PARENT_ERROR_PIPE = 2;

// always in a pipe[], pipe[0] is for read and pipe[1] is for write
const int READ_FD  = 0;
const int WRITE_FD = 1;

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define PARENT_ERROR_FD ( pipes[PARENT_ERROR_PIPE][READ_FD]  )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
#define CHILD_ERROR_FD  ( pipes[PARENT_ERROR_PIPE][WRITE_FD] )

// select() timeout
const int TV_SEC  = 0;
const int TV_USEC = 10000;


TidalRepl::TidalRepl() {
    // Initialize pipes
    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    // Fork process and make child execute REPL
    repl_pid = fork();
    if (repl_pid == 0) {
        // Child
        dup2(CHILD_READ_FD,  STDIN_FILENO);
        dup2(CHILD_WRITE_FD, STDOUT_FILENO);
        dup2(CHILD_ERROR_FD, STDERR_FILENO);

        // Close fds not required by child. Also, we don't want the exec'ed
        // program to know these existed.
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);
        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);
        close(PARENT_ERROR_FD);

        char* argv[] = { "/usr/bin/ghci", "-XOverloadedStrings", 0 };
        execv(argv[0], argv);
    } else {
        // Parent

        // close fds not required by parent
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);
    }
}

TidalRepl::~TidalRepl() {
    if (repl_pid) kill(repl_pid, SIGTERM);

    close(PARENT_READ_FD);
    close(PARENT_WRITE_FD);
    close(PARENT_ERROR_FD);

    cout << "\e[0m" << endl;
}

void TidalRepl::eval(string s, bool print) {
    if (print) cout << "\e[33m" << s << "\e[0m" << endl;

    s = s + '\n';
    const char* cstr = s.c_str();
    int res = write(PARENT_WRITE_FD, cstr, strlen(cstr));
    if (res == -1) perror("write");
}

void TidalRepl::read_async() {
    fd_set rfds;
    struct timeval tv;

    // Watch parent stdin to see when it has input written by the child
    FD_ZERO(&rfds);
    FD_SET(PARENT_READ_FD, &rfds);
    FD_SET(PARENT_ERROR_FD, &rfds);

    // Timeout: 10000us, review if 0 is possible and doesn't hog the cpu
    tv.tv_sec = TV_SEC;
    tv.tv_usec = TV_USEC;

    int ret = select(PARENT_ERROR_FD+1, &rfds, NULL, NULL, &tv);
    // Don't rely on the value of tv now!

    if (ret == -1) {
        perror("select()");
    } else if (ret) {
        if (FD_ISSET(PARENT_READ_FD, &rfds)) {
            // Read from child’s stdout
            int count = read(PARENT_READ_FD, buf, BUFFER_SIZE);
            if (count == -1) {
                cerr << "stdout IO error" << endl;
            } else {
                buf[count] = 0;
                cout << "\e[32m" << buf << "\e[0m";
            }
        }

        if (FD_ISSET(PARENT_ERROR_FD, &rfds)) {
            // Read from child’s stderr
            int count = read(PARENT_ERROR_FD, buf, BUFFER_SIZE);
            if (count == -1) {
                cerr << "stderr IO error" << endl;
            } else {
                buf[count] = 0;
                cout << "\e[31m" << buf << "\e[0m";
            }
        }
    } else {
        //cerr << '.' << flush;
    }
}

void TidalRepl::boot(const string& boot_path) {
    ifstream f(boot_path.c_str());

    if (f.is_open()) {
        stringstream f_buf;
        f_buf << f.rdbuf();
        eval(f_buf.str(), false);
    } else {
        cerr << "Unable to open bootstrap file at " << boot_path << endl;
    }
}
