/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PBCC_ACTIONS_API
#define INC_PBCC_ACTIONS_API

#include "pubnub_api_types.h"
#include "pubnub_memory_block.h"

struct pbcc_context;


/** @file pbcc_actions_api.h

    This has the functions for formating and parsing the
    requests and responses for 'Actions API' transactions
  */

/** Possible action types */
enum pubnub_action_type {
    pbactypReaction,
    pbactypReceipt,
    pbactypCustom
};

/** Forms the action object from @p json describing the action that will be 'added', later on,
    via POST method in 'pubnub_add_action' request body.
    Format of new object:
      { "type": "reaction" | "receipt" | "custom",
        "value": json,
        "uuid": string-from-context
      }
    @param pb The pubnub ccore context. Can't be NULL
    @param obj_buffer buffer provided for new json object
    @param buffer_size Size of provided buffer
    @param actype Action type
    @param json On entrace to the function points to json object describing the action.
                When function successfully returns points to the beggining of @p obj_buffer
                where newly formed object is placed
    @return #PNR_OK on success, an error otherwise
  */
enum pubnub_res pbcc_form_the_action_object(struct pbcc_context* pb,
                                            char* obj_buffer,
                                            size_t buffer_size,
                                            enum pubnub_action_type actype,
                                            char const** json);

/** Prepares the 'add_action' transaction, mostly by
    formatting the URI of the HTTP request.
  */
enum pubnub_res pbcc_add_action_prep(struct pbcc_context* pb,
                                     char const* channel, 
                                     char const* message_timetoken, 
                                     char const* value);

/** Searches the response(if previous transaction on the @p pb ccore_context had been
    accomplished successfully) and retrieves message timetoken("messageTimetoken" key value).
    Example of 'add_action' response in the ccore_context buffer:
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
    @param pb The pubnub ccore_context. Can't be NULL
    @return Structured pointer to memory block containing message timetoken value within the
            ccore_context response buffer
  */
pubnub_chamebl_t pbcc_get_message_timetoken(struct pbcc_context* pb);

/** Searches the response(if previous transaction on the @p pb ccore_context had been
    accomplished successfully) and retrieves action timetoken("actionTimetoken" key value).
    Example of 'add_action' response in the ccore_context buffer:
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
    @param pb The pubnub ccore_context. Can't be NULL
    @return Structured pointer to memory block containing action timetoken value within the
            ccore_context response buffer
  */
pubnub_chamebl_t pbcc_get_action_timetoken(struct pbcc_context* pb);

/** Prepares the 'remove_action' transaction, mostly by
    formatting the URI of the HTTP request.
  */
enum pubnub_res pbcc_remove_action_prep(struct pbcc_context* pb,
                                        char const* channel,
                                        char const* message_timetoken,
                                        char const* action_timetoken);

/** Prepares the 'get_actions' transaction, mostly by
    formatting the URI of the HTTP request.
  */
enum pubnub_res pbcc_get_actions_prep(struct pbcc_context* pb,
                                      char const* channel,
                                      char const* start,
                                      char const* end,
                                      size_t limit);

/** This function expects previous transaction to be successfully accomplished.
    If it is not the case, returns corresponding error.
    When precondition is fulfilled, it searches for "more" field and if it finds it,
    retreives the hyperlink to the rest which is used for obtaining another part of the
    server response. Anotherwords, once the hyperlink is found in the existing response
    it is used for formatting the URI of new HTTP request.
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
    returns success: PNR_GOT_ALL_ACTIONS meaning that already received answer is complete.
    @param pb The pubnub ccore_context containing response buffer to be searched. Can't be NULL
    @retval PNR_STARTED transaction successfully initiated.
    @retval PNR_GOT_ALL_ACTIONS transaction successfully finished.
    @retval corresponding error otherwise
  */
enum pubnub_res pbcc_get_actions_more_prep(struct pbcc_context* pb);

/** Prepares the 'history_with_actions' transaction, mostly by
    formatting the URI of the HTTP request.
  */
enum pubnub_res pbcc_history_with_actions_prep(struct pbcc_context* pb,
                                               char const* channel,
                                               char const* start,
                                               char const* end,
                                               size_t limit);

/** Parses server response simply on most 'Actions API' transaction requests.
    If transaction is successful, the response(a JSON object) will have key
    "data" with corresponding value. If not, there should be "error" key 'holding'
    error description. If there is neither of the two keys mentioned function
    returns response format error.
    Complete answer will be available via pubnub_get().

    @retval PNR_OK on success
    @retval PNR_ACTIONS_API_ERROR on error
    @retval PNR_FORMAT_ERROR no "data", nor "error" key present in response
  */
enum pubnub_res pbcc_parse_actions_api_response(struct pbcc_context* pb);

/** Parses server response simply on 'pubnub_history_with_actions' transaction request.
    If transaction is successful, the response(a JSON object) will have key
    "channels" with corresponding value. If not, there should be "error_message" key
    'holding' error description. If there is neither of the two keys mentioned function
    returns response format error.
    Complete answer will be available via pubnub_get().

    @retval PNR_OK on success
    @retval PNR_ACTIONS_API_ERROR on error
    @retval PNR_FORMAT_ERROR no "channels", nor "error_mesage" key present in response
  */
enum pubnub_res pbcc_parse_history_with_actions_response(struct pbcc_context* pb);

#endif /* !defined INC_PBCC_ACTIONS_API */



