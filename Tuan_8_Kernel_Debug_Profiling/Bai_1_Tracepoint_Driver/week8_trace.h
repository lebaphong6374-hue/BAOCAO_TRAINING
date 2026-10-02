#undef TRACE_SYSTEM
#define TRACE_SYSTEM week8_driver

#if !defined(_WEEK8_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _WEEK8_TRACE_H

#include <linux/tracepoint.h>

/*
 * Tracepoint: week8_write
 *
 * Ghi lại số byte được ghi vào driver.
 */
TRACE_EVENT(week8_write,

    TP_PROTO(size_t len),

    TP_ARGS(len),

    TP_STRUCT__entry(
        __field(size_t, len)
    ),

    TP_fast_assign(
        __entry->len = len;
    ),

    TP_printk("len=%zu", __entry->len)
);

#endif /* _WEEK8_TRACE_H */


/*
 * Thông tin để Kernel tìm lại file header
 * khi generate tracepoint code.
 */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .

#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE week8_trace

#include <trace/define_trace.h>
