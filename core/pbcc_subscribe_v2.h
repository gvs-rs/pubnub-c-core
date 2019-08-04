/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PBCC_SUBSCRIBE_V2
#define INC_PBCC_SUBSCRIBE_V2

struct pbcc_context;

#include "pubnub_memory_block.h"

/** Pubnub V2 message has lots of data and here's how we express them
    for the pubnub_get_v2().

    The "string fields" are expressed as "Pascal strings", that is, a
    pointer with string length, and _don't_ include a NUL character.
    Also, these pointers are actually pointing into the full received
    message, so, their lifetime is tied to the message lifetime and
    any subsequent transaction on the same context will invalidate
    them.

*/
struct pubnub_v2_message {
    /** The time token of the message - when it was published. */
    struct pubnub_char_mem_block tt;
    /** Region of the message - not interesting in most cases */
    int region;
    /** Message flags */
    int flags;
    /** Channel that message was published to */
    struct pubnub_char_mem_block channel;
    /** Subscription match or the channel group */
    struct pubnub_char_mem_block match_or_group;
    /** The message itself */
    struct pubnub_char_mem_block payload;
    /** The message metadata, as published */
    struct pubnub_char_mem_block metadata;
    /** is the message a signal(, or published) */ 
    bool is_signal;
};


/** Prepares the Subscribe_v2 operation (transaction), mostly by
    formatting the URI of the HTTP request.
  */
enum pubnub_res pbcc_subscribe_v2_prep(struct pbcc_context* p,
                                       char const*          channel,
                                       char const*          channel_group,
                                       unsigned*            heartbeat,
                                       char const*          filter_expr);


/** Parses the string received as a response for a subscribe_v2 operation
    (transaction). This checks if the response is valid, and, if it
    is, prepares for giving the v2 messages that are received in the
    response to the user (via pbcc_get_msg_v2()).

    @param p The Pubnub C core context to parse the response "in"
    @return PNR_OK: OK, PNR_FORMAT_ERROR: error (invalid response)
  */
enum pubnub_res pbcc_parse_subscribe_v2_response(struct pbcc_context* p);


/** Returns the next v2 message from the Pubnub C Core context.
    Empty structure(memset to zeros) if there are no (more) v2 messages
  */
struct pubnub_v2_message pbcc_get_msg_v2(struct pbcc_context* p);


#endif /* !defined INC_PBCC_SUBSCRIBE_V2 */
