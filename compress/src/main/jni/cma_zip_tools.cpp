#include "cma_zip_tools_Archive.h"
#include <android/log.h>


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


#include "StdAfx.h"

#if defined( _7ZIP_LARGE_PAGES)
#include "../../../../C/Alloc.h"
#endif

#include "Common/MyInitGuid.h"

#include "Common/CommandLineParser.h"
#include "Common/IntToString.h"
#include "Common/MyException.h"
#include "Common/StdOutStream.h"
#include "Common/StringConvert.h"
#include "Common/StringToInt.h"

#include "Windows/Error.h"

#ifdef _WIN32
#include "Windows/MemoryLock.h"
#endif


#include "ArchiveCommandLine.h"
#include "ExitCode.h"
#include "Extract.h"

#ifdef EXTERNAL_CODECS

#include "LoadCodecs.h"

#endif

#include "BenchCon.h"
#include "ExtractCallbackConsole.h"
#include "List.h"
#include "OpenCallbackConsole.h"
#include "UpdateCallbackConsole.h"

#include "MyVersion.h"

#include "myPrivate.h"
#include "Windows/System.h"

#include "ExtractGUI.h"
#include "UpdateGUI.h"

Environment environment;


using namespace NWindows;
using namespace NFile;
using namespace NCommandLineParser;

static const char *kCopyrightString = "\n7-Zip"
                                      #ifndef EXTERNAL_CODECS
                                      " (A)"
                                      #endif

                                      #ifdef _WIN64
                                      " [64]"
                                      #endif

                                      " " MY_VERSION_COPYRIGHT_DATE "\n"
                                      "p7zip Version " P7ZIP_VERSION;

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
    int len = s.Length();
    LOGI("%s", (const char *) &s);
    //stdStream << s;
    // for (int i = len; i < size; i++)
    // LOGI(' ');
    //stdStream << ' ';
}

#endif

static void PrintString(const UString &s, int size) {
    int len = s.Length();
    LOGI("%s", (LPCSTR) GetOemString(s));
    /* stdStream << s;
     for (int i = len; i < size; i++)
       stdStream << ' ';*/
}

static inline char GetHex(Byte value) {

    return (char) ((value < 10) ? ('0' + value) : ('A' + (value - 10)));
}


int ProcessCommand(int numArgs, const char *args[], Environment &env) {
#if defined(_WIN32) && !defined(UNDER_CE)
    SetFileApisToOEM();
#endif

    UStringVector commandStrings;
#ifdef _WIN32
    NCommandLineParser::SplitCommandLine(GetCommandLineW(), commandStrings);
#else
    // GetArguments(numArgs, args, commandStrings);
    extern void mySplitCommandLine(int numArgs, const char *args[], UStringVector &parts);
    mySplitCommandLine(numArgs, args, commandStrings);
#endif

    if (commandStrings.Size() == 1) {
        ShowCopyrightAndHelp(true);
        return 0;
    }
    commandStrings.Delete(0);

    CArchiveCommandLineOptions options;

    CArchiveCommandLineParser parser;

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
        LOGE("Error Loading 7z Codecs");
        return -1;
    }
    //throw CSystemException(result);


    bool isExtractGroupCommand = options.Command.IsFromExtractGroup();
    if (codecs->Formats.Size() == 0 &&
        (isExtractGroupCommand ||
         options.Command.CommandType == NCommandType::kList ||
         options.Command.IsFromUpdateGroup())) {
        LOGE("%s", kNoFormats);
        return -1;
        //throw kNoFormats;
    }
    CIntVector formatIndices;
    if (!codecs->FindFormatForArchiveType(options.ArcType, formatIndices)) {
        LOGE("%s", kUnsupportedArcTypeMessage);
        return -2;
        //throw kUnsupportedArcTypeMessage;
    }


    if (options.Command.CommandType == NCommandType::kInfo) {
        LOGI("Formats:");
        // stdStream << endl << "Formats:" << endl;
        int i;
        for (i = 0; i < codecs->Formats.Size(); i++) {
            const CArcInfoEx &arc = codecs->Formats[i];
#ifdef EXTERNAL_CODECS
            if (arc.LibIndex >= 0) {
                char s[16];
                ConvertUInt32ToString(arc.LibIndex, s);
            //    PrintString(s, 2);
            } else
#endif
                LOGI("");
            //stdStream << "  ";
            //stdStream << ' ';
            LOGI(" %s%s %s ", arc.UpdateEnabled ? "C" : " ", arc.KeepName ? "K" : " ",
                 (LPCSTR) GetOemString(arc.Name));
            //stdStream << (char)(arc.UpdateEnabled ? 'C' : ' ');
            //stdStream << (char)(arc.KeepName ? 'K' : ' ');
            //stdStream << "  ";
            //PrintString(stdStream, arc.Name, 6);
            //stdStream << "  ";
            UString s;
            for (int t = 0; t < arc.Exts.Size(); t++) {
                const CArcExtInfo &ext = arc.Exts[t];
                s += ext.Ext;
                if (!ext.AddExt.IsEmpty()) {
                    s += L" (";
                    s += ext.AddExt;
                    s += L')';
                }
                s += L' ';
            }
           // PrintString(s, 14);
            //stdStream << "  ";

            const CByteBuffer &sig = arc.StartSignature;
            char sigstring[sig.GetCapacity() + 1];
            for (size_t j = 0; j < sig.GetCapacity(); j++) {
                Byte b = sig[j];
                if (b > 0x20 && b < 0x80) {
                    //  stdStream << (char)b;
                    sigstring[j] = (char) b;
                } else {
                    //stdStream << GetHex((Byte)((b >> 4) & 0xF));
                    //stdStream << GetHex((Byte)(b & 0xF));
                }
                //stdStream << ' ';
            }
            // stdStream << endl;
        }
        LOGI("Codecs:");
        // stdStream << endl << "Codecs:" << endl;

#ifdef EXTERNAL_CODECS
        UInt32 numMethods;
        if (codecs->GetNumberOfMethods(&numMethods) == S_OK)
            for (UInt32 j = 0; j < numMethods; j++) {
                int libIndex = codecs->GetCodecLibIndex(j);
                if (libIndex >= 0) {
                    char s[16];
                    ConvertUInt32ToString(libIndex, s);
                  //  PrintString(s, 2);
                } else
                    LOGI("");

                //stdStream << (char)(codecs->GetCodecEncoderIsAssigned(j) ? 'C' : ' ');
                UInt64 id;
                // stdStream << "  ";
                HRESULT res = codecs->GetCodecId(j, id);
                if (res != S_OK)
                    id = (UInt64) (Int64) -1;
                char s[32];
                ConvertUInt64ToString(id, s, 16);
                //PrintString(stdStream, s, 8);
                // stdStream << "  ";
                // PrintString(stdStream, codecs->GetCodecName(j), 11);
                // stdStream << endl;
                LOGI(" %s %s %s", (codecs->GetCodecEncoderIsAssigned(j) ? "C" : " "), s,
                     (LPCSTR) GetOemString(codecs->GetCodecName(j)));
                /*
                if (res != S_OK)
                  throw "incorrect Codec ID";
                */
            }
#endif
        return 0;
    } else if (isExtractGroupCommand || options.Command.CommandType == NCommandType::kList) {
        if (isExtractGroupCommand) {
            LOGI("Start processing Extraction Command >>>>>");
            CExtractCallbackImp *ecs = new CExtractCallbackImp(env);
            CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

#ifndef _NO_CRYPTO
            ecs->PasswordIsDefined = options.PasswordEnabled;
            ecs->Password = options.Password;
#endif

            ecs->Init();

            CExtractOptions eo;
            eo.StdOutMode = options.StdOutMode;
            eo.OutputDir = options.OutputDir;
            eo.YesToAll = options.YesToAll;
            eo.OverwriteMode = options.OverwriteMode;
            eo.PathMode = options.Command.GetPathMode();
            eo.TestMode = options.Command.IsTestMode();
            eo.CalcCrc = options.CalcCrc;
#if !defined(_7ZIP_ST) && !defined(_SFX)
            eo.Properties = options.ExtractProperties;
#endif

            bool messageWasDisplayed = false;
            LOGI("Calling ExtractGUI >>>>>");
            HRESULT result = ExtractGUI(codecs, formatIndices,
                                        options.ArchivePathsSorted,
                                        options.ArchivePathsFullSorted,
                                        options.WildcardCensor.Pairs.Front().Head,
                                        eo, options.ShowDialog, messageWasDisplayed, ecs);
            if (result != S_OK) {
                if (result != E_ABORT && messageWasDisplayed)
                    return NExitCode::kFatalError;
                LOGI("Calling ExtractGUI Error,Throwing Exception");
                // throw CSystemException(result);
            }
            if (!ecs->IsOK()) {
                LOGI("Extraction Error!!");
                return NExitCode::kFatalError;
            }
            LOGI("Extraction Completed Successfully");
        } else {
            LOGI("Start processing Listing Command >>>>>");
            UInt64 numErrors = 0;
            CustomArchiveItemList &dataList = *reinterpret_cast<CustomArchiveItemList *>(environment.extraData);
            HRESULT result = ListArchives(
                    codecs,
                    formatIndices,
                    options.StdInMode,
                    options.ArchivePathsSorted,
                    options.ArchivePathsFullSorted,
                    options.WildcardCensor.Pairs.Front().Head,
                    options.EnableHeaders,
                    options.TechMode,
#ifndef _NO_CRYPTO
                    options.PasswordEnabled,
                    options.Password,
#endif
                    numErrors,
                    dataList);
            if (numErrors == 8888) {
                return NExitCode::kPassError;
            }
            if (numErrors > 0) {
                g_StdOut << endl << "Errors: " << numErrors;
                return NExitCode::kFatalError;
            }
            if (result != S_OK) {
                throw CSystemException(result);
            }
        }
        return 0;
    } else if (options.Command.IsFromUpdateGroup()) {
#ifndef _NO_CRYPTO
        bool passwordIsDefined = options.PasswordEnabled && !options.Password.IsEmpty();
#endif

        CUpdateCallbackGUI callback(env);
#ifndef _NO_CRYPTO
        callback.PasswordIsDefined = passwordIsDefined;
        callback.AskPassword = options.PasswordEnabled && options.Password.IsEmpty();
        callback.Password = options.Password;
#endif

        callback.Init();

        if (!options.UpdateOptions.Init(codecs, formatIndices, options.ArchiveName)) {
            //ErrorLangMessage(IDS_UPDATE_NOT_SUPPORTED, 0x02000601);
            return NExitCode::kFatalError;
        }
        bool messageWasDisplayed = false;
        if (!options.ShowDialog)
            options.ShowDialog = true;
        LOGI("Starting Archiving.......");

        HRESULT result = UpdateGUI(
                codecs,
                options.WildcardCensor, options.UpdateOptions,
                options.ShowDialog,
                messageWasDisplayed, &callback, *(reinterpret_cast<CInfo *>(env.extraData)));

        if (result != S_OK) {
            CSysString message;
            NError::MyFormatMessage(result, message);
            LOGE("Error, SystemException: %s", (LPCSTR) GetOemString(message));
            if (result == E_ABORT)
                return result;
            if (messageWasDisplayed) {
                LOGE("Error, kFatalError:%d", result);
                return NExitCode::kFatalError;
            }
        }
        if (callback.FailedFiles.Size() > 0) {
            if (!messageWasDisplayed) {
                LOGE("Error, CSystemException:%d", E_FAIL);
                throw CSystemException(E_FAIL);
            }
            return NExitCode::kWarning;
        }

        return 0;
    }

    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

int InitializeUpdateCallbackIds(JNIEnv *env) {
    int ret = 0;
    jclass updateCallbackClass = env->FindClass("vn/cma/compress/ZipCallback");
    if (updateCallbackClass == NULL) {
        LOGE("Error:couldn't get classid of class: %s", "updateCallbackClass");
        return -1;
    }
    LOGI("Initializing Method IDs for : %s", "updateCallback");
    startArchive = env->GetMethodID(updateCallbackClass, "startArchive", "(Ljava/lang/String;Z)J");
    if (!startArchive)
        LOGE("Error:couldn't get methodid of method: %s", "startArchive");

    checkBreak = env->GetMethodID(updateCallbackClass, "checkBreak", "()J");
    if (!checkBreak)
        LOGE("Error:couldn't get methodid of method: %s", "checkBreak");

    scanProgress = env->GetMethodID(updateCallbackClass, "scanProgress", "(JJLjava/lang/String;)J");
    if (!scanProgress)
        LOGE("Error:couldn't get methodid of method: %s", "scanProgress");

    updateSetNumFiles = env->GetMethodID(updateCallbackClass, "setNumFiles", "(J)J");
    if (!updateSetNumFiles)
        LOGE("Error:couldn't get methodid of method: %s", "setNumFiles");

    updateSetTotal = env->GetMethodID(updateCallbackClass, "setTotal", "(J)J");
    if (!updateSetTotal)
        LOGE("Error:couldn't get methodid of method: %s", "setTotal");

    updateSetCompleted = env->GetMethodID(updateCallbackClass, "setCompleted", "(J)J");
    if (!updateSetCompleted)
        LOGE("Error:couldn't get methodid of method: %s", "setCompleted");

    updateSetRatioInfo = env->GetMethodID(updateCallbackClass, "setRatioInfo", "(JJ)J");
    if (!updateSetRatioInfo)
        LOGE("Error:couldn't get methodid of method: %s", "setRatioInfo");

    getStream = env->GetMethodID(updateCallbackClass, "getStream", "(Ljava/lang/String;Z)J");
    if (!getStream)
        LOGE("Error:couldn't get methodid of method: %s", "getStream");

    updateSetOperationResult = env->GetMethodID(updateCallbackClass, "setOperationResult", "(J)J");
    if (!updateSetOperationResult)
        LOGE("Error:couldn't get methodid of method: %s", "setOperationResult");

    openCheckBreak = env->GetMethodID(updateCallbackClass, "openCheckBreak", "()J");
    if (!openCheckBreak)
        LOGE("Error:couldn't get methodid of method: %s", "openCheckBreak");

    openSetCompleted = env->GetMethodID(updateCallbackClass, "openSetCompleted", "(JJ)J");
    if (!openSetCompleted)
        LOGE("Error:couldn't get methodid of method: %s", "openSetCompleted");

    addErrorMessage = env->GetMethodID(updateCallbackClass, "addErrorMessage",
                                       "(Ljava/lang/String;)V");
    if (!addErrorMessage)
        LOGE("Error:couldn't get methodid of method: %s", "addErrorMessage");
    return ret;
}

JNIEXPORT void JNICALL Java_vn_cma_compress_Archive_init
        (JNIEnv *env, jclass cls) {
    InitializeUpdateCallbackIds(env);
}


JNIEXPORT jint JNICALL Java_vn_cma_compress_Archive_createArchive
        (JNIEnv *env, jobject obj, jstring archivename, jobjectArray filespaths, jint length,
         jint level, jint dictionary,
         jint wordSize, jboolean orderMode, jboolean solidDefined, jlong solidBlockSize,
         jstring method,
         jstring encryptionMethod, jint findex, jboolean encryptHeaders,
         jboolean encryptHeadersAllowed, jstring pass, jboolean multiThread, jobject callback) {
    try {
        int ret = 0;
        memset(&environment, 0, sizeof(Environment));
        environment.env = env;
        environment.obj = callback;

        char arcName[1024];
        memset(&arcName[0], 0, sizeof(arcName));
        char methodName[30];
        memset(&methodName[0], 0, sizeof(methodName));
        char encMethodName[30];
        memset(&encMethodName[0], 0, sizeof(encMethodName));
        char passWord[400];
        memset(&passWord[0], 0, sizeof(passWord));
        int len = env->GetStringLength(archivename);
        env->GetStringUTFRegion(archivename, 0, len, arcName);

        len = env->GetStringLength(method);
        env->GetStringUTFRegion(method, 0, len, methodName);

        len = env->GetStringLength(encryptionMethod);
        env->GetStringUTFRegion(encryptionMethod, 0, len, encMethodName);

        len = env->GetStringLength(pass);
        env->GetStringUTFRegion(pass, 0, len, passWord);

        const char *files[length];
        AStringVector pathsstrings;
        pathsstrings.Clear();
        for (int i = 0; i < length; i++) {
            char arcbuf[1024];
            memset(&arcbuf[0], 0, sizeof(arcbuf));
            jstring jstr = (jstring) (env->GetObjectArrayElement(filespaths, i));
            len = env->GetStringLength(jstr);
            env->GetStringUTFRegion(jstr, 0, len, arcbuf);
            pathsstrings.Add(arcbuf);
            files[i] = pathsstrings[i];

            LOGI("File:%s in Archive:%s", files[i], arcName);
            //memset(&arcbuf[0], 0, sizeof(arcbuf));
        }
        if (length <= 0) {
            LOGE("Error,String length is zero....");
            return -2;
        }
        const int count = length + 4;
        const char **args = new const char *[count];//{"7z","a","-y",arcName,arcbuf};
        args[0] = "7z";
        args[1] = "a";
        args[2] = "-y";
        args[3] = arcName;
        for (int i = 0; i < length; i++)
            args[i + 4] = files[i];
        CInfo Info;
        UString s = GetSystemString(arcName);
        Info.ArchiveName = s;
        Info.FormatIndex = findex;
        Info.Dictionary = dictionary;
        Info.Level = level;
        Info.Method = GetSystemString(methodName);
        Info.OrderMode = orderMode == JNI_TRUE;
        Info.Order = wordSize;
        Info.SolidIsSpecified = solidDefined == JNI_TRUE;
        Info.SolidBlockSize = solidBlockSize;
        Info.EncryptHeaders = encryptHeaders == JNI_TRUE;
        Info.EncryptHeadersIsAllowed = encryptHeadersAllowed == JNI_TRUE;
        Info.MultiThreadIsAllowed = multiThread == JNI_TRUE;
        Info.EncryptionMethod = GetSystemString(encMethodName);
        Info.Password = GetSystemString(passWord);

        environment.extraData = &Info;
        ret = ProcessCommand(count, args, environment);
        //pathsstrings.
        delete[] args;
        args = NULL;
        return ret;
    }catch (...){
        return -1992;
    }
}

#ifdef __cplusplus
}
#endif





