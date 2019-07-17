/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_internal.h"

#include "core/pubnub_entity_api.h"
#include "core/pbcc_entity_api.h"
#include "core/pubnub_ccore.h"
#include "core/pubnub_netcore.h"
#include "core/pubnub_assert.h"
#include "core/pubnub_timers.h"

#include "core/pbpal.h"

#include <ctype.h>
#include <string.h>

/** Returns a paginated list of users associated with the subscription key of the context @p pb,
    optionally including each record's custom data object.
    @param include array of (C) strings with additional/complex user attributes to include in
                   response. Use NULL if you don't want to retrieve additional attributes.
    @param include_count dimension of @p include. Set 0 if you don’t want to retrieve additional
                         attributes
    @param limit Number of entities to return in response. Regular values 1 - 100. If you set `0`,
                 that means “use the default”. At the time of this writing, default was 100.
    @param start Previously-returned cursor bookmark for fetching the next page. Use NULL if you
                 don’t want to paginate with a start bookmark.
    @param end Previously-returned cursor bookmark for fetching the previous page. Ignored if you
               also supply the start parameter. Use NULL if you don’t want to paginate with an
               end bookmark.
    @param count Request totalCount to be included in paginated response. By default, totalCount
                 is omitted.
  */
enum pubnub_res pubnub_fetch_all_users(pubnub_t* pb, 
                                       char const** include, 
                                       size_t include_count,
                                       size_t limit,
                                       char const* start,
                                       char const* end,
                                       enum pubnub_tribool count)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_fetch_all_users_prep(&pb->core,
                                     include, 
                                     include_count,
                                     limit,
                                     start,
                                     end,
                                     count);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_FETCH_ALL_USERS;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Creates a user with the attributes specified in @p user_obj.
    Returns the created user object, optionally including the user's
    custom data object.
    @note User ID and name are required properties in the @p user_obj
    @param user_obj The JSON string with the definition of the User
                    Object to create.
  */
enum pubnub_res pubnub_create_user(pubnub_t* pb, 
                                   char const** include, 
                                   size_t include_count,
                                   char const* user_obj)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

#if PUBNUB_USE_GZIP_COMPRESSION
    pb->core.gzip_msg_len = 0;
    if (pbgzip_compress(pb, user_obj) == PNR_OK) {
        user_obj = pb->core.gzip_msg_buf;
    }
#endif
    rslt = pbcc_create_user_prep(&pb->core, include, include_count, user_obj);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_CREATE_USER;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_via_post = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Returns the user object specified with @p user_id, optionally including the user's
    custom data object.
    @param user_id The User ID for which to retrieve the user object.
                   Cannot be NULL.
  */
enum pubnub_res pubnub_fetch_user(pubnub_t* pb,
                                  char const** include, 
                                  size_t include_count,
                                  char const* user_id)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_fetch_user_prep(&pb->core, include, include_count, user_id);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_FETCH_USER;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Updates the user object specified with the `id` key of the @p user_obj with any new
    information you provide. Returns the updated user object, optionally including
    the user's custom data object. 
  */
enum pubnub_res pubnub_update_user(pubnub_t* pb, 
                                   char const** include,
                                   size_t include_count,
                                   char const* user_obj)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_update_user_prep(&pb->core, include, include_count, user_obj);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_UPDATE_USER;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_via_post = pb->flags.is_patch_or_delete = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Deletes the user specified with @p user_id.
  */
enum pubnub_res pubnub_delete_user(pubnub_t* pb, char const* user_id)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_delete_user_prep(&pb->core, user_id);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_DELETE_USER;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_patch_or_delete = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Returns the spaces associated with the subscriber key of the context @p pb, optionally
    including each space's custom data object.
    @param include array of (C) strings with additional/complex attributes to include in response.
                   Use NULL if you don't want to retrieve additional attributes.
    @param include_count dimension of @p include. Set 0 if you don’t want to retrieve additional
                         attributes
    @param limit Number of entities to return in response. Regular values 1 - 100. If you set `0`,
                 that means “use the default”. At the time of this writing, default was 100.
    @param start Previously-returned cursor bookmark for fetching the next page. Use NULL if you
                 don’t want to paginate with a start bookmark.
    @param end Previously-returned cursor bookmark for fetching the previous page. Ignored if
               you also supply the start parameter. Use NULL if you don’t want to paginate with
               an end bookmark.
    @param count Request totalCount to be included in paginated response. By default, totalCount
                 is omitted.
  */
enum pubnub_res pubnub_fetch_all_spaces(pubnub_t* pb, 
                                        char const** include, 
                                        size_t include_count,
                                        size_t limit,
                                        char const* start,
                                        char const* end,
                                        enum pubnub_tribool count)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_fetch_all_spaces_prep(&pb->core,
                                      include, 
                                      include_count,
                                      limit,
                                      start,
                                      end,
                                      count);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_FETCH_ALL_SPACES;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Creates a space with the attributes specified in @p space_obj.
    Returns the created space object, optionally including its custom data object.
    @note Space ID and name are required properties of @p space_obj
    @param space_obj The JSON string with the definition of the Space Object to create.
  */
enum pubnub_res pubnub_create_space(pubnub_t* pb, 
                                    char const** include, 
                                    size_t include_count,
                                    char const* space_obj)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

#if PUBNUB_USE_GZIP_COMPRESSION
    pb->core.gzip_msg_len = 0;
    if (pbgzip_compress(pb, space_obj) == PNR_OK) {
        space_obj = pb->core.gzip_msg_buf;
    }
#endif
    rslt = pbcc_create_space_prep(&pb->core, include, include_count, space_obj);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_CREATE_SPACE;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_via_post = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Returns the space object specified with @p space_id, optionally including its custom
    data object.
    @param space_id The Space ID for which to retrieve the space object. Cannot be NULL.
  */
enum pubnub_res pubnub_fetch_space(pubnub_t* pb,
                                   char const** include, 
                                   size_t include_count,
                                   char const* space_id)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_fetch_space_prep(&pb->core, include, include_count, space_id);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_FETCH_SPACE;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Updates the space specified by the `id` property of the @p space_obj. Returns the space object,
    optionally including its custom data object. 
  */
enum pubnub_res pubnub_update_space(pubnub_t* pb, 
                                    char const** include,
                                    size_t include_count,
                                    char const* space_obj)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_update_space_prep(&pb->core, include, include_count, space_obj);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_UPDATE_SPACE;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_via_post = pb->flags.is_patch_or_delete = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Deletes the space specified with @p space_id.
  */
enum pubnub_res pubnub_delete_space(pubnub_t* pb, char const* space_id)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_delete_space_prep(&pb->core, space_id);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_DELETE_SPACE;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_patch_or_delete = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Returns the space memberships of the user specified with @p user_id, optionally including
    the custom data objects for… (ОВО ИЗЛЕДА НИЈЕ БАШ ДОВРШЕНО, МОГУЋЕ ДА ОВОГА, ОДНОСНО, `инцлуде` ПАРАМЕТРА УОПШТЕ НЕЋЕ БИТИ)
    @param include array of (C) strings with additional/complex attributes to include in response.
                   Use NULL if you don't want to retrieve additional attributes.
    @param include_count dimension of @p include. Set 0 if you don’t want to retrieve additional
                         attributes
    @param limit Number of entities to return in response. Regular values 1 - 100.
                 If you set `0`, that means “use the default”. At the time of this writing,
                 default was 100.
    @param start Previously-returned cursor bookmark for fetching the next page. Use NULL if you
                 don’t want to paginate with a start bookmark.
    @param end Previously-returned cursor bookmark for fetching the previous page. Ignored if
               you also supply the start parameter. Use NULL if you don’t want to paginate with
               an end bookmark.
    @param count Request totalCount to be included in paginated response. By default, totalCount
                 is omitted.
*/
enum pubnub_res pubnub_fetch_users_space_memberships(pubnub_t* pb,
                                                     char const* user_id,
                                                     char const** include,
                                                     size_t include_count,
                                                     size_t limit,
                                                     char const* start,
                                                     char const* end,
                                                     enum pubnub_tribool count)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_fetch_users_space_memberships_prep(&pb->core,
                                                   user_id,
                                                   include, 
                                                   include_count,
                                                   limit,
                                                   start,
                                                   end,
                                                   count);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_FETCH_USERS_SPACE_MEMBERSHIPS;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Updates the space memberships of the user specified by @p user_id. Use the `add`,
    `update`, and `remove` properties in the @p update_obj to perform those operations
    on one, or more memberships. Returns the user's space memberships, optionally
    including the custom data objects for …(ОВО ИЗЛЕДА НИЈЕ БАШ ДОВРШЕНО, МОГУЋЕ ДА
    ОВОГА, ОДНОСНО, `инцлуде` ПАРАМЕТРА УОПШТЕ НЕЋЕ БИТИ)

    An example for @update_obj:
    {
      "add": [
        {
          "id": "my-channel"
        }
      ],
      "update": [
        {
          "id": "main",
          "custom": {
            "starred": true
          }
        }
      ],
      "remove": [
        {
          "id": "space-0"
        }
      ]
    }
    @param update_obj The JSON object that defines the updates to perform.
                      Cannot be NULL.
  */
enum pubnub_res pubnub_update_users_space_memberships(pubnub_t* pb, 
                                                      char const* user_id,
                                                      char const** include,
                                                      size_t include_count,
                                                      char const* update_obj)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
#if PUBNUB_USE_GZIP_COMPRESSION
    pb->core.gzip_msg_len = 0;
    if (pbgzip_compress(pb, update_obj) == PNR_OK) {
        update_obj = pb->core.gzip_msg_buf;
    }
#endif
    rslt = pbcc_update_users_space_memberships_prep(&pb->core,
                                                    user_id,
                                                    include,
                                                    include_count,
                                                    update_obj);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_UPDATE_USERS_SPACE_MEMBERSHIPS;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_via_post = pb->flags.is_patch_or_delete = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Returns all users in the space specified with @p space_id, optionally including
    the custom data objects for… (ОВО ИЗЛЕДА НИЈЕ БАШ ДОВРШЕНО, МОГУЋЕ ДА ОВОГА, ОДНОСНО,
    `инцлуде` ПАРАМЕТРА УОПШТЕ НЕЋЕ БИТИ)
    @param space_id The Space ID for which to retrieve the user object.
  */
enum pubnub_res pubnub_fetch_members_in_space(pubnub_t* pb,
                                              char const* space_id,
                                              char const** include,
                                              size_t include_count,
                                              size_t limit,
                                              char const* start,
                                              char const* end,
                                              enum pubnub_tribool count)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
    rslt = pbcc_fetch_members_in_space_prep(&pb->core,
                                            space_id,
                                            include,
                                            include_count,
                                            limit,
                                            start,
                                            end,
                                            count);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_FETCH_MEMBERS_IN_SPACE;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}


/** Updates the list of members of the space specified with @p space_id. Use the `add`,
    `update`, and `remove` properties in the @p update_obj to perform those operations
    on one, or more memberships. Returns the spaces's memberships, optionally including
    the custom data objects for …(ОВО ИЗЛЕДА НИЈЕ БАШ ДОВРШЕНО, МОГУЋЕ ДА ОВОГА, ОДНОСНО,
    `инцлуде` ПАРАМЕТРА УОПШТЕ НЕЋЕ БИТИ)

    An example for @update_obj:
    {
      "add": [
        {
          "id": "user-1"
        }
      ],
      "update": [
        {
          "id": "user-2",
          "custom": {
            "role": “moderator”
          }
        }
      ],
      "remove": [
        {
          "id": "user-0"
        }
      ]
    }
    @param update_obj The JSON object that defines the updates to perform. Cannot be NULL.
  */
enum pubnub_res pubnub_update_members_in_space(pubnub_t* pb, 
                                               char const* space_id,
                                               char const** include,
                                               size_t include_count,
                                               char const* update_obj)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }
    
#if PUBNUB_USE_GZIP_COMPRESSION
    pb->core.gzip_msg_len = 0;
    if (pbgzip_compress(pb, update_obj) == PNR_OK) {
        update_obj = pb->core.gzip_msg_buf;
    }
#endif
    rslt = pbcc_update_members_in_space_prep(&pb->core,
                                             space_id,
                                             include,
                                             include_count,
                                             update_obj);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_UPDATE_MEMBERS_IN_SPACE;
        pb->core.last_result = PNR_STARTED;
        pb->flags.is_via_post = pb->flags.is_patch_or_delete = true;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}
