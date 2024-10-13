#include "Item.hpp"

ItemSlot *OInventory::GetItemSlotByID(int id) {
    return *(ItemSlot **)((_QWORD)this + 8LL * id + 0x30);
}