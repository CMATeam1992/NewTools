//#include "myWindows/StdAfx.h"
#include "StdAfx.h"
#include "vn_cma_extract_Archive.h"
#include <android/log.h>
#include <tools/methodids.h>
#include <7zip/UI/Console/UpdateCallbackConsole.h>
#include <Windows/ErrorMsg.h>
#include <7zip/Common/RegisterCodec.h>
#include <7zip/UI/Common/ExitCode.h>
#include <7zip/UI/Common/ArchiveCommandLine.h>
#include <7zip/UI/Console/ExtractCallbackConsole.h>
#include <7zip/UI/Console/HashCon.h>
#include <7zip/UI/Console/List.h>
//#include "RegisterCodec.h"
#include "Common/StdOutStream.h"
//#include "OpenCallbackConsole.h"
//#include "CreateCoder.h"
#include "../Common/MyString.h"

#ifdef _WIN32
#include <Psapi.h>
#else

#include "myPrivate.h"

#endif

/***********************************************************************************************
 *                          MethodIds of ExtractCallBack
 *************************************************************************************************/


jmethodID beforeOpen, extractResult, openResult, thereAreNoFiles, setPassword;
jmethodID askWrite, setCurrentFilePath, showMessage, setNumFiles;
jmethodID setRatioInfo;
jmethodID askOverwrite, prepareOperation, messageError, exAddErrorMessage, setOperationResult;
jmethodID cryptoGetTextPassword;
jmethodID setTotal, setCompleted;
jmethodID open_CheckBreak, open_SetTotal, open_SetCompleted, open_CryptoGetTextPassword,
        open_GetPasswordIfAny, open_WasPasswordAsked, open_ClearPasswordWasAskedFlag;

/*********************************************************************************************
 *                           MethodIds of UpdateCallback
 **********************************************************************************************/
jmethodID startArchive, checkBreak, scanProgress, updateSetNumFiles, updateSetTotal,
        updateSetCompleted, updateSetRatioInfo, getStream, updateSetOperationResult,
        openCheckBreak, openSetCompleted, addErrorMessage;

/**********************************************************************************************
 *                             MethodIds for ArchiveItemsList
 **********************************************************************************************/
jmethodID archiveItemsList_addItem;

using namespace NWindows;


#if defined( _7ZIP_LARGE_PAGES)
#include "../../../../C/Alloc.h"
#endif

//#include "Common/MyInitGuid.h"

#include "Common/CommandLineParser.h"
#include "Common/IntToString.h"
#include "Common/MyException.h"
#include "Common/StdOutStream.h"
#include "Common/StringConvert.h"
#include "Common/StringToInt.h"

//#include "Windows/Error.h"

#ifdef _WIN32
#include "Windows/MemoryLock.h"
#endif


//#include "ArchiveCommandLine.h"
//#include "ExitCode.h"
//#include "Extract.h"

#ifdef EXTERNAL_CODECS

#include "LoadCodecs.h"

#endif

//#include "BenchCon.h"
//#include "ExtractCallbackConsole.h"
//#include "List.h"
//#include "OpenCallbackConsole.h"
//#include "UpdateCallbackConsole.h"
//
//#include "MyVersion.h"

#include "Windows/System.h"
#include "tools/CInfo.h"
#include "ndkhelper.h"

//#include "ExtractGUI.h"
//#include "UpdateGUI.h"

Environment environment;


using namespace NWindows;
using namespace NFile;
using namespace NCommandLineParser;

//static const char *kCopyrightString = "\n7-Zip"
//                                      #ifndef EXTERNAL_CODECS
//                                      " (A)"
//                                      #endif
//
//                                      #ifdef _WIN64
//                                      " [64]"
//                                      #endif
//
//                                      " " MY_VERSION_COPYRIGHT_DATE "\n"
//                                      "p7zip Version " P7ZIP_VERSION;

static const char *kHelpString =
        "\nUsage: 7z"
        #ifdef _NO_CRYPTO
        "r"
        #else
        #ifndef EXTERNAL_CODECS
        "a"
        #endif
        #endif
        " <command> [<switches>...] <archive_name> [<file_names>...]\n"
        "       [<@listfiles...>]\n"
        "\n"
        "<Commands>\n"
        "  a: Add files to archive\n"
        "  b: Benchmark\n"
        "  d: Delete files from archive\n"
        "  e: Extract files from archive (without using directory names)\n"
        "  l: List contents of archive\n"
        //    "  l[a|t][f]: List contents of archive\n"
        //    "    a - with Additional fields\n"
        //    "    t - with all fields\n"
        //    "    f - with Full pathnames\n"
        "  t: Test integrity of archive\n"
        "  u: Update files to archive\n"
        "  x: eXtract files with full paths\n"
        "<Switches>\n"
        "  -ai[r[-|0]]{@listfile|!wildcard}: Include archives\n"
        "  -ax[r[-|0]]{@listfile|!wildcard}: eXclude archives\n"
        "  -bd: Disable percentage indicator\n"
        "  -i[r[-|0]]{@listfile|!wildcard}: Include filenames\n"
        "  -m{Parameters}: set compression Method\n"
        "  -o{Directory}: set Output directory\n"
        #ifndef _NO_CRYPTO
        "  -p{Password}: set Password\n"
        #endif
        "  -r[-|0]: Recurse subdirectories\n"
        "  -scs{UTF-8 | WIN | DOS}: set charset for list files\n"
        "  -sfx[{name}]: Create SFX archive\n"
        "  -si[{name}]: read data from stdin\n"
        "  -slt: show technical information for l (List) command\n"
        "  -so: write data to stdout\n"
        "  -ssc[-]: set sensitive case mode\n"
        "  -t{Type}: Set type of archive\n"
        "  -u[-][p#][q#][r#][x#][y#][z#][!newArchiveName]: Update options\n"
        "  -v{Size}[b|k|m|g]: Create volumes\n"
        "  -w[{path}]: assign Work directory. Empty path means a temporary directory\n"
        "  -x[r[-|0]]]{@listfile|!wildcard}: eXclude filenames\n"
        "  -y: assume Yes on all queries\n";

// ---------------------------
// exception messages

static const char *kEverythingIsOk = "Everything is Ok";
static const char *kUserErrorMessage = "Incorrect command line";
static const char *kNoFormats = "7-Zip cannot find the code that works with archives.";
static const char *kUnsupportedArcTypeMessage = "Unsupported archive type";

static const wchar_t *kDefaultSfxModule = L"7zCon.sfx";


static void ShowCopyrightAndHelp(bool needHelp) {
    int nbcpu = NWindows::NSystem::GetNumberOfProcessors();
    /*LOGI(kCopyrightString);
    LOGI(" (locale=%s ,Utf16=%s ,HugeFiles=%s %d %s",my_getlocale()
            ,global_use_utf16_conversion?"on":"off"
                    ,(sizeof(off_t) >= 8)?"on,":"off,",nbcpu,(nbcpu > 1)?" CPUs":" CPU");*/
    /*s << kCopyrightString << " (locale=" << my_getlocale() <<",Utf16=";
    if (global_use_utf16_conversion) s << "on";
    else                             s << "off";
    s << ",HugeFiles=";
    if (sizeof(off_t) >= 8) s << "on,";
    else                    s << "off,";

    if (nbcpu > 1) s << nbcpu << " CPUs)\n";
    else           s << nbcpu << " CPU)\n";*/

    /*if (needHelp)
        LOGI(kHelpString);*/
    // s << kHelpString;
}

#ifdef EXTERNAL_CODECS

static void PrintString(const AString &s, int size) {
 //   int len = s.Length();
   // LOGI("%s", (const char *) &s);
    //stdStream << s;
    // for (int i = len; i < size; i++)
    // LOGI(' ');
    //stdStream << ' ';
}

#endif

static void PrintString(const UString &s, int size) {
    //  int len = s.Length();
    //LOGI("%s", (LPCSTR) GetOemString(s));
    /* stdStream << s;
     for (int i = len; i < size; i++)
       stdStream << ' ';*/
}

static void PrintWarningsPaths(const CErrorPathCodes &pc, CStdOutStream &so) {
    FOR_VECTOR(i, pc.Paths) {
        so << pc.Paths[i] << " : ";
        so << NError::MyFormatMessage(pc.Codes[i]) << endl;
    }
    so << "----------------" << endl;
}

static int WarningsCheck(HRESULT result, const CCallbackConsoleBase &callback,
                         const CUpdateErrorInfo &errorInfo,
                         CStdOutStream *so,
                         CStdOutStream *se,
                         bool showHeaders) {
    int exitCode = NExitCode::kSuccess;

    if (callback.ScanErrors.Paths.Size() != 0) {
        if (se) {
            *se << endl;
            *se << "Scan WARNINGS for files and folders:" << endl << endl;
            PrintWarningsPaths(callback.ScanErrors, *se);
            *se << "Scan WARNINGS: " << callback.ScanErrors.Paths.Size();
            *se << endl;
        }
        exitCode = NExitCode::kWarning;
    }

    if (result != S_OK || errorInfo.ThereIsError()) {
        if (se) {
            UString message;
            if (!errorInfo.Message.IsEmpty()) {
                message.AddAscii(errorInfo.Message);
                message.Add_LF();
            }
            {
                FOR_VECTOR(i, errorInfo.FileNames) {
                    message += fs2us(errorInfo.FileNames[i]);
                    message.Add_LF();
                }
            }
            if (errorInfo.SystemError != 0) {
                message += NError::MyFormatMessage(errorInfo.SystemError);
                message.Add_LF();
            }
            if (!message.IsEmpty())
                *se << L"\nError:\n" << message;
        }

        // we will work with (result) later
        // throw CSystemException(result);
        return NExitCode::kFatalError;
    }

    unsigned numErrors = callback.FailedFiles.Paths.Size();
    if (numErrors == 0) {
        if (showHeaders)
            if (callback.ScanErrors.Paths.Size() == 0)
                if (so) {
                    if (se)
                        se->Flush();
                    *so << kEverythingIsOk << endl;
                }
    } else {
        if (se) {
            *se << endl;
            *se << "WARNINGS for files:" << endl << endl;
            PrintWarningsPaths(callback.FailedFiles, *se);
            *se << "WARNING: Cannot open " << numErrors << " file";
            if (numErrors > 1)
                *se << 's';
            *se << endl;
        }
        exitCode = NExitCode::kWarning;
    }

    return exitCode;
}

static void PrintString(CStdOutStream &so, const UString &s, unsigned size) {
    unsigned len = s.Len();
    so << s;
    for (unsigned i = len; i < size; i++)
        so << ' ';
}

static void PrintStringRight(CStdOutStream &so, const AString &s, unsigned size) {
    unsigned len = s.Len();
    for (unsigned i = len; i < size; i++)
        so << ' ';
    so << s;
}

static void PrintUInt32(CStdOutStream &so, UInt32 val, unsigned size) {
    char s[16];
    ConvertUInt32ToString(val, s);
    PrintStringRight(so, s, size);
}

static void PrintLibIndex(CStdOutStream &so, int libIndex) {
    if (libIndex >= 0)
        PrintUInt32(so, libIndex, 2);
    else
        so << "  ";
    so << ' ';
}

static void PrintHexId(CStdOutStream &so, UInt64 id) {
    char s[32];
    ConvertUInt64ToHex(id, s);
    PrintStringRight(so, s, 8);
}

static inline char GetHex(Byte value) {

    return (char) ((value < 10) ? ('0' + value) : ('A' + (value - 10)));
}

extern CStdOutStream *g_StdStream;
extern CStdOutStream *g_ErrStream;
extern unsigned g_NumCodecs;
extern const CCodecInfo *g_Codecs[];

extern unsigned g_NumHashers;
extern const CHasherInfo *g_Hashers[];


#ifdef __cplusplus
extern "C" {
#endif
int ProcessCommand(int numArgs, const char *args[], Environment &env) {


#if defined(_WIN32) && !defined(UNDER_CE)
    SetFileApisToOEM();
#endif

    UStringVector commandStrings;

#ifdef _WIN32
    NCommandLineParser::SplitCommandLine(GetCommandLineW(), commandStrings);
#else
    // GetArguments(numArgs, args, commandStrings);
    mySplitCommandLine(numArgs, const_cast<char **>(args), commandStrings);
#endif

    if (commandStrings.Size() == 1) {
        ShowCopyrightAndHelp(true);
        return 0;
    }
    commandStrings.Delete(0);

    CArcCmdLineOptions options;

    CArcCmdLineParser parser;

    parser.Parse1(commandStrings, options);

    if (options.HelpMode) {
        ShowCopyrightAndHelp(true);
        return 0;
    }
#if defined(_7ZIP_LARGE_PAGES)
    if (options.LargePages)
    {
      SetLargePageSize();
#ifdef _WIN32
      NSecurity::EnableLockMemoryPrivilege();
#endif
    }
#endif

    //CStdOutStream &stdStream = options.StdOutMode ? g_StdErr : g_StdOut;
    //g_StdStream = &stdStream;

    if (options.EnableHeaders)
        ShowCopyrightAndHelp(false);

    parser.Parse2(options);

    CCodecs *codecs = new CCodecs;
    CMyComPtr<
#ifdef EXTERNAL_CODECS
            ICompressCodecsInfo
#else
            IUnknown
#endif
    > compressCodecsInfo = codecs;
    HRESULT result = codecs->Load();
    if (result != S_OK) {
        //LOGE("Error Loading 7z Codecs");
        return -1;
    }
    //throw CSystemException(result);


    bool isExtractGroupCommand = options.Command.IsFromExtractGroup();
    if (codecs->Formats.Size() == 0 &&
        (isExtractGroupCommand ||
         options.Command.CommandType == NCommandType::kList ||
         options.Command.IsFromUpdateGroup())) {
        // LOGE("%s", kNoFormats);
        return -1;
        //throw kNoFormats;
    }
    CIntVector formatIndices;
    if (!codecs->FindFormatForArchiveType(options.ArcType, formatIndices)) {
        //LOGE("%s", kUnsupportedArcTypeMessage);
        return -2;
        //throw kUnsupportedArcTypeMessage;
    }

    CObjectVector<COpenType> types;
    if (!ParseOpenTypes(*codecs, options.ArcType, types))
        throw kUnsupportedArcTypeMessage;

    CIntVector excludedFormats;
    FOR_VECTOR (k, options.ExcludedArcTypes) {
        CIntVector tempIndices;
        if (!codecs->FindFormatForArchiveType(options.ExcludedArcTypes[k], tempIndices)
            || tempIndices.Size() != 1)
            throw kUnsupportedArcTypeMessage;
        excludedFormats.AddToUniqueSorted(tempIndices[0]);
        // excludedFormats.Sort();
    }

    HRESULT hresultMain = S_OK;
    int retCode = NExitCode::kSuccess;
    unsigned percentsNameLevel = 1;
    if (options.LogLevel == 0 || options.Number_for_Percents != options.Number_for_Out)
        percentsNameLevel = 2;

    unsigned consoleWidth = 80; // FIXME

    CStdOutStream *percentsStream = nullptr;
    if (options.Number_for_Percents != k_OutStream_disabled)
        percentsStream = (options.Number_for_Percents == k_OutStream_stderr) ? &g_StdErr
                                                                             : &g_StdOut;


    if (options.Command.CommandType == NCommandType::kInfo) {
        CStdOutStream &so = (g_StdStream ? *g_StdStream : g_StdOut);
        unsigned i;
#ifdef EXTERNAL_CODECS
        so << endl << "Libs:" << endl;
        for (i = 0; i < codecs->Libs.Size(); i++)
        {
           // PrintLibIndex(so, i);
            so << ' ' << codecs->Libs[i].Path << endl;
        }
#endif

        so << endl << "Formats:" << endl;

        const char *kArcFlags = "KSNFMGOPBELH";
        const unsigned kNumArcFlags = (unsigned) strlen(kArcFlags);

        for (i = 0; i < codecs->Formats.Size(); i++) {
            const CArcInfoEx &arc = codecs->Formats[i];

#ifdef EXTERNAL_CODECS
            PrintLibIndex(so, arc.LibIndex);
#else
            so << "  ";
#endif

            so << (char) (arc.UpdateEnabled ? 'C' : ' ');

            for (unsigned b = 0; b < kNumArcFlags; b++) {
                so << (char)
                        ((arc.Flags & ((UInt32) 1 << b)) != 0 ? kArcFlags[b] : ' ');
            }

            so << ' ';
            PrintString(so, arc.Name, 8);
            so << ' ';
            UString s;

            FOR_VECTOR (t, arc.Exts) {
                if (t != 0)
                    s.Add_Space();
                const CArcExtInfo &ext = arc.Exts[t];
                s += ext.Ext;
                if (!ext.AddExt.IsEmpty()) {
                    s += L" (";
                    s += ext.AddExt;
                    s += L')';
                }
            }

            PrintString(so, s, 13);
            so << ' ';

            if (arc.SignatureOffset != 0)
                so << "offset=" << arc.SignatureOffset << ' ';

            FOR_VECTOR(si, arc.Signatures) {
                if (si != 0)
                    so << "  ||  ";

                const CByteBuffer &sig = arc.Signatures[si];

                for (size_t j = 0; j < sig.Size(); j++) {
                    if (j != 0)
                        so << ' ';
                    Byte b = sig[j];
                    if (b > 0x20 && b < 0x80) {
                        so << (char) b;
                    } else {
                        so << GetHex((b >> 4) & 0xF);
                        so << GetHex(b & 0xF);
                    }
                }
            }
            so << endl;
        }

        so << endl << "Codecs:" << endl; //  << "Lib          ID Name" << endl;

        for (i = 0; i < g_NumCodecs; i++) {
            const CCodecInfo &cod = *g_Codecs[i];

            PrintLibIndex(so, -1);

            if (cod.NumStreams == 1)
                so << ' ';
            else
                so << cod.NumStreams;

            so << (char) (cod.CreateEncoder ? 'E' : ' ');
            so << (char) (cod.CreateDecoder ? 'D' : ' ');

            so << ' ';
            PrintHexId(so, cod.Id);
            so << ' ' << cod.Name << endl;
        }


#ifdef EXTERNAL_CODECS

        UInt32 numMethods;
        if (codecs->GetNumMethods(&numMethods) == S_OK)
            for (UInt32 j = 0; j < numMethods; j++)
            {
                PrintLibIndex(so, codecs->GetCodec_LibIndex(j));

                UInt32 numStreams = codecs->GetCodec_NumStreams(j);
                if (numStreams == 1)
                    so << ' ';
                else
                    so << numStreams;

                so << (char)(codecs->GetCodec_EncoderIsAssigned(j) ? 'E' : ' ');
                so << (char)(codecs->GetCodec_DecoderIsAssigned(j) ? 'D' : ' ');

                so << ' ';
                UInt64 id;
                HRESULT res = codecs->GetCodec_Id(j, id);
                if (res != S_OK)
                    id = (UInt64)(Int64)-1;
                PrintHexId(so, id);
                so << ' ' << codecs->GetCodec_Name(j) << endl;
            }

#endif


        so << endl << "Hashers:" << endl; //  << " L Size       ID Name" << endl;

        for (i = 0; i < g_NumHashers; i++) {
            const CHasherInfo &codec = *g_Hashers[i];
            PrintLibIndex(so, -1);
            PrintUInt32(so, codec.DigestSize, 4);
            so << ' ';
            PrintHexId(so, codec.Id);
            so << ' ' << codec.Name << endl;
        }

#ifdef EXTERNAL_CODECS

        numMethods = codecs->GetNumHashers();
        for (UInt32 j = 0; j < numMethods; j++)
        {
            PrintLibIndex(so, codecs->GetHasherLibIndex(j));
            PrintUInt32(so, codecs->GetHasherDigestSize(j), 4);
            so << ' ';
            PrintHexId(so, codecs->GetHasherId(j));
            so << ' ' << codecs->GetHasherName(j) << endl;
        }

#endif

    } else if (isExtractGroupCommand || options.Command.CommandType == NCommandType::kList) {


        UStringVector ArchivePathsSorted;
        UStringVector ArchivePathsFullSorted;

        if (options.StdInMode) {
            ArchivePathsSorted.Add(options.ArcName_for_StdInMode);
            ArchivePathsFullSorted.Add(options.ArcName_for_StdInMode);
        } else {
            CExtractScanConsole scan;
            scan.Init(options.EnableHeaders ? g_StdStream : nullptr, g_ErrStream, percentsStream);
            scan.SetWindowWidth(consoleWidth);

            if (g_StdStream && options.EnableHeaders)
                *g_StdStream << "Scanning the drive for archives:" << endl;

            CDirItemsStat st;

            scan.StartScanning();

            hresultMain = EnumerateDirItemsAndSort(
                    options.arcCensor,
                    NWildcard::k_RelatPath,
                    UString(), // addPathPrefix
                    ArchivePathsSorted,
                    ArchivePathsFullSorted,
                    st,
                    &scan);

            scan.CloseScanning();

            if (hresultMain == S_OK) {
                if (options.EnableHeaders)
                    scan.PrintStat(st);
            } else {
                /*
                if (res != E_ABORT)
                {
                  throw CSystemException(res);
                  // errorInfo.Message = "Scanning error";
                }
                return res;
                */
            }
        }

        if (hresultMain == S_OK)
            if (isExtractGroupCommand) {
                CExtractCallbackConsole *ecs = new CExtractCallbackConsole(env);
                //  CExtractCallbackImp *ecs = new CExtractCallbackImp(env);
                CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

#ifndef _NO_CRYPTO
                ecs->PasswordIsDefined = options.PasswordEnabled;
                ecs->Password = options.Password;
#endif

                ecs->Init(g_StdStream, g_ErrStream, percentsStream);
                ecs->MultiArcMode = (ArchivePathsSorted.Size() > 1);

                ecs->LogLevel = options.LogLevel;
                ecs->PercentsNameLevel = percentsNameLevel;

                if (percentsStream)
                    ecs->SetWindowWidth(consoleWidth);

                /*
                COpenCallbackConsole openCallback;
                openCallback.Init(g_StdStream, g_ErrStream);

                #ifndef _NO_CRYPTO
                openCallback.PasswordIsDefined = options.PasswordEnabled;
                openCallback.Password = options.Password;
                #endif
                */

                CExtractOptions eo;
                (CExtractOptionsBase &) eo = options.ExtractOptions;

                eo.StdInMode = options.StdInMode;
                eo.StdOutMode = options.StdOutMode;
                eo.YesToAll = options.YesToAll;
                eo.TestMode = options.Command.IsTestCommand();

#ifndef _SFX
                eo.Properties = options.Properties;
#endif

                UString errorMessage;
                CDecompressStat stat;
                CHashBundle hb;
                IHashCalc *hashCalc = NULL;

                if (!options.HashMethods.IsEmpty()) {
                    hashCalc = &hb;
                    //   ThrowException_if_Error(hb.SetMethods(EXTERNAL_CODECS_VARS_L options.HashMethods));
                    hb.Init();
                }

                hresultMain = Extract(
                        codecs,
                        types,
                        excludedFormats,
                        ArchivePathsSorted,
                        ArchivePathsFullSorted,
                        options.Censor.Pairs.Front().Head,
                        eo,
                        ecs,
                        ecs,
                        hashCalc,
                        errorMessage,
                        stat,
                        ecs);

                ecs->ClosePercents();

                if (!errorMessage.IsEmpty()) {
                    if (g_ErrStream)
                        *g_ErrStream << endl << "ERROR:" << endl << errorMessage << endl;
                    if (hresultMain == S_OK)
                        hresultMain = E_FAIL;
                }

                CStdOutStream *so = g_StdStream;

                bool isError = false;

                if (so) {
                    *so << endl;

                    if (ecs->NumTryArcs > 1) {
                        *so << "Archives: " << ecs->NumTryArcs << endl;
                        *so << "OK archives: " << ecs->NumOkArcs << endl;
                    }
                }

                if (ecs->NumCantOpenArcs != 0) {
                    isError = true;
                    if (so)
                        *so << "Can't open as archive: " << ecs->NumCantOpenArcs << endl;
                }

                if (ecs->NumArcsWithError != 0) {
                    isError = true;
                    if (so)
                        *so << "Archives with Errors: " << ecs->NumArcsWithError << endl;
                }

                if (so) {
                    if (ecs->NumArcsWithWarnings != 0)
                        *so << "Archives with Warnings: " << ecs->NumArcsWithWarnings << endl;

                    if (ecs->NumOpenArcWarnings != 0) {
                        *so << endl;
                        if (ecs->NumOpenArcWarnings != 0)
                            *so << "Warnings: " << ecs->NumOpenArcWarnings << endl;
                    }
                }

                if (ecs->NumOpenArcErrors != 0) {
                    isError = true;
                    if (so) {
                        *so << endl;
                        if (ecs->NumOpenArcErrors != 0)
                            *so << "Open Errors: " << ecs->NumOpenArcErrors << endl;
                    }
                }

                if (isError)
                    retCode = NExitCode::kFatalError;

                if (so)
                    if (ecs->NumArcsWithError != 0 || ecs->NumFileErrors != 0) {
                        // if (ecs->NumArchives > 1)
                        {
                            *so << endl;
                            if (ecs->NumFileErrors != 0)
                                *so << "Sub items Errors: " << ecs->NumFileErrors << endl;
                        }
                    } else if (hresultMain == S_OK) {
                        if (stat.NumFolders != 0)
                            *so << "Folders: " << stat.NumFolders << endl;
                        if (stat.NumFiles != 1 || stat.NumFolders != 0 || stat.NumAltStreams != 0)
                            *so << "Files: " << stat.NumFiles << endl;
                        if (stat.NumAltStreams != 0) {
                            *so << "Alternate Streams: " << stat.NumAltStreams << endl;
                            *so << "Alternate Streams Size: " << stat.AltStreams_UnpackSize << endl;
                        }

                        *so
                                << "Size:       " << stat.UnpackSize << endl
                                << "Compressed: " << stat.PackSize << endl;
                        if (hashCalc) {
                            *so << endl;
                            PrintHashStat(*so, hb);
                        }
                    }
            } else {
                UInt64 numErrors = 0;
                UInt64 numWarnings = 0;

                // options.ExtractNtOptions.StoreAltStreams = true, if -sns[-] is not definmed
                CustomArchiveItemList &dataList = *reinterpret_cast<CustomArchiveItemList *>(environment.extraData);
                hresultMain = ListArchives2(
                        codecs,
                        types,
                        excludedFormats,
                        options.StdInMode,
                        ArchivePathsSorted,
                        ArchivePathsFullSorted,
                        options.ExtractOptions.NtOptions.AltStreams.Val,
                        options.AltStreams.Val, // we don't want to show AltStreams by default
                        options.Censor.Pairs.Front().Head,
                        options.EnableHeaders,
                        options.TechMode,
#ifndef _NO_CRYPTO
                        options.PasswordEnabled,
                        options.Password,
#endif
                        &options.Properties,
                        numErrors, numWarnings, dataList);

                if (options.EnableHeaders)
                    if (numWarnings > 0)
                        g_StdOut << endl << "Warnings: " << numWarnings << endl;

                if (numErrors > 0) {
                    if (options.EnableHeaders)
                        g_StdOut << endl << "Errors: " << numErrors << endl;
                    retCode = NExitCode::kFatalError;
                }
            }
    } else if (options.Command.IsFromUpdateGroup()) {
        LOGI("Start processing Listing Command IsFromUpdateGroup >>>>>");
        CUpdateOptions &uo = options.UpdateOptions;
        if (uo.SfxMode && uo.SfxModule.IsEmpty())
            uo.SfxModule = kDefaultSfxModule;

        COpenCallbackConsole openCallback;
        openCallback.Init(g_StdStream, g_ErrStream, percentsStream);

#ifndef _NO_CRYPTO
        bool passwordIsDefined =
                (options.PasswordEnabled && !options.Password.IsEmpty());
        openCallback.PasswordIsDefined = passwordIsDefined;
        openCallback.Password = options.Password;
#endif

        CUpdateCallbackConsole callback(env);
        callback.LogLevel = options.LogLevel;
        callback.PercentsNameLevel = percentsNameLevel;

        if (percentsStream)
            callback.SetWindowWidth(consoleWidth);

#ifndef _NO_CRYPTO
        callback.PasswordIsDefined = passwordIsDefined;
        callback.AskPassword = (options.PasswordEnabled && options.Password.IsEmpty());
        callback.Password = options.Password;
#endif

        callback.StdOutMode = uo.StdOutMode;
        callback.Init(g_StdStream, g_ErrStream, percentsStream);

        CUpdateErrorInfo errorInfo;

        /*
        if (!uo.Init(codecs, types, options.ArchiveName))
          throw kUnsupportedUpdateArcType;
        */
        hresultMain = UpdateArchive(codecs,
                                    types,
                                    options.ArchiveName,
                                    options.Censor,
                                    uo,
                                    errorInfo, &openCallback, &callback, true);

        callback.ClosePercents2();

        CStdOutStream *se = g_StdStream;
        if (!se)
            se = g_ErrStream;

        retCode = WarningsCheck(hresultMain, callback, errorInfo,
                                g_StdStream, se,
                                true // options.EnableHeaders
        );
#ifdef ENV_UNIX
        if (uo.SfxMode) {
            //  void myAddExeFlag(const UString &name);
            for (int i = 0; i < uo.Commands.Size(); i++) {
                CUpdateArchiveCommand &command = uo.Commands[i];
                if (!uo.StdOutMode) {
                    myAddExeFlag(command.ArchivePath.GetFinalPath());
                }
            }
        }
#endif
    }

    return 0;

}
#ifdef __cplusplus
}
#endif

JavaVM *jvm;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) return -1;
    jvm = vm;
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *, void *) {
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_vn_cma_extract_Archive_listArchive2
        (JNIEnv *env, jobject obj, jstring path, jobject itemsList, jstring password) {
    try {
        if (jvm) {
            jvm->AttachCurrentThread(&env, nullptr);
            // LOGI("jvm->AttachCurrentThread...");
        }
        memset(&environment, 0, sizeof(Environment));
        environment.env = env;
        environment.obj = obj;
        char outbuf[1024];
        memset(&outbuf[0], 0, sizeof(outbuf));
        int len = env->GetStringLength(path);
        env->GetStringUTFRegion(path, 0, len, outbuf);
        //  LOGI("Listing Archive: %s \n", outbuf);

        char passOutbuf[1024];
        memset(&passOutbuf[0], 0, sizeof(passOutbuf));
        int lenPass = env->GetStringLength(password);
        env->GetStringUTFRegion(password, 0, lenPass, passOutbuf);
        //  LOGI("Listing Archive password: %s \n", passOutbuf);

        const char *args[4] = {"7z", "l", passOutbuf, outbuf};

        CustomArchiveItemList dataList;
        environment.extraData = &dataList;
        int ret = ProcessCommand(4, args, environment);
        //LOGI("Number of Items in List is : %d ", dataList.Size());
//        g_StdOut << "Number of Items in List is :" << dataList.Size() << endl;
        for (int i = 0; i < dataList.Size(); i++) {
            jstring listItemPath = env->NewStringUTF(GetOemString(dataList[i].itemPath));
            jstring listItemDateTime = env->NewStringUTF(GetOemString(dataList[i].time));
            jlong unpackSize = dataList[i].unpackSize;
            jlong packSize = dataList[i].packSize;
            env->CallVoidMethod(itemsList,
                                archiveItemsList_addItem,
                                listItemPath,
                                listItemDateTime,
                                unpackSize,
                                packSize,
                                dataList[i].isFolder ? JNI_TRUE : JNI_FALSE
            );
            env->DeleteLocalRef(listItemPath);
            env->DeleteLocalRef(listItemDateTime);
        }
        return ret;
    } catch (...) {
        return -1992;
    }
}

JNIEXPORT jint JNICALL Java_vn_cma_extract_Archive_extractArchive
        (JNIEnv *env, jobject, jstring arc, jstring dest, jobject obj) {
    try {
        if (jvm) {
            jvm->AttachCurrentThread(&env, nullptr);
            LOGI("jvm->AttachCurrentThread...");
        }
        int ret = 0;
        memset(&environment, 0, sizeof(Environment));
        environment.env = env;
        environment.obj = obj;
        char arcbuf[1024];
        memset(&arcbuf[0], 0, sizeof(arcbuf));
        char destbuf[255];
        memset(&destbuf[0], 0, sizeof(destbuf));
        destbuf[0] = '-';
        destbuf[1] = 'o';

        int len = env->GetStringLength(arc);
        env->GetStringUTFRegion(arc, 0, len, arcbuf);

        len = env->GetStringLength(dest);
        env->GetStringUTFRegion(dest, 0, len, destbuf + 2);

        LOGI("Opening Archive: %s \n", arcbuf);
        LOGI("Extracting to: %s \n", destbuf);
        const char *args[5] = {"7z", "x", "-y", destbuf, arcbuf};
        ret = ProcessCommand(5, args, environment);
        return ret;
    } catch (...) {
        return -1992;
    }
}

JNIEXPORT void JNICALL Java_vn_cma_extract_Archive_init
        (JNIEnv *env, jclass cls) {
    jclass extractCallbackClass = env->FindClass("vn/cma/extract/ExtractCallback");
    if (extractCallbackClass == nullptr) {
        LOGE("AAAA Error:couldn't get classid of class: %s", "ExtractCallback");
        return;
    }
    LOGI("Initializing Method IDs for : %s", "ExtractCallback");

    setCurrentFilePath = env->GetMethodID(extractCallbackClass, "setCurrentFilePath",
                                          "(Ljava/lang/String;J)J");
    if (!setCurrentFilePath)
        LOGE("Error:couldn't get methodid of method: %s", "setCurrentFilePath");

    exAddErrorMessage = env->GetMethodID(extractCallbackClass, "addErrorMessage",
                                         "(Ljava/lang/String;)V");
    if (!exAddErrorMessage)
        LOGE("Error:couldn't get methodid of method: %s", "addErrorMessage");

    setOperationResult = env->GetMethodID(extractCallbackClass, "setOperationResult", "(IJZ)J");
    if (!setOperationResult)
        LOGE("Error:couldn't get methodid of method: %s", "setOperationResult");


    cryptoGetTextPassword = env->GetMethodID(extractCallbackClass, "cryptoGetTextPassword",
                                             "(Ljava/lang/String;)Ljava/lang/String;");
    if (!cryptoGetTextPassword)
        LOGE("Error:couldn't get methodid of method: %s", "cryptoGetTextPassword");


    setTotal = env->GetMethodID(extractCallbackClass, "setTotal", "(J)J");
    if (!setTotal)
        LOGE("AAAAA Error:couldn't get methodid of method: %s", "setTotal");

    setCompleted = env->GetMethodID(extractCallbackClass, "setCompleted", "(J)J");
    if (!setCompleted)
        LOGE("Error:couldn't get methodid of method: %s", "setCompleted");

    // Initialization for Java Archive List
    jclass archiveItemsListClass = env->FindClass("vn/cma/extract/data/ArchiveItemsList");
    if (archiveItemsListClass == nullptr) {
        LOGE("Error:couldn't get classid of class: %s", "ArchiveItemsList");
        return;
    }
    archiveItemsList_addItem = env->GetMethodID(archiveItemsListClass, "addItem",
                                                "(Ljava/lang/String;Ljava/lang/String;JJZ)V");
    if (archiveItemsList_addItem == nullptr) {
        LOGE("Error:couldn't get methodid of method: %s", "archiveItemsList_addItem");
        return;
    }
}

#ifdef __cplusplus
}

#endif





