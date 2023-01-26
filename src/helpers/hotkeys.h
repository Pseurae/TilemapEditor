#pragma once

#include <functional>
#include <list>
#include <utility>
#include <set>
#include <vector>

struct HotKey
{
    int mod;
    int key;
};

class HotKeyManager
{
public:
    using HotKeyCallback = std::function<void()>;
    using HotKeyList = std::list<std::pair<HotKey, HotKeyCallback>>;

    static void AddHotKey(int mod, int key, const HotKeyCallback &func);
    static void ProcessHotKeys();

    static void SetMods(int mods) { s_Mods = mods; }

    static void AddKey(uint32_t key) { s_Keys.insert(key); }

    static void ClearKeys() 
    { 
        s_Mods = 0;
        s_Keys.clear();
    }
private:
    static std::set<uint32_t> s_Keys;
    static uint8_t s_Mods;
    static HotKeyList s_HotKeys;
};
