//
// Created by JackyHieu1211 on 16/07/2022.
//

#ifndef ANDROIDP7ZIP_CINFO_H
#define ANDROIDP7ZIP_CINFO_H
#include "../p7zip/CPP/Common/MyString.h"
#include "../p7zip/CPP/Common/MyVector.h"
#include "../p7zip/C/7zTypes.h"

struct CInfo
{
    bool SolidIsSpecified;
    bool MultiThreadIsAllowed;
    UInt64 SolidBlockSize;
    UInt32 NumThreads;

    CRecordVector<UInt64> VolumeSizes;

    UInt32 Level;
    UString Method;
    UInt32 Dictionary;
    bool OrderMode;
    UInt32 Order;
    UString Options;

    UString EncryptionMethod;

    bool SFXMode;
    bool OpenShareForWrite;


    UString ArchiveName; // in: Relative for ; out: abs
    UString CurrentDirPrefix;
    bool KeepName;

    bool GetFullPathName(UString &result) const;

    int FormatIndex;

    UString Password;
    bool EncryptHeadersIsAllowed;
    bool EncryptHeaders;

    void Init()
    {
        Level = Dictionary = Order = UInt32(-1);
        OrderMode = false;
        Method.Empty();
        Options.Empty();
        EncryptionMethod.Empty();
    }
    CInfo()
    {
        Init();
    }
};


#endif //ANDROIDP7ZIP_CINFO_H
