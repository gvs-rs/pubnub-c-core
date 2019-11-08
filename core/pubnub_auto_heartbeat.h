/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PBAUTO_HEARTBEAT
#define INC_PBAUTO_HEARTBEAT

#if PUBNUB_USE_AUTO_HEARTBEAT
/** Maximum number of auto heartbeat thumpers that can be used at one time */
#define PUBNUB_MAX_HEARTBEAT_THUMPERS 16
#define UNASSIGNED PUBNUB_MAX_HEARTBEAT_THUMPERS

/** Pubnub context fields for saving subscribed channels, channel groups
    and heartbeat thumper context index used by the module for keeping presence.
  */
#define M_channel_ch_group_n_thumperIndex()                   \
    struct {                                                  \
        char* channel;                                        \
        char* channel_group;                                  \
        unsigned thumperIndex;                                \
    } autoRegister;

/** Enables keeping presence on subscribed channels and channel groups.
    Initially auto heartbeat on @p pb context is disabled.
    This module keeps presence by performing pubnub_heartbeat() transaction periodicaly
    with uuid given whenever subscription on @p pb context is not in progress and
    auto heartbeat is not disabled. This process is independent from anything user
    may by doing with the context when its not subscribing.
    If the uuid(or any other relevant data, like dns server, or proxy) is changed at
    some point, the module will update it automatically in its heartbeats.
    The same goes if the pubnub context leaves some of the channels, or channel groups.
    @param pb The pubnub context. Can't be NULL
    @param period_sec Auto heartbeat thumping period
    @return 0 on success, -1 otherwise
  */
int pubnub_enable_auto_heartbeat(pubnub_t* pb, size_t period_sec);

/** Sets(changes) auto heartbeat thumping period.
    @param pb The pubnub context. Can't be NULL
    @param period_sec Auto heartbeat thumping period
    @return 0 on success, -1 otherwise
  */
int pubnub_set_heartbeat_period(pubnub_t* pb, size_t period_sec);

/** Desables auto heartbeat on the @p pb context.
  */
void pubnub_disable_auto_heartbeat(pubnub_t* pb);

/** Tells if auto heartbeat is enabled on the @p pb context.
  */
bool pubnub_is_auto_heartbeat_enabled(pubnub_t* pb);

/** Releases all allocated heartbeat thumpers.
    @return 0 on success, -1 otherwise
  */
int pubnub_heartbeat_free_thumpers(void);

/** Gives notice to auto heartbeat module that subscribe transaction has begun */
int pbauto_heartbeat_notify(pubnub_t* pb);

/** Commences auto heartbeat timer, if auto heartbeat is enabled and when subscribe transaction
    is acomplished.
  */
int pbauto_heartbeat_start_timer(pubnub_t* pb);

/** Releases allocated strings for subscribed channels and channel groups
  */
void pbauto_heartbeat_free_info(pubnub_t* pb);

/** Saves channels and channel groups, or if both NULL reads saved from the context.
  */
enum pubnub_res pbauto_heartbeat_form_channels_and_ch_groups(pubnub_t* pb,
                                                             char const** channel,
                                                             char const** channel_group);

/** Stops auto heartbeat thread */
void pbauto_heartbeat_stop(void);

#else
#define M_channel_ch_group_n_thumperIndex()
#define pubnub_enable_auto_heartbeat(pb, period_sec) -1
#define pubnub_set_heartbeat_period(pb, period_sec) -1
#define pubnub_disable_auto_heartbeat(pb)
#define pubnub_is_auto_heartbeat_enabled(pb) false
#define pubnub_heartbeat_free_thumpers() 0
#define pbauto_heartbeat_notify(pb)
#define pbauto_heartbeat_start_timer(pb)
#define pbauto_heartbeat_free_info(pb)
#define pbauto_heartbeat_form_channels_and_ch_groups(pb, addr_channel, addr_channel_group) PNR_OK
#define pbauto_heartbeat_stop()
#endif /* PUBNUB_USE_AUTO_HEARTBEAT */

#endif /* !defined INC_PBAUTO_HEARTBEAT */

