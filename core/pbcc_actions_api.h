/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PBCC_ACTIONS_API
#define INC_PBCC_ACTIONS_API

#include "pubnub_api_types.h"
#include "pubnub_memory_block.h"

struct pbcc_context;


enum pubnub_action_type {
    pbactypReaction,
    pbactypReceipt,
    pbactypCustom
};


enum pubnub_res pbcc_form_the_action_object(struct pbcc_context* pb,
                                            char* obj_buffer,
                                            size_t buffer_size,
                                            enum pubnub_action_type actype,
                                            char const** json);

enum pubnub_res pbcc_add_action_prep(struct pbcc_context* pb,
                                     char const* channel, 
                                     char const* message_timetoken, 
                                     char const* value);

pubnub_chamebl_t pbcc_get_message_timetoken(struct pbcc_context* pb);

pubnub_chamebl_t pbcc_get_action_timetoken(struct pbcc_context* pb);

enum pubnub_res pbcc_remove_action_prep(struct pbcc_context* pb,
                                        char const* channel,
                                        char const* message_timetoken,
                                        char const* action_timetoken);

enum pubnub_res pbcc_get_actions_prep(struct pbcc_context* pb,
                                      char const* channel,
                                      char const* start,
                                      char const* end,
                                      size_t limit);

enum pubnub_res pbcc_get_actions_more_prep(struct pbcc_context* pb);

enum pubnub_res pbcc_history_with_actions_prep(struct pbcc_context* pb,
                                               char const* channel,
                                               char const* start,
                                               char const* end,
                                               size_t limit);

enum pubnub_res pbcc_parse_actions_api_response(struct pbcc_context* pb);

enum pubnub_res pbcc_parse_history_with_actions_response(struct pbcc_context* pb);

#endif /* !defined INC_PBCC_ACTIONS_API */



