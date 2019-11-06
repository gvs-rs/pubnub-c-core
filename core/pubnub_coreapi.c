/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_internal.h"

#include "pubnub_coreapi.h"
#include "pubnub_ccore.h"
#include "pubnub_netcore.h"
#include "pubnub_assert.h"
#include "pubnub_timers.h"

#include "pbpal.h"

#include <stdlib.h>
#include <ctype.h>


#if PUBNUB_ONLY_PUBSUB_API
#warning This module is not useful if configured to use only the publish and subscribe API
#endif


#if PUBNUB_USE_AUTO_HEARTBEAT
#include "lib/pb_strnlen_s.h"
static void update_list(char** list, const char* leave_list)
{
    const char* ll_start;
    size_t ll_len;
    const char* ll_end;
    
    PUBNUB_ASSERT_OPT(list != NULL);
    PUBNUB_ASSERT_OPT(leave_list != NULL);

    ll_len = pb_strnlen_s(leave_list, PUBNUB_MAX_OBJECT_LENGTH);
    if (0 == ll_len) {
        return;
    }
    ll_end = leave_list + ll_len;
    for (ll_start = leave_list; ll_start < ll_end;) {            
        const char* ch_end = (const char*)memchr(ll_start, ',', ll_end - ll_start);
        size_t ch_len;
        char* list_start;
        size_t list_len = pb_strnlen_s(*list, PUBNUB_MAX_OBJECT_LENGTH);
        char* list_end;
        
        if (0 == list_len) {
            break;
        }
        list_end = *list + list_len;
        if (NULL == ch_end) {
            ch_end = ll_end;
        }
        ch_len = ch_end - ll_start;
        for (list_start = *list; list_start < list_end;) {
            char* list_ch_end = (char*)memchr(list_start, ',', list_end - list_start);

            if (NULL == list_ch_end) {
                list_ch_end = list_end;
            }
            if ((strncmp(list_start, ll_start, ch_len) == 0) &&
                ((size_t)(list_ch_end - list_start) == ch_len)) {
                size_t rest = list_end - list_ch_end + 1;
                if (rest > 1) {
                    /* Moves everything behind next comma including string end */
                    memmove(list_start, list_ch_end + 1, rest);
                }
                else {
                    /* Erases last comma if any */
                    list_start[(list_start > *list) ? -1 : 0] = '\0';
                }
            }
            list_start = list_ch_end + 1;
        }
        ll_start = ch_end + 1;
    }
    if (0 == pb_strnlen_s(*list, PUBNUB_MAX_OBJECT_LENGTH)) {
        free(*list);
        *list = NULL;
    }
}


static void update_channels_and_ch_groups(pubnub_t* pb,
                                          const char* channel,
                                          const char* channel_group)
{
    PUBNUB_ASSERT_OPT(pb_valid_ctx_ptr(pb));

    if ((NULL == channel) && (NULL == channel_group)) {
        pbauto_heartbeat_free_register(pb);
    }
    if ((pb->autoRegister.channel != NULL) && (channel != NULL)) {
        update_list(&pb->autoRegister.channel, channel);
    }
    if ((pb->autoRegister.channel_group != NULL) && (channel_group != NULL)) {
        update_list(&pb->autoRegister.channel_group, channel_group);
    }
}


static void check_if_default_channel_and_groups(pubnub_t* p,
                                                char const* channel,
                                                char const* channel_group,
                                                char const** prep_channel,
                                                char const** prep_channel_group)
{
    PUBNUB_ASSERT_OPT(prep_channel != NULL);
    PUBNUB_ASSERT_OPT(prep_channel_group != NULL);

    if ((NULL == channel) && (NULL == channel_group)) {
        *prep_channel = p->autoRegister.channel;
        *prep_channel_group = p->autoRegister.channel_group;
    }
    else {
        *prep_channel = channel;
        *prep_channel_group = channel_group;
    }
}
#else
#define update_channels_and_ch_groups(pb, channel, channel_group)
#define check_if_default_channel_and_groups(p,                         \
                                            channel,                   \
                                            channel_group,             \
                                            prep_channel,              \
                                            prep_channel_group)        \
    do {                                                               \
        *(prep_channel) = (channel);                                   \
        *(prep_channel_group) = (channel_group);                       \
    } while(0)
#endif /* PUBNUB_USE_AUTO_HEARTBEAT */


enum pubnub_res pubnub_leave(pubnub_t* p, const char* channel, const char* channel_group)
{
    enum pubnub_res rslt;
    char const* prep_channel;
    char const* prep_channel_group;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(p));

    pubnub_mutex_lock(p->monitor);
    if (!pbnc_can_start_transaction(p)) {
        pubnub_mutex_unlock(p->monitor);
        return PNR_IN_PROGRESS;
    }
    check_if_default_channel_and_groups(p,
                                        channel,
                                        channel_group,
                                        &prep_channel,
                                        &prep_channel_group);
    
    rslt = pbcc_leave_prep(&p->core, prep_channel, prep_channel_group);
    if (PNR_STARTED == rslt) {
        p->trans            = PBTT_LEAVE;
        p->core.last_result = PNR_STARTED;
        update_channels_and_ch_groups(p, channel, channel_group);
        pbnc_fsm(p);
        rslt = p->core.last_result;
    }

    pubnub_mutex_unlock(p->monitor);
    return rslt;
}


enum pubnub_res pubnub_time(pubnub_t* p)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(p));

    pubnub_mutex_lock(p->monitor);
    if (!pbnc_can_start_transaction(p)) {
        pubnub_mutex_unlock(p->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_time_prep(&p->core);
    if (PNR_STARTED == rslt) {
        p->trans            = PBTT_TIME;
        p->core.last_result = PNR_STARTED;
        pbnc_fsm(p);
        rslt = p->core.last_result;
    }

    pubnub_mutex_unlock(p->monitor);
    return rslt;
}


enum pubnub_res pubnub_history(pubnub_t*   pb,
                               const char* channel,
                               unsigned    count,
                               bool        include_token)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_history_prep(
        &pb->core, channel, count, include_token, pbccNotSet, pbccNotSet, pbccNotSet, NULL, NULL);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_HISTORY;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_heartbeat(pubnub_t*   pb,
                                 const char* channel,
                                 const char* channel_group)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_heartbeat_prep(&pb->core, channel, channel_group);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_HEARTBEAT;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_here_now(pubnub_t*   pb,
                                const char* channel,
                                const char* channel_group)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_here_now_prep(
        &pb->core, channel, channel_group, pbccNotSet, pbccNotSet);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_HERENOW;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_global_here_now(pubnub_t* pb)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_here_now_prep(&pb->core, NULL, NULL, pbccNotSet, pbccNotSet);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_GLOBAL_HERENOW;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_where_now(pubnub_t* pb, const char* uuid)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_where_now_prep(&pb->core, uuid ? uuid : pbcc_uuid_get(&pb->core));
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_WHERENOW;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_set_state(pubnub_t*   pb,
                                 char const* channel,
                                 char const* channel_group,
                                 const char* uuid,
                                 char const* state)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_set_state_prep(
        &pb->core, channel, channel_group, uuid ? uuid : pbcc_uuid_get(&pb->core), state);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_SET_STATE;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_state_get(pubnub_t*   pb,
                                 char const* channel,
                                 char const* channel_group,
                                 const char* uuid)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_state_get_prep(
        &pb->core, channel, channel_group, uuid ? uuid : pbcc_uuid_get(&pb->core));
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_STATE_GET;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_remove_channel_group(pubnub_t* pb, char const* channel_group)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_remove_channel_group_prep(&pb->core, channel_group);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_REMOVE_CHANNEL_GROUP;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_remove_channel_from_group(pubnub_t*   pb,
                                                 char const* channel,
                                                 char const* channel_group)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_channel_registry_prep(&pb->core, channel_group, "remove", channel);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_REMOVE_CHANNEL_FROM_GROUP;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_add_channel_to_group(pubnub_t*   pb,
                                            char const* channel,
                                            char const* channel_group)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_channel_registry_prep(&pb->core, channel_group, "add", channel);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_ADD_CHANNEL_TO_GROUP;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


enum pubnub_res pubnub_list_channel_group(pubnub_t* pb, char const* channel_group)
{
    enum pubnub_res rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    if (!pbnc_can_start_transaction(pb)) {
        pubnub_mutex_unlock(pb->monitor);
        return PNR_IN_PROGRESS;
    }

    rslt = pbcc_channel_registry_prep(&pb->core, channel_group, NULL, NULL);
    if (PNR_STARTED == rslt) {
        pb->trans            = PBTT_LIST_CHANNEL_GROUP;
        pb->core.last_result = PNR_STARTED;
        pbnc_fsm(pb);
        rslt = pb->core.last_result;
    }

    pubnub_mutex_unlock(pb->monitor);
    return rslt;
}


bool pubnub_can_start_transaction(pubnub_t* pb)
{
    bool rslt;

    PUBNUB_ASSERT(pb_valid_ctx_ptr(pb));

    pubnub_mutex_lock(pb->monitor);
    rslt = pbnc_can_start_transaction(pb);
    pubnub_mutex_unlock(pb->monitor);

    return rslt;
}
