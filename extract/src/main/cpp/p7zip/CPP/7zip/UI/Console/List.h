// List.h

#ifndef __LIST_H
#define __LIST_H

#include <tools/CustomArchiveItem.h>
#include "../../../Common/Wildcard.h"

#include "../Common/LoadCodecs.h"
#include "../CPP/7zip/UI/Common/OpenArchive.h"
HRESULT ListArchives(CCodecs *codecs,
    const CObjectVector<COpenType> &types,
    const CIntVector &excludedFormats,
    bool stdInMode,
    UStringVector &archivePaths, UStringVector &archivePathsFull,
    bool processAltStreams, bool showAltStreams,
    const NWildcard::CCensorNode &wildcardCensor,
    bool enableHeaders, bool techMode,
    #ifndef _NO_CRYPTO
    bool &passwordEnabled, UString &password,
    #endif
    #ifndef _SFX
    const CObjectVector<CProperty> *props,
    #endif
    UInt64 &errors,
    UInt64 &numWarnings);


typedef CObjectVector<CustomArchiveItem> CustomArchiveItemList ;
HRESULT ListArchives2(CCodecs *codecs,
                     const CObjectVector<COpenType> &types,
                     const CIntVector &excludedFormats,
                     bool stdInMode,
                     UStringVector &archivePaths, UStringVector &archivePathsFull,
                     bool processAltStreams, bool showAltStreams,
                     const NWildcard::CCensorNode &wildcardCensor,
                     bool enableHeaders, bool techMode,
#ifndef _NO_CRYPTO
                     bool &passwordEnabled, UString &password,
#endif
#ifndef _SFX
                     const CObjectVector<CProperty> *props,
#endif
                     UInt64 &errors,
                     UInt64 &numWarnings,
                      CustomArchiveItemList& listData);

#endif
