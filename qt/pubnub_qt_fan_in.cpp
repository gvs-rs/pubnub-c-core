/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt_fan_in.h"

extern "C" {
#include "core/pubnub_helper.h"
}

#include <QCoreApplication>
#include <QTimer>

const QString chann = "hello_world";


bool pubnub_qt_fan_in::reconnect(pubnub_qt& pb, char const *from, char const *to)
{
    disconnect(&pb, SIGNAL(outcome(pubnub_res)), this, from);
    return connect(&pb, SIGNAL(outcome(pubnub_res)), this, to);
}


void pubnub_qt_fan_in::onPublish_1(pubnub_res result)
{
    d_out << "onPublish_1! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_pub1.last_publish_result() << "\n";
    d_publish_1_done = true;
}


void pubnub_qt_fan_in::onPublish_2(pubnub_res result)
{
    d_out << "onPublish_2! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_pub2.last_publish_result() << "\n";
    d_publish_2_done = true;
}


void pubnub_qt_fan_in::subscribe()
{
    pubnub_res result = d_pb_sub.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_in::onSubscribe(pubnub_res result)
{
    d_out << "onSubscribe! Result: '" << pubnub_res_2_string(result) << "' Messages: ";
    QList<QString> msg = d_pb_sub.get_all();
    for (int i = 0; i < msg.size(); ++i) {
        d_out << msg[i].toLatin1().data() << '\n';
    }
    if (d_publish_1_done && d_publish_2_done) {
        QCoreApplication::instance()->quit();
    }
    subscribe();
}


void pubnub_qt_fan_in::onConnect(pubnub_res result)
{
    d_out << "onConnect! Result: '" << pubnub_res_2_string(result) << "'\n";
    d_pb_pub1.set_uuid_v4_random();
    d_pb_pub2.set_uuid_v4_random();

    connect(&d_pb_pub1, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish_1(pubnub_res)));
    connect(&d_pb_pub2, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish_2(pubnub_res)));
    reconnect(d_pb_sub, SLOT(onConnect(pubnub_res)), SLOT(onSubscribe(pubnub_res)));
    result = d_pb_pub1.publish(chann, "\"Hello world from Qt - pb_pub1!\"");
    if (result != PNR_STARTED) {
        d_out << "Publish_1 failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
    result = d_pb_pub2.publish(chann, "\"Hello world from Qt - pb_pub2!\"");
    if (result != PNR_STARTED) {
        d_out << "Publish_2 failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
    subscribe();
}


void pubnub_qt_fan_in::execute()
{
    qDebug() << "execute()";

    d_pb_sub.set_uuid_v4_random();

    connect(&d_pb_sub, SIGNAL(outcome(pubnub_res)), this, SLOT(onConnect(pubnub_res)));
    subscribe();
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    pubnub_qt_fan_in sample;

    QTimer::singleShot(0, &sample, SLOT(execute()));
    
    return app.exec();
}
