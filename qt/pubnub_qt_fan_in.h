/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt.h"

#include <QTextStream>


class pubnub_qt_fan_in : public QObject {
    Q_OBJECT
  
public:
    pubnub_qt_fan_in() 
        : d_pb_pub1("demo", "demo")
        , d_pb_pub2("demo", "demo")
        , d_pb_sub("demo", "demo")
        , d_out(stdout)
    {}

public slots:
    void execute();
    
private slots:
    void onConnect(pubnub_res result);
    void onSubscribe(pubnub_res result);
    void onPublish_1(pubnub_res result);
    void onPublish_2(pubnub_res result);
            
private:
    bool reconnect(pubnub_qt& pb, char const *from, char const *to);
    void subscribe();
    pubnub_qt d_pb_pub1;
    pubnub_qt d_pb_pub2;
    pubnub_qt d_pb_sub;
    bool d_publish_1_done = false;
    bool d_publish_2_done = false;
    QTextStream d_out;
};
