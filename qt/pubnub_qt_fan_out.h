/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt.h"

#include <QTextStream>

class pubnub_qt_fan_out : public QObject {
    Q_OBJECT
  
public:
    pubnub_qt_fan_out() 
        : d_pb_1("demo", "demo")
        , d_pb_2("demo", "demo")
        , d_pb_3("demo", "demo")
        , d_out(stdout)
    {}

public slots:
    void execute();
    
private slots:
    void onSubscribe_pb3(pubnub_res result);
    void onSubscribe_pb2(pubnub_res result);
    void onPublish_pb1(pubnub_res result);

private:
    void pb1_publish_if_ready();
    void subscribe_ctx(pubnub_qt& pb);
    void print_messages(pubnub_qt& pb);
    void exit_when_done();
    pubnub_qt d_pb_1;
    pubnub_qt d_pb_2;
    pubnub_qt d_pb_3;
    bool d_pb1_publish_done = false;
    bool d_pb2_connect_done = false;
    bool d_pb3_connect_done = false;
    bool d_pb2_subscribe_done = false;
    bool d_pb3_subscribe_done = false;
    QTextStream d_out;
};


