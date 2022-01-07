#include <iostream>

#include <csignal>
#include <event.h>

void signal_cb(int fd, short event, void* argc){
    auto *base = static_cast<event_base*>(argc);
    struct timeval delay{2, 0};
    std::cout << "caught an interrupt signal; exiting cleanly in two seconds...\n";
    event_base_loopexit(base, &delay);
}

void timeout_cb(int fd, short event, void* argc){
    std::cout << "timeout\n";
}

int main() {
    struct event_base* base = event_init();
    struct event* signal_event = evsignal_new(base, SIGINT, signal_cb, base);
    event_add(signal_event, nullptr);
    timeval tv{1, 0};
    struct event* timeout_event = evtimer_new(base, timeout_cb, nullptr);
    event_add(timeout_event, &tv);
    event_base_dispatch(base);
    event_free(timeout_event);
    event_free(signal_event);
    event_base_free(base);
    return 0;
}