/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub.hpp"

#include <iostream>
#include <exception>
#include <pthread.h>


static void wait_seconds(double time_in_seconds)
{
    time_t  start = time(NULL);
    double time_passed_in_seconds;
    do {
        time_passed_in_seconds = difftime(time(NULL), start);
    } while (time_passed_in_seconds < time_in_seconds);
}


static void* wait_and_than_cancel(void* pb)
{
    for (;;) {
        /* Wait for 6 minutes before canceling the operation */
        wait_seconds(360);
        static_cast<pubnub::context*>(pb)->set_proxy_manual(pubnub::http_connect_proxy,
                                                            "ezecom.110.74.196.235.ezecom.com.kh",
                                                            46140);
        static_cast<pubnub::context*>(pb)->cancel();
    }
    return pb;
}


int main()
{
    try {
        std::string chan("hello_world");
        pubnub::context pb("demo", "demo");
        int rslt;
        pthread_t thread_id;
 
        /* Leave this commented out to use the default - which is
           blocking I/O on most platforms. Uncomment to use non-
           blocking I/O.
        */
        pb.set_blocking_io(pubnub::non_blocking);
        
        if (0 != pb.set_uuid_v4_random()) {
            pb.set_uuid("grof-monte-cristo");
        }
        else {
            std::cout << "Generated UUID: " << pb.uuid() << std::endl;
        }
        pb.set_auth("belgrade");
        pb.set_proxy_manual(pubnub::http_connect_proxy, "ip17-149.cbn.net.id", 30204);
//        pb.set_proxy_manual(pubnub::http_connect_proxy, "127.0.0.1", 8888);
/*
        pb.set_proxy_manual(pubnub::http_connect_proxy,
                            "110.74.196.235",
                            46140);
*/
/*
        pb.set_transaction_timeout(
#if __cplusplus >= 201103L
            std::chrono::seconds(30)
#else
            30000
#endif
            );
*/

        std::cout << "First subscribe / connect" << std::endl;
        if (PNR_OK ==  pb.subscribe(chan).await()) {
            std::cout << "Subscribe/connected!" << std::endl;
        }
        else {
            std::cout << "Subscribe failed!" << std::endl;
        }
        
        rslt = pthread_create(&thread_id, NULL, wait_and_than_cancel, static_cast<void*>(&pb));
        if (rslt != 0) {
            std::cout << "Failed to create the canceling thread, error code: " << rslt << "\n";
            return -1;
        }
        
        for (;;) {
            enum pubnub_res res = pb.subscribe(chan).await();
            if (PNR_OK == res) {
                std::cout << "Subscribed! Got messages:" << std::endl;
                /// Lets illustrate getting all the message in a vector,
                /// and iterating over it
#if __cplusplus >= 201103L
                auto msg = pb.get_all();
                for (auto it = msg.begin(); it != msg.end(); ++it) {
                    std::cout << *it << std::endl;
                }
#else
                std::vector<std::string> msg = pb.get_all();
                for (std::vector<std::string>::iterator it = msg.begin(); it != msg.end(); ++it) {
                    std::cout << *it << std::endl;
                }
#endif
            }
            else if (PNR_CANCELLED == res) {
                std::cout << "Subscribe canceled!" << std::endl;
            }
            else {
                std::cout << "Subscribe failed!" << std::endl;
            }
        }
    }
    catch (std::exception &exc) {
        std::cout << "Caught exception: " << exc.what() << std::endl;
    }

std::cout << "Pubnub C++ " <<
#if defined(PUBNUB_CALLBACK_API)
    "callback" <<
#else
    "sync" <<
#endif
    " demo over." << std::endl;

    return 0;
}
