project "event"
    kind "StaticLib"

    includedirs { "include", "." }

    files {"event.c", "buffer.c", "bufferevent.c", "bufferevent_sock.c", "bufferevent_pair.c",
	       "listener.c", "evmap.c", "log.c", "evutil.c", "strlcpy.c", "signal.c", "bufferevent_filter.c",
		     "evthread.c", "bufferevent_ratelim.c", "evutil_rand.c" }
		files {"event_tagging.c", "http.c", "evdns.c", "evrpc.c"}

    configuration "windows"
        files {"win32select.c", "evthread_win32.c", "buffer_iocp.c", "event_iocp.c", "bufferevent_async.c"}
        includedirs { "compat" }
