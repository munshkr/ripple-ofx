#include "zmq.hpp"

class SubscribeThread : public ofThread {
    public:

    void setEditor(Editor* editor) {
        lock();
        this->editor = editor;
        unlock();
    }

    Editor* getEditor() {
        return editor;
    }

    private:

    void threadedFunction() {
        zmq::context_t ctx(1);

        zmq::socket_t subscriber(ctx, ZMQ_SUB);

        subscriber.connect("tcp://localhost:9000");
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "", strlen(""));

        ofLog() << "[subThread] started listening";

        while (isThreadRunning()) {
            zmq::message_t msg;
            subscriber.recv(&msg);

            string message = string(static_cast<char*>(msg.data()), msg.size());

            ofLog() << "received message: '" << message << "'" << endl;

            lock();
            this->editor->setText(message);
            unlock();
        }

        subscriber.close();
        ctx.close();
    }

    Editor* editor;
};
