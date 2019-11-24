/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "core/pb_sleep_ms.h"

void pb_sleep_ms(DWORD dwMilliseconds)
{
    Sleep(dwMilliseconds);
}
