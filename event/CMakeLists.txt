project (event C)
set (SRC_FILES1 event.c buffer.c bufferevent.c bufferevent_sock.c bufferevent_pair.c
                listener.c evmap.c log.c evutil.c strlcpy.c signal.c bufferevent_filter.c
                evthread.c bufferevent_ratelim.c evutil_rand.c event_tagging.c http.c
                evdns.c evrpc.c)
set (INC_FILES1 bufferevent-internal.h defer-internal.h evbuffer-internal.h evdns.h evdns.h
                event.h event-internal.h evhttp.h evthread-internal.h evutil.h ht-internal.h
                http-internal.h iocp-internal.h ipv6-internal.h log-internal.h 
                minheap-internal.h mm-internal.h ratelim-internal.h strlcpy-internal.h
                util-internal.h)

FILE (GLOB INC_FILES2 include/event2/*.h)

if (MSVC)
    list (APPEND SRC_FILES1 win32select.c evthread_win32.c buffer_iocp.c event_iocp.c
                           bufferevent_async.c)
    include_directories ( "compat" )
endif ()
source_group(src FILES ${SRC_FILES1} ${INC_FILES1})
source_group(src\\include\\event2 FILES ${INC_FILES2})

include_directories ( "include" "." )

add_library (event STATIC ${SRC_FILES1} ${INC_FILES1} ${INC_FILES2})
