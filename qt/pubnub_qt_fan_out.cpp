/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_qt_fan_out.h"

extern "C" {
#include "core/pubnub_helper.h"
}

#include <QCoreApplication>
#include <QTimer>

const QString chann = "hello_world";


void pubnub_qt_fan_out::onPublish(pubnub_res result)
{
    d_out << "onPublish! Result: '" << pubnub_res_2_string(result)
          << "', Response: " << d_pb_pub.last_publish_result() << "\n";
    d_publish_done = true;
}


void pubnub_qt_fan_out::publish_if_ready()
{
    if (d_connect_1_done && d_connect_2_done) {
        pubnub_res result = d_pb_pub.publish(chann, "\"Hello world from Qt - pb_pub!\"");
        if (result != PNR_STARTED) {
            d_out << "Publish failed, result: '"<< pubnub_res_2_string(result) << "'\n";
            QCoreApplication::instance()->quit();
        }
    }
}


void pubnub_qt_fan_out::exit_when_done()
{
    if (d_subscribe_1_done && d_subscribe_2_done) {
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


void pubnub_qt_fan_out::onSubscribe_1(pubnub_res result)
{
    d_out << "onSubscribe_1! Result: '" << pubnub_res_2_string(result) << "'\n";
    if (!d_connect_1_done) {
        d_connect_1_done = true;
        publish_if_ready();
    }
    else {
        print_messages(d_pb_sub1);
        if (d_publish_done) {
            d_subscribe_1_done = true;
            exit_when_done();
        }
    }
    subscribe_ctx(d_pb_sub1);
}


void pubnub_qt_fan_out::onSubscribe_2(pubnub_res result)
{
    d_out << "onSubscribe_2! Result: '" << pubnub_res_2_string(result) << "'\n";
    if (!d_connect_2_done) {
        d_connect_2_done = true;
        publish_if_ready();
    }
    else {
        print_messages(d_pb_sub2);
        if (d_publish_done) {
            d_subscribe_2_done = true;
            exit_when_done();
        }
    }
    subscribe_ctx(d_pb_sub2);
}


void pubnub_qt_fan_out::execute()
{
    qDebug() << "execute()";

    d_pb_pub.set_uuid_v4_random();
    d_pb_sub1.set_uuid_v4_random();
    d_pb_sub2.set_uuid_v4_random();

    connect(&d_pb_pub, SIGNAL(outcome(pubnub_res)), this, SLOT(onPublish(pubnub_res)));
    connect(&d_pb_sub1, SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe_1(pubnub_res)));
    connect(&d_pb_sub2, SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe_2(pubnub_res)));
    subscribe_ctx(d_pb_sub1);
    subscribe_ctx(d_pb_sub2);
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    pubnub_qt_fan_out sample;

    QTimer::singleShot(0, &sample, SLOT(execute()));
    
    return app.exec();
}
