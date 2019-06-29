/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt_fan_in.h"

extern "C" {
#include "core/pubnub_helper.h"
}

#include <QCoreApplication>
#include <QTimer>

const QString chann = "hello_world";
bool m_pb1_publish_done = false;
bool m_pb2_publish_done = false;


bool pubnub_qt_fan_in::reconnect(pubnub_qt& pb, char const *from, char const *to)
{
    disconnect(&pb, SIGNAL(outcome(pubnub_res)), this, from);
    return connect(&pb, SIGNAL(outcome(pubnub_res)), this, to);
}


void pubnub_qt_fan_in::onPublish_pb1(pubnub_res result)
{
    d_out << "onPublish_pb1! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_1.last_publish_result() << "\n";
    m_pb1_publish_done = true;
}


void pubnub_qt_fan_in::onPublish_pb2(pubnub_res result)
{
    d_out << "onPublish_pb2! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_2.last_publish_result() << "\n";
    m_pb2_publish_done = true;
}


void pubnub_qt_fan_in::onSubscribe_pb3(pubnub_res result)
{
    d_out << "onSubscribe_pb3! Result: '" << pubnub_res_2_string(result) << "' Messages:\n";
    QList<QString> msg = d_pb_3.get_all();
    for (int i = 0; i < msg.size(); ++i) {
        d_out << msg[i].toLatin1().data() << '\n';
    }
    if (m_pb1_publish_done && m_pb2_publish_done) {
        QCoreApplication::instance()->quit();
    }
    result = d_pb_3.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_3) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_in::onConnect_pb3(pubnub_res result)
{
    d_out << "onConnect_pb3! Result: '" << pubnub_res_2_string(result) << "'\n";
//    d_pb_1.set_ssl_options(0);
//    d_pb_2.set_ssl_options(0);
    d_pb_1.set_uuid_v4_random();
    d_pb_2.set_uuid_v4_random();

    connect(&d_pb_1, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish_pb1(pubnub_res)));
    connect(&d_pb_2, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish_pb2(pubnub_res)));
    reconnect(d_pb_3, SLOT(onConnect_pb3(pubnub_res)), SLOT(onSubscribe_pb3(pubnub_res)));
    result = d_pb_1.publish(chann, "\"Hello world from Qt - pb_1!\"");
    if (result != PNR_STARTED) {
        d_out << "Publish(pb_1) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
    result = d_pb_2.publish(chann, "\"Hello world from Qt - pb_2!\"");
    if (result != PNR_STARTED) {
        d_out << "Publish(pb_2) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
    result = d_pb_3.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_3) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_in::execute()
{
    qDebug() << "execute()";

//    d_pb_3.set_ssl_options(0);
    d_pb_3.set_uuid_v4_random();

    connect(&d_pb_3, SIGNAL(outcome(pubnub_res)), this, SLOT(onConnect_pb3(pubnub_res)));
    pubnub_res result = d_pb_3.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_3) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    pubnub_qt_fan_in sample;

    QTimer::singleShot(0, &sample, SLOT(execute()));
    
    return app.exec();
}
