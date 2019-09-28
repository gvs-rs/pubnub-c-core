/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PUBNUB_ACTIONS_API
#define INC_PUBNUB_ACTIONS_API

#include "pbcc_actions_api.h"

#include <stdbool.h>


/** Adds new type of message called action as a support for user reactions on a published
    messages.
    If the transaction is finished successfully response will have 'data' field with
    added action data.

    General notes:
    - All Actions API transactions have to have UUID set. If UUID is empty in the context
      (not set), any of these transactions will return error.
    - All Actions API transactions support auth parameter, but it can be empty(not set).
    - In most of Actions API transactions responses(all JSONS) if there is a "data" field
      present, means that transaction succeeded and its value 'carries' data payload.
      For 'pubnub_history_with_actions()' and pubnub_history_with_actions_more()' it is
      "channels" field instead of "data".
    - If there is no "data" field present than "error" field 'holds' the error description
    - For Actions API transactions that expect "channels" field, when it is not present
      than "error_message" field 'holds' the error description
    - If there is neither of the two expected keys present than functions return response
      format error

    @param pb The pubnub context. Can't be NULL
    @param channel The channel on which action is referring to.
    @param message_timetoken The timetoken of a published message action is applying to
    @param actype Action type
    @param value Json object describing the action that is to be added
    @return #PNR_STARTED on success, an error otherwise
  */
enum pubnub_res pubnub_add_action(pubnub_t* pb, 
                                  char const* channel, 
                                  char const* message_timetoken, 
                                  enum pubnub_action_type actype, 
                                  char const* value);


/** Searches the response(if previous transaction on the @p pb context had been
    pubnub_add_action and was accomplished successfully) and retrieves message timetoken
    ("messageTimetoken" key value).
    Example of pubnub_add_action response in the context buffer:
      {
        "status": 200,
        "data": {
           "type": "reaction",
           "value": "smiley_face",
           "uuid": "user-456",
           "actionTimetoken": 15610547826970050,
           "messageTimetoken": 15610547826969050
        }
      }
    If key expected is not found, preconditions were not fulfilled, or error was encountered,
    returned structure has 0 'size' field and NULL 'ptr' field.
    @param pb The pubnub context. Can't be NULL
    @return Structured pointer to memory block containing message timetoken value within the
            context response buffer
  */
pubnub_chamebl_t pubnub_get_message_timetoken(pubnub_t* pb);


/** Searches the response(if previous transaction on the @p pb context had been
    pubnub_add_action and was accomplished successfully) and retrieves action timetoken
    ("actionTimetoken" key value).
    Example of pubnub_add_action response in the context buffer:
      {
        "status": 200,
        "data": {
           "type": "reaction",
           "value": "smiley_face",
           "uuid": "user-456",
           "actionTimetoken": 15610547826970050,
           "messageTimetoken": 15610547826969050
        }
      }
    If key expected is not found, preconditions were not fulfilled, or error was encountered,
    returned structure has 0 'size' field and NULL 'ptr' field.
    @param pb The pubnub context. Can't be NULL
    @return Structured pointer to memory block containing action timetoken value within the
            context response buffer
  */
pubnub_chamebl_t pubnub_get_action_timetoken(pubnub_t* pb);


/** Initiates transaction that deletes(removes) previously added action on a published message.
    If the transaction is finished successfully response will have, for "data" key value, an
    empty object.
    In case data field is not present "error" field will provide error description.
    If there is neither of the two expected fields in the response it is considered format
    error.
    @param pb The pubnub context. Can't be NULL
    @param channel The channel on which action was previously added.
    @param message_timetoken The timetoken of a published message action was applied to.
                             (Obtained from the response when action was added)
    @param action_timetoken The action timetoken when it was added(Gotten from the transaction
                            response when action was added)
    @return #PNR_STARTED on success, an error otherwise
  */
enum pubnub_res pubnub_remove_action(pubnub_t* pb,
                                     char const* channel,
                                     char const* message_timetoken,
                                     char const* action_timetoken);


/** Initiates transaction that returns all actions added on a given @p channel between @p start
    and @p end action timetoken.
    If the transaction is finished successfully response will have "data" field with
    obtained actions data json object value.
    The response to this transaction can be partial and than it contains the field "more" with
    hyperlink string value to the rest.
    @see pubnub_get_actions_more()
    Example of pubnub_get_actions response:
      {
        "status": 200,
        "data": [
          {
            "type": "reaction",
            "value": "smiley_face",
            "uuid": "user-456",
            "actionTimetoken": 15610547826970050,
            "messageTimetoken": 15610547826969050
          },
          {
            "type": "custom",
            "value": "note",
            "uuid": "user-75",
            "actionTimetoken": 15610547826970053,
            "messageTimetoken": 15610547826969050
          }
        ],
        "more": "/v1/actions/sub-c-abc/channel/demo-channel?start=15610547826970050"
      }
    If server reports an error its description will be the "error" field string value.
    In case no "data", nor "error" field are present in the response it is considered
    format error.
    @param pb The pubnub context. Can't be NULL
    @param channel The channel on which actions are observed.
    @param start Start action timetoken. Can be NULL meaning there is no lower limitation in time.
    @param end End action timetoken. Can be NULL in which case upper time limit is present moment.
    @param limit Number of actions to return in response. Regular values 1 - 100. If you set `0`,
                 that means “use the default”. At the time of this writing, default was 100.
                 Any value greater than 100 is considered an error.
    @return #PNR_STARTED on success, an error otherwise
  */
enum pubnub_res pubnub_get_actions(pubnub_t* pb,
                                   char const* channel,
                                   char const* start,
                                   char const* end,
                                   size_t limit);


/** This function expects previous transaction to be the one for reading the actions and
    that it was successfully accomplished. If it is not the case, returns corresponding
    error.
    When preconditions are fulfilled, it searches for "more" field and if it finds it,
    retreives the hyperlink to the rest which it uses for obtaining another part of the
    server response. Anotherwords, once the hyperlink is found in the existing response
    it is used for initiating new request and function than behaves, essentially, as
    pubnub_get_actions().
    Example of the successful response on previous pubnub_get_actions, or
    pubnub_get_actions_more transaction:
      {
        "status": 200,
        "data": [
          {
            "type": "reaction",
            "value": "smiley_face",
            "uuid": "user-456",
            "actionTimetoken": 15610547826970050,
            "messageTimetoken": 15610547826969050
          }
        ],
        "more": "/v1/actions/sub-c-abc/channel/demo-channel?start=15610547826970050"
      }
    If there is no "more" field encountered in the previous transaction response it
    returns success: PNR_GOT_ALL_ACTIONS meaning that the answer is complete.
    @param pb The pubnub context containing response buffer to be searched. Can't be NULL
    @retval PNR_STARTED transaction successfully initiated.
    @retval PNR_GOT_ALL_ACTIONS transaction successfully finished.
    @retval corresponding error otherwise
  */
enum pubnub_res pubnub_get_actions_more(pubnub_t* pb);


/** Initiates transaction that returns all actions added on a given @p channel between @p start
    and @p end message timetoken.
    The response to this transaction can be partial and than it contains the field "more" with
    hyperlink string value to the rest.
    @see pubnub_history_with_actions_more()
    If the transaction is finished successfully response will have field "channels" with
    message history_with_actions data value.
    Example of pubnub_history_with_actions response:
      {
        "status": 200,
        "error": false,
        "error_message": "",
        "channels": {
          "demo-channel": [
            {
              "message": "Hi",
              "timetoken": 15610547826970040,
              "actions": {
                "receipt": {
                  "read": [
                    {
                      "uuid": "user-7",
                      "actionTimetoken": 15610547826970044
                    }
                  ]
                }
              }
            },
            {
              "message": "Hello",
              "timetoken": 15610547826970000,
              "actions": {
                "reaction": {
                  "smiley_face": [
                    {
                      "uuid": "user-456",
                      "actionTimetoken": 15610547826970050
                    }
                  ],
                  "poop_pile": [
                    {
                      "uuid": "user-789",
                      "actionTimetoken": 15610547826980050
                    },
                    {
                      "uuid": "user-567",
                      "actionTimetoken": 15610547826970000
                    }
                  ]
                }
              }
            }
          ]
        },
        "more": "/v1/history-with-actions/s/channel/c?start=15610547826970000&limit=98"
      }
    If server reports an error its description will be the string value of "error_message" field.
    In case no "channels", nor "error_message" field are present in the response it is considered
    format error.
    @param pb The pubnub context. Can't be NULL
    @param channel The channel on which actions are observed.
    @param start Start message timetoken. Can be NULL meaning there is no lower limitation in time.
    @param end End message timetoken. Can be NULL in which case upper time limit is present moment.
    @param limit Number of actions to return in response. Regular values 1 - 100. If you set `0`,
                 that means “use the default”. At the time of this writing, default was 100.
                 Any value greater than 100 is considered an error.
    @return #PNR_STARTED on success, an error otherwise
  */
enum pubnub_res pubnub_history_with_actions(pubnub_t* pb,
                                            char const* channel,
                                            char const* start,
                                            char const* end,
                                            size_t limit);


/** This function expects previous transaction to be the one for reading the history with
    actions and that it was successfully accomplished. If it is not the case, returns
    corresponding error.
    When preconditions are fulfilled it searches for "more" field and if it finds it,
    retreives the hyperlink to the rest of the answer which it uses for obtaining another
    part of the server response. Anotherwords, once the hyperlink is found in the existing
    response it is used for initiating new request and function than behaves as
    pubnub_history_with_actions().
    If there is no "more" field encountered in the previous transaction response it
    returns success: PNR_GOT_ALL_ACTIONS meaning that the answer is complete.
    @param pb The pubnub context containing response buffer to be searched. Can't be NULL
    @retval PNR_STARTED transaction successfully initiated.
    @retval PNR_GOT_ALL_ACTIONS transaction successfully finished.
    @retval corresponding error otherwise
  */
enum pubnub_res pubnub_history_with_actions_more(pubnub_t* pb);


#endif /* !defined INC_PUBNUB_ACTIONS_API */
