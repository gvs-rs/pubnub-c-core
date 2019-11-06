/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_assert.h"

#include <stddef.h>

struct pubnub_;

/** A pubnub context. An opaque data structure that holds all the data
    needed for a context.
 */
typedef struct pubnub_ pubnub_t;

int pubnub_enable_auto_hearbeat(pubnub_t* pb, size_t period_sec)
{
    PUBNUB_UNUSED(pb);
    return 0;
}

int pubnub_set_heatrbeat_period(pubnub_t* pb, size_t period_sec)
{
    PUBNUB_UNUSED(pb);
    return 0;
}

void pubnub_disable_auto_hearbeat(pubnub_t* pb)
{
    PUBNUB_UNUSED(pb);
}

bool pubnub_is_auto_hertbeat_enabled(pubnub_t* pb)
{
    PUBNUB_UNUSED(pb);
    return false;
}

int pubnub_heartbeat_free_thumpers(void)
{
    return 0;
}

int pbauto_heartbeat_notify(pubnub_t* pb)
{
    PUBNUB_UNUSED(pb);
    return 0;
}

int pbauto_heartbeat_start_timer(pubnub_t* pb)
{
    PUBNUB_UNUSED(pb);
    return 0;
}

void pbauto_heartbeat_free_register(pubnub_t* pb)
{
    PUBNUB_UNUSED(pb);
}

int pbauto_heartbeat_form_channels_and_ch_groups(pubnub_t* pb,
                                                 char const** channel,
                                                 char const** channel_group)
{
    PUBNUB_UNUSED(pb);
    return 0;
}

void pbauto_heartbeat_stop(void)
{
    return;
}
