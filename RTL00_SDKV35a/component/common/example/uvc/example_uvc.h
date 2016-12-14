#ifndef EXAMPLE_UVC_H
#define EXAMPLE_UVC_H

#include <platform/platform_stdlib.h>
#include "platform_opts.h"
#include "dlist.h"
#include "osdep_api.h"

#define UVC_RTSP_EN     1

//example structure to handle rtp_object operation in queue
struct rtp_payload_queue
{
    struct list_head wait_queue;
    _Mutex wait_mutex;
    struct list_head done_queue;
    _Mutex done_mutex;
    _Sema wait_sema; //up it whenever a rtp payload queue in wait_queue
    _Sema done_sema; //up it whenever a rtp payload queue in done_queue
    int flush_err;
};

void example_uvc(void);

#endif /* EXAMPLE_UVC_H */