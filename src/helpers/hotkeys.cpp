#include "helpers/hotkeys.h"
#include <set>

std::set<uint32_t> HotKeyManager::s_Keys;
uint8_t HotKeyManager::s_Mods;
HotKeyManager::HotKeyList HotKeyManager::s_HotKeys;

void HotKeyManager::AddHotKey(int mods, int key, const HotKeyCallback &func)
{
    HotKey hotkey = { mods, key };
    s_HotKeys.push_back(std::make_pair(hotkey, func));
}

void HotKeyManager::ProcessHotKeys()
{
    for (auto &[hotkey, func] : s_HotKeys)
    {
        if (s_Keys.count(hotkey.key) > 0 && hotkey.mod == s_Mods)
        {
            func();
            ClearKeys();
            break;
        }
    }
}
