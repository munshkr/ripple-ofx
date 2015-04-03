#include "ofApp.h"

#define NUM_PIPES 3

#define PARENT_WRITE_PIPE   0
#define PARENT_READ_PIPE    1
#define PARENT_ERROR_PIPE   2

int pipes[NUM_PIPES][2];

// always in a pipe[], pipe[0] is for read and pipe[1] is for write
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define PARENT_ERROR_FD ( pipes[PARENT_ERROR_PIPE][READ_FD]  )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
#define CHILD_ERROR_FD  ( pipes[PARENT_ERROR_PIPE][WRITE_FD] )

#define BUFFER_SIZE 4096


void ofApp::setup() {
    initRepl();

    ofSetVerticalSync(true);

    // handle ESC internally since we use it to exit selection
    ofSetEscapeQuitsApp(false);

    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    //ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 48);
    ofxEditor::loadFont("fonts/C64_Pro-STYLE.ttf", 48);

    // open a file by default
    //ofFile testFile;
    //testFile.open("hi.tidal", ofFile::ReadOnly);
    //editor.setText(testFile.readToBuffer().getText());

    ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    // white text with gray shadow, on black background
    ofBackground(0);
    //editor.getSettings().setTextShadowColor(ofColor::gray);

    // enable syntax highlighting by default
    setTidalSyntax(colorScheme);
    editor.setColorScheme(&colorScheme);

    editor.setAutoFocus(true);

    debug = false;
}

void ofApp::draw() {
    editor.draw();

    if (debug) {
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString((int) ofGetFrameRate()), ofGetWidth() - 70, ofGetHeight() - 10);
    }
}

void ofApp::update() {
    // read and print everything that ghci says
    read_async();
}

void ofApp::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case 'q':
                ofExit();
                return;
            case 's':
                if (editor.getColorScheme()) {
                    editor.clearColorScheme();
                } else {
                    editor.setColorScheme(&colorScheme);
                }
                return;
            case 'e':
                execute();
                return;
            case 'd':
                debug = !debug;
                return;
            case 'f':
                ofToggleFullscreen();
                return;
            case 'l':
                editor.setLineWrapping(!editor.getLineWrapping());
                return;
            case 'n':
                editor.setLineNumbers(!editor.getLineNumbers());
                return;
            case 'z':
                editor.setAutoFocus(!editor.getAutoFocus());
                return;
            case 'w':
                ofxEditor::setTextShadow(!ofxEditor::getTextShadow());
                return;
            case '1':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(1, 5);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
            case '2':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(5, 2);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
        }
    }
    editor.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
    editor.resize(w, h);
}

void ofApp::setTidalSyntax(ofxEditorColorScheme &scheme) {
    for (int i = 1; i < 10; i++) {
        scheme.setWordColor("d" + ofToString(i), ofColor::orange);
    }
    scheme.setWordColor("$", ofColor::green);
    scheme.setWordColor("|+|", ofColor::orangeRed);
    scheme.setWordColor("<$>", ofColor::orangeRed);
    scheme.setWordColor("<*>", ofColor::orangeRed);

    scheme.setSingleLineComment("--");
    scheme.setMultiLineComment("{-", "-}");

    scheme.setStringColor(ofColor::yellow);
    scheme.setNumberColor(ofColor::orangeRed);
    scheme.setCommentColor(ofColor::gray);
}

void ofApp::execute() {
    bool selection = editor.isSelection();
    if (selection) {
        eval(editor.getText());
    } else {
        eval(getParagraph());
    }
}

string ofApp::getParagraph() {
    cout << "not done yet" << endl;
    return "";
}

void ofApp::eval(string s) {
    cout << "\e[33m" << s << "\e[0m" << endl;
    s = s + '\n';
    const char* cstr = s.c_str();
    int res = write(PARENT_WRITE_FD, cstr, strlen(cstr));
    if (res == -1) {
        perror("write");
    }
}

void ofApp::read_async() {
    fd_set rfds;
    struct timeval tv;

    // Watch parent stdin to see when it has input written by the child
    FD_ZERO(&rfds);
    FD_SET(PARENT_READ_FD, &rfds);
    FD_SET(PARENT_ERROR_FD, &rfds);

    // Timeout: 10000us, review if 0 is possible and doesn't hog the cpu
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

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

void ofApp::initRepl() {
    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            ofExit();
        }
    }

    int child_pid = fork();
    if (child_pid == 0) {
        // Child
        char * argv[] = { (char*) "/usr/bin/ghci", (char*) "-XOverloadedStrings", 0 };

        dup2(CHILD_READ_FD, STDIN_FILENO);
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

        execv(argv[0], argv);
    } else {
        // Parent

        // close fds not required by parent
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_ERROR_FD);

        cout << "Loading Tidal..." << endl;
        eval(":set prompt \"\"\n" \
             ":module Sound.Tidal.Context\n" \
             "let newStream = stream \"127.0.0.1\" 7771 dirt\n" \
             "d1 <- newStream\n" \
             "(cps, getNow) <- bpsUtils\n" \
             "let bps x = cps (x/2)\n");
    }
}
