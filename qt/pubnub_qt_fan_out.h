/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt.h"

#include <QTextStream>

class pubnub_qt_fan_out : public QObject {
    Q_OBJECT
  
public:
    pubnub_qt_fan_out() 
        : d_pb_pub("demo", "demo")
        , d_pb_sub1("demo", "demo")
        , d_pb_sub2("demo", "demo")
        , d_out(stdout)
    {}

public slots:
    void execute();
    
private slots:
    void onSubscribe_1(pubnub_res result);
    void onSubscribe_2(pubnub_res result);
    void onPublish(pubnub_res result);

private:
    void publish_if_ready();
    void subscribe_ctx(pubnub_qt& pb);
    void print_messages(pubnub_qt& pb);
    void exit_when_done();
    pubnub_qt d_pb_pub;
    pubnub_qt d_pb_sub1;
    pubnub_qt d_pb_sub2;
    bool d_publish_done = false;
    bool d_connect_1_done = false;
    bool d_connect_2_done = false;
    bool d_subscribe_1_done = false;
    bool d_subscribe_2_done = false;
    QTextStream d_out;
};


