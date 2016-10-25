class SubscribeThread : public ofThread {
    void threadedFunction() {
        while (isThreadRunning()) {
            ofLog() << "[thread] sleeping 1 second";
            sleep(1000);
        }
    }
};
