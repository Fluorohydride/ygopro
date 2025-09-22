project "event"
    kind "StaticLib"

    local EVENT_VERSION = (io.readfile("configure") or ""):match("NUMERIC_VERSION%s+(0x%x+)")
    if not EVENT_VERSION then
        print("Warning: Could not determine libevent version from the configure file, assuming 2.1.12.")
        EVENT_VERSION = "0x02010c00" -- 2.1.12
    end
    EVENT_VERSION = tonumber(EVENT_VERSION)
    if EVENT_VERSION>=0x02020000 then
        print("Warning: Using libevent version 2.2.x is not supported, please use 2.1.x, otherwise you may encounter issues.")
    end

    includedirs { "include", "compat" }

    files { "event.c", "evthread.c", "buffer.c", "bufferevent.c", "bufferevent_sock.c",
            "bufferevent_filter.c", "bufferevent_pair.c", "listener.c", "bufferevent_ratelim.c",
            "evmap.c", "log.c", "evutil.c", "evutil_rand.c", "strlcpy.c", "signal.c", 
            "event_tagging.c", "http.c", "evdns.c", "evrpc.c" }

    if EVENT_VERSION>=0x02010000 then
        files { "evutil_time.c" }
    end

    filter "system:windows"
        prebuildcommands { "xcopy /E /Y $(ProjectDir)..\\event\\WIN32-Code $(ProjectDir)..\\event\\include" }
        files { "win32select.c", "evthread_win32.c", "buffer_iocp.c", "event_iocp.c", "bufferevent_async.c" }
        if EVENT_VERSION>=0x02010000 then
            prebuildcommands { "xcopy /E /Y $(ProjectDir)..\\event\\WIN32-Code\\nmake $(ProjectDir)..\\event\\include" }
            defines { "UINT32_MAX=0xffffffffU" } -- quirk of libevent 2.1
        end
        if EVENT_VERSION<0x02010000 then
            defines { "WIN32" } -- quirk of old libevent
        end

    filter "system:linux"
        files { "evthread_pthread.c", "epoll.c", "epoll_sub.c", "poll.c", "select.c" }

    filter "system:macosx"
        files { "evthread_pthread.c", "kqueue.c", "poll.c", "select.c" }
