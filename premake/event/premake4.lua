project "event"
    kind "StaticLib"

    includedirs { "include", "compat" }

    files { "event.c", "evthread.c", "buffer.c", "bufferevent.c", "bufferevent_sock.c",
            "bufferevent_filter.c", "bufferevent_pair.c", "listener.c", "bufferevent_ratelim.c",
            "evmap.c", "log.c", "evutil.c", "evutil_rand.c", "strlcpy.c", "signal.c", 
            "event_tagging.c", "http.c", "evdns.c", "evrpc.c" }

    configuration "windows"
        files { "win32select.c", "evthread_win32.c", "buffer_iocp.c", "event_iocp.c", "bufferevent_async.c" }
