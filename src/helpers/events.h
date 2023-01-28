#pragma once

#include "helpers/fs.h"
#include <functional>
#include <list>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

struct EventFnBase
{};

template<typename ...Args>
struct EventFn : public EventFnBase
{
    using CallbackFn = std::function<void(Args...)>;
    EventFn(CallbackFn &fn) : callback(std::move(fn)) {}

    void operator() (Args... args)
    {
        this->callback(args...);
    }
    CallbackFn callback;
};

template <typename T>
concept EventConcept = std::is_base_of<EventFnBase, T>::value;

class EventManager
{
public:
    using EventList = std::list<std::tuple<uintmax_t, void *, EventFnBase *>>;

    template<EventConcept E>
    static void subscribe(void *token, typename E::CallbackFn fn)
    {
        s_Events.push_back(std::make_tuple(typeid(E).hash_code(), token, new E(fn)));
    }

    template<EventConcept E>
    static void subscribe(typename E::CallbackFn fn)
    {
        subscribe<E>(nullptr, fn);
    }

    template<EventConcept E>
    static void unsubscribe(void *token)
    {
        auto iter = std::find_if(s_Events.begin(), s_Events.end(), [&](auto &item) {
            return item[1] == token && item[0] == typeid(E).hash_code();
        });

        if (iter != s_Events.end()) {
            s_Events.erase(iter);
        }
    }

    template<EventConcept E>
    static void unsubscribe()
    {
        unsubscribe<E>(nullptr);
    }

    template<EventConcept E, typename ...Args>
    static void publish(Args&&... args)
    {
        for (auto &[id, token, ev] : s_Events)
        {
            E *event = static_cast<E *>(ev);
            if (typeid(*event).hash_code() == id)
                (*event)(args...);
        }
    }
private:
    static EventList s_Events;
};

#define EVENT_DEF(name, ...) \
    struct name final : EventFn<__VA_ARGS__> \
    { \
        explicit name(CallbackFn fn) : EventFn(fn) {} \
    }

EVENT_DEF(RequestNewTilemap, int, int);
EVENT_DEF(RequestNewTilemapWindow);
EVENT_DEF(RequestOpenTilemap, const fs::path &);
EVENT_DEF(RequestOpenRecentTilemap, const fs::path &);
EVENT_DEF(RequestOpenTilemapWindow);
EVENT_DEF(RequestOpenTileset, const fs::path &);
EVENT_DEF(RequestOpenTilesetWindow);
EVENT_DEF(RequestSaveTilemap, bool);
EVENT_DEF(RequestOpenPopup, const char *);
EVENT_DEF(RequestProgramQuit, bool);
EVENT_DEF(RequestUndo);
EVENT_DEF(RequestRedo);

