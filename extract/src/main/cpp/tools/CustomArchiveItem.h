// CustomArchiveItem.h

#ifndef __CUSTOM_ARCHIVE_ITEM_H
#define __CUSTOM_ARCHIVE_ITEM_H

#include "../p7zip/CPP/Common/MyString.h"
#include "../p7zip/C/7zTypes.h"

struct CustomArchiveItem
{
  CSysString itemPath;
  CSysString time;
  UInt64 packSize, unpackSize;
  bool isFolder;
};

#endif
