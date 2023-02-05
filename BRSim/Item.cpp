#include "Item.h"
#include "Agent.h"

Item::Item(std::string textureName, ItemType itemType)
	:itemType(itemType)
{
	char fileName[128];
	sprintf_s(fileName, "Data/Textures/%s", textureName.c_str());
	pickupTexture.loadFromPNG(fileName);
}