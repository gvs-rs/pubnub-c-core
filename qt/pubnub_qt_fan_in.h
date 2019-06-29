/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt.h"

#include <QTextStream>


class pubnub_qt_fan_in : public QObject {
    Q_OBJECT
  
public:
    pubnub_qt_fan_in() 
        : d_pb_1("demo", "demo")
        , d_pb_2("demo", "demo")
        , d_pb_3("demo", "demo")
        , d_out(stdout)
    {}

public slots:
    void execute();
    
private slots:
    void onConnect_pb3(pubnub_res result);
    void onSubscribe_pb3(pubnub_res result);
    void onPublish_pb1(pubnub_res result);
    void onPublish_pb2(pubnub_res result);
            
private:
    bool reconnect(pubnub_qt& pb, char const *from, char const *to);
    void pb3_subscribe();
    pubnub_qt d_pb_1;
    pubnub_qt d_pb_2;
    pubnub_qt d_pb_3;
    bool d_pb1_publish_done = false;
    bool d_pb2_publish_done = false;
    QTextStream d_out;
};
