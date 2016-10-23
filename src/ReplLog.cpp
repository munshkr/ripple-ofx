#include "ReplLog.h"

const unsigned int OUTPUT_FONT_SIZE = 12;

ReplLog::ReplLog(Repl* repl) {
    this->repl = repl;
    this->bufferSize = 0;
    repl->setListener(this);
}

ReplLog::~ReplLog() {};

void ReplLog::draw() {
    list< pair<Repl::EventType, string> >::const_iterator it = buffer.begin();
    for (int y = 0; it != buffer.end(); it++, y += OUTPUT_FONT_SIZE) {
        const Repl::EventType type = it->first;
        const string &line = it->second;

        switch (type) {
          case Repl::INPUT:
            ofSetColor(0, 64, 0);
            break;
          case Repl::OUTPUT:
            ofSetColor(64);
            break;
          case Repl::ERROR:
            ofSetColor(127, 0, 0);
            break;
        }

        ofDrawBitmapString(line, 0, y);
    }
}

void ReplLog::inputLineEvent(const string& line) {
    appendReplBuffer(line, Repl::INPUT);
}

void ReplLog::outputLineEvent(const string& line) {
    appendReplBuffer(line, Repl::OUTPUT);
}

void ReplLog::errorLineEvent(const string& line) {
    appendReplBuffer(line, Repl::ERROR);
}

void ReplLog::appendReplBuffer(const string& line, const Repl::EventType type) {
    if (bufferSize > ofGetHeight() / OUTPUT_FONT_SIZE) {
        buffer.pop_front();
    } else {
        bufferSize++;
    }
    buffer.push_back(make_pair(type, line));
}

