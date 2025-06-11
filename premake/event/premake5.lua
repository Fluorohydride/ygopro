project "event"
    kind "StaticLib"

    includedirs { "include", "compat" }

    files { "event.c", "evthread.c", "buffer.c", "bufferevent.c", "bufferevent_sock.c",
            "bufferevent_filter.c", "bufferevent_pair.c", "listener.c", "bufferevent_ratelim.c",
            "evmap.c", "log.c", "evutil.c", "evutil_rand.c", "strlcpy.c", "signal.c", 
            "event_tagging.c", "http.c", "evdns.c", "evrpc.c" }

    if os.isfile("evutil_time.c") then
        files { "evutil_time.c" }
    end

    filter "system:windows"
        prebuildcommands { "xcopy /E /Y $(ProjectDir)..\\event\\WIN32-Code $(ProjectDir)..\\event\\include" }
        files { "win32select.c", "evthread_win32.c", "buffer_iocp.c", "event_iocp.c", "bufferevent_async.c" }
        defines { "WIN32" } -- quirk of old libevent

    filter "system:linux"
        files { "evthread_pthread.c", "epoll.c", "epoll_sub.c", "poll.c", "select.c" }

    filter "system:macosx"
        files { "evthread_pthread.c", "kqueue.c", "poll.c", "select.c" }
