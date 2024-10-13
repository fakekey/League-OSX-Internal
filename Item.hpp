#pragma once
#ifndef Item_h
#define Item_h

#include "XRayDef.hpp"

class ItemInfo {
  public:
    union {
        DEFINE_MEMBER_N(_DWORD ItemID, 0x9C);
    };
};

class OItem {
  public:
    union {
        DEFINE_MEMBER_N(ItemInfo *Info, 0x30);
    };
};

class ItemSlot {
  public:
    union {
        DEFINE_MEMBER_N(OItem *Item, 0x10);
    };
};

class OInventory {
  public:
    ItemSlot *GetItemSlotByID(int id);
};

#endif /* Item_h */