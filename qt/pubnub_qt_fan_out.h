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
    pubnub_qt d_pb_1, d_pb_2, d_pb_3;
    QTextStream d_out;
};
