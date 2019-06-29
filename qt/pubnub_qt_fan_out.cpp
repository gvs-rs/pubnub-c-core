/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt_fan_out.h"

extern "C" {
#include "core/pubnub_helper.h"
}

#include <QCoreApplication>
#include <QTimer>

const QString chann = "hello_world";
static bool m_pb1_publish_done = false;
static bool m_pb2_connect_done = false;
static bool m_pb3_connect_done = false;
static bool m_pb2_subscribe_done = false;
static bool m_pb3_subscribe_done = false;


void pubnub_qt_fan_out::onPublish_pb1(pubnub_res result)
{
    d_out << "onPublish_pb1! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_1.last_publish_result() << "\n";
    m_pb1_publish_done = true;
}


void pubnub_qt_fan_out::onSubscribe_pb2(pubnub_res result)
{
    d_out << "onSubscribe_pb2! Result: '" << pubnub_res_2_string(result) << "'\n";
    if (!m_pb2_connect_done) {
        m_pb2_connect_done = true;
        if (m_pb3_connect_done) {
            result = d_pb_1.publish(chann, "\"Hello world from Qt - pb_1!\"");
            if (result != PNR_STARTED) {
                d_out << "Publish(pb_1) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
                QCoreApplication::instance()->quit();
            }
        }
    }
    else {
        d_out << " Messages: ";
        QList<QString> msg = d_pb_2.get_all();
        for (int i = 0; i < msg.size(); ++i) {
            d_out << msg[i].toLatin1().data() << '\n';
        }
        if (m_pb1_publish_done) {
            if (m_pb3_subscribe_done) {
                QCoreApplication::instance()->quit();
            }
            m_pb2_subscribe_done = true;
        }
    }
    result = d_pb_2.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_2) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_out::onSubscribe_pb3(pubnub_res result)
{
    d_out << "onSubscribe_pb3! Result: '" << pubnub_res_2_string(result) << "'\n";
    if (!m_pb3_connect_done) {
        m_pb3_connect_done = true;
        if (m_pb2_connect_done) {
            result = d_pb_1.publish(chann, "\"Hello world from Qt - pb_1!\"");
            if (result != PNR_STARTED) {
                d_out << "Publish(pb_1) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
                QCoreApplication::instance()->quit();
            }
        }
    }
    else {
        d_out << " Messages: ";
        QList<QString> msg = d_pb_3.get_all();
        for (int i = 0; i < msg.size(); ++i) {
            d_out << msg[i].toLatin1().data() << '\n';
        }
        if (m_pb1_publish_done) {
            if (m_pb2_subscribe_done) {
                QCoreApplication::instance()->quit();
            }
            m_pb3_subscribe_done = true;
        }
    }
    result = d_pb_3.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_3) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_out::execute()
{
    qDebug() << "execute()";

//    d_pb_1.set_ssl_options(0);
//    d_pb_2.set_ssl_options(0);
//    d_pb_3.set_ssl_options(0);
    d_pb_1.set_uuid_v4_random();
    d_pb_2.set_uuid_v4_random();
    d_pb_3.set_uuid_v4_random();

    connect(&d_pb_1, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish_pb1(pubnub_res)));
    connect(&d_pb_2, SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe_pb2(pubnub_res)));
    connect(&d_pb_3, SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe_pb3(pubnub_res)));
    pubnub_res result = d_pb_2.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_2) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
    result = d_pb_3.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb_3) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    pubnub_qt_fan_out sample;

    QTimer::singleShot(0, &sample, SLOT(execute()));
    
    return app.exec();
}
