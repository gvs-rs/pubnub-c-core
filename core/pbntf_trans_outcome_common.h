/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */

#if PUBNUB_USE_ENTITY_API
#define ENTITY_API_PATCH_OR_DELETE_TO_FALSE(flags) (flags).is_patch_or_delete = false
#else                                                               
#define ENTITY_API_PATCH_OR_DELETE_TO_FALSE(flags)
#endif

/** This macro does the common "stuff to do" on the outcome of a
    transaction. Should be used by all `pbntf_trans_outcome()`
    functions.

    In case of PubNub protocol error, resets the time-token. This
    means some messages were (possibly) lost, but allows us to recover
    from bad situations, e.g. too many messages queued or unexpected
    problem caused by a particular message.
*/
#define PBNTF_TRANS_OUTCOME_COMMON(pb, state)                                      \
    do {                                                                           \
        pubnub_t*       M_pb_     = (pb);                                          \
        enum pubnub_res M_pbrslt_ = M_pb_->core.last_result;                       \
        PUBNUB_LOG_INFO("Context %p Transaction outcome: %d\n", M_pb_, M_pbrslt_); \
        switch (M_pbrslt_) {                                                       \
        case PNR_FORMAT_ERROR:                                                     \
            PUBNUB_LOG_WARNING("Context %p Resetting time token\n", M_pb_);        \
            M_pb_->core.timetoken[0] = '0';                                        \
            M_pb_->core.timetoken[1] = '\0';                                       \
            break;                                                                 \
        default:                                                                   \
            break;                                                                 \
        }                                                                          \
        M_pb_->flags.is_via_post = false;                                          \
        ENTITY_API_PATCH_OR_DELETE_TO_FALSE(M_pb_->flags);                         \
        M_pb_->state = state;                                                      \
    } while (0)
