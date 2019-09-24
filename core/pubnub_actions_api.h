/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PUBNUB_ACTIONS_API
#define INC_PUBNUB_ACTIONS_API

#include "pbcc_actions_api.h"

#include <stdbool.h>


/** Adds new type of message called action as a support for user reactions on a published
    messages.

    General notes:
    - All Actions API transactions have to have UUID set. If UUID is empty in the context
      (not set), any of these transactions will return error.
    - All Actions API transactions support auth parameter, but it can be empty(not set).
    - In most of Actions API transactions responses(all JSONS) if there is a "data" field
      present, means that transaction succeeded and it 'carries' data payload.
      For 'pubnub_history_with_actions()' and pubnub_history_with_actions_more()' it is
      field "channels" instead of "data".
    - If there is no "data" field present than "error" field 'holds' the error description
    - For Actions API transactions that expect "channels" field, when its not present than
      "error_message" field 'holds' the error description
    - If there is neither of the two expected keys present than functions return response
      format error
  */
enum pubnub_res pubnub_add_action(pubnub_t* pb, 
                                  char const* channel, 
                                  char const* message_timetoken, 
                                  enum pubnub_action_type actype, 
                                  char const* value);

pubnub_chamebl_t pubnub_get_message_timetoken(pubnub_t* pb);

pubnub_chamebl_t pubnub_get_action_timetoken(pubnub_t* pb);

enum pubnub_res pubnub_remove_action(pubnub_t* pb,
                                     char const* channel,
                                     char const* message_timetoken,
                                     char const* action_timetoken);

enum pubnub_res pubnub_get_actions(pubnub_t* pb,
                                   char const* channel,
                                   char const* start,
                                   char const* end,
                                   size_t limit);

enum pubnub_res pubnub_get_actions_more(pubnub_t* pb);

enum pubnub_res pubnub_history_with_actions(pubnub_t* pb,
                                            char const* channel,
                                            char const* start,
                                            char const* end,
                                            size_t limit);

enum pubnub_res pubnub_history_with_actions_more(pubnub_t* pb);


#endif /* !defined INC_PUBNUB_ACTIONS_API */
