/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt_fan_out.h"

extern "C" {
#include "core/pubnub_helper.h"
}

#include <QCoreApplication>
#include <QTimer>

const QString chann = "hello_world";


void pubnub_qt_fan_out::onPublish_pb1(pubnub_res result)
{
    d_out << "onPublish_pb1! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_1.last_publish_result() << "\n";
    d_pb1_publish_done = true;
}


void pubnub_qt_fan_out::pb1_publish_if_ready()
{
    if (d_pb2_connect_done && d_pb3_connect_done) {
        pubnub_res result = d_pb_1.publish(chann, "\"Hello world from Qt - pb_1!\"");
        if (result != PNR_STARTED) {
            d_out << "Publish(pb_1) failed, result: '"<< pubnub_res_2_string(result) << "'\n";
            QCoreApplication::instance()->quit();
        }
    }
}


void pubnub_qt_fan_out::exit_when_done()
{
    if (d_pb2_subscribe_done && d_pb3_subscribe_done) {
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_out::print_messages(pubnub_qt& pb)
{
    d_out << " Messages: ";
    QList<QString> msg = pb.get_all();
    for (int i = 0; i < msg.size(); ++i) {
        d_out << msg[i].toLatin1().data() << '\n';
    }
}


void pubnub_qt_fan_out::subscribe_ctx(pubnub_qt& pb)
{
    pubnub_res result = pb.subscribe(chann);
    if (result != PNR_STARTED) {
        d_out << "Subscribe(pb=" << &pb <<") failed, result: '"<< pubnub_res_2_string(result) << "'\n";
        QCoreApplication::instance()->quit();
    }
}


void pubnub_qt_fan_out::onSubscribe_pb2(pubnub_res result)
{
    d_out << "onSubscribe_pb2! Result: '" << pubnub_res_2_string(result) << "'\n";
    if (!d_pb2_connect_done) {
        d_pb2_connect_done = true;
        pb1_publish_if_ready();
    }
    else {
        print_messages(d_pb_2);
        if (d_pb1_publish_done) {
            d_pb2_subscribe_done = true;
            exit_when_done();
        }
    }
    subscribe_ctx(d_pb_2);
}


void pubnub_qt_fan_out::onSubscribe_pb3(pubnub_res result)
{
    d_out << "onSubscribe_pb3! Result: '" << pubnub_res_2_string(result) << "'\n";
    if (!d_pb3_connect_done) {
        d_pb3_connect_done = true;
        pb1_publish_if_ready();
    }
    else {
        print_messages(d_pb_3);
        if (d_pb1_publish_done) {
            d_pb3_subscribe_done = true;
            exit_when_done();
        }
    }
    subscribe_ctx(d_pb_3);
}


void pubnub_qt_fan_out::execute()
{
    qDebug() << "execute()";

    d_pb_1.set_uuid_v4_random();
    d_pb_2.set_uuid_v4_random();
    d_pb_3.set_uuid_v4_random();

    connect(&d_pb_1, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish_pb1(pubnub_res)));
    connect(&d_pb_2, SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe_pb2(pubnub_res)));
    connect(&d_pb_3, SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe_pb3(pubnub_res)));
    subscribe_ctx(d_pb_2);
    subscribe_ctx(d_pb_3);
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    pubnub_qt_fan_out sample;

    QTimer::singleShot(0, &sample, SLOT(execute()));
    
    return app.exec();
}
