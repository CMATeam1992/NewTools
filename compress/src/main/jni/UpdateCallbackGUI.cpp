// UpdateCallbackGUI.cpp

#include "StdAfx.h"

#include "Common/IntToString.h"
#include "Common/StringConvert.h"

#include "Windows/Error.h"
#include "Windows/PropVariant.h"

//#include "../FileManager/PasswordDialog.h"

#include "UpdateCallbackGUI.h"


extern jmethodID startArchive, checkBreak, scanProgress, updateSetNumFiles, updateSetTotal,
        updateSetCompleted, updateSetRatioInfo, getStream, updateSetOperationResult,
        openCheckBreak, openSetCompleted, addErrorMessage;


using namespace NWindows;

CUpdateCallbackGUI::~CUpdateCallbackGUI() {}

void CUpdateCallbackGUI::Init() {
    FailedFiles.Clear();
    NumFiles = 0;
}

void CUpdateCallbackGUI::AddErrorMessage(LPCWSTR message) {
    if (enviro.env) {
        jstring Name = enviro.env->NewStringUTF((LPCSTR) GetOemString(message));
        enviro.env->CallVoidMethod(enviro.obj, addErrorMessage, Name);
        enviro.env->DeleteLocalRef(Name);
    }
}

void CUpdateCallbackGUI::AddErrorMessage(const wchar_t *name, DWORD systemError) {
    AddErrorMessage(
            UString(L"WARNING: ") +
            NError::MyFormatMessageW(systemError) +
            UString(L": ") +
            UString(name));
    LOGE("%s", (const char *) GetOemString(UString(L"WARNING: ") +
                                           NError::MyFormatMessageW(systemError) +
                                           UString(L": ") +
                                           UString(name)));
    //LOGE("");
}

HRESULT CUpdateCallbackGUI::OpenResult(const wchar_t *name, HRESULT result) {
    if (result != S_OK) {
        LOGE("%s", (const char *) GetOemString(name));
        AddErrorMessage(UString(L"Error: ") + name + UString(L" is not supported archive"));
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::StartScanning() {
    return S_OK;
}

HRESULT CUpdateCallbackGUI::CanNotFindError(const wchar_t *name, DWORD systemError) {
    FailedFiles.Add(name);
    AddErrorMessage(name, systemError);
    return S_OK;
}

HRESULT CUpdateCallbackGUI::FinishScanning() {
    return S_OK;
}

HRESULT CUpdateCallbackGUI::StartArchive(const wchar_t *name, bool updating) {
    if (enviro.env) {
        jstring Name = enviro.env->NewStringUTF((LPCSTR) GetOemString(name));
        enviro.env->CallLongMethod(enviro.obj, startArchive, Name,
                                   updating ? JNI_TRUE : JNI_FALSE);
        enviro.env->DeleteLocalRef(Name);
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::FinishArchive() {
    LOGI("FinishArchive");
    return S_OK;
}

HRESULT CUpdateCallbackGUI::CheckBreak() {
    return S_OK;
}

HRESULT CUpdateCallbackGUI::ScanProgress(UInt64 numFolders, UInt64 numFiles, const wchar_t *path) {
    if (enviro.env) {
        jstring Name = enviro.env->NewStringUTF((LPCSTR) GetOemString(path));
        enviro.env->CallLongMethod(enviro.obj, scanProgress, numFolders, (jlong) numFiles, Name);
        enviro.env->DeleteLocalRef(Name);
    }
    return CheckBreak();//S_OK;
}

HRESULT CUpdateCallbackGUI::Finilize() {
    return S_OK;
}

HRESULT CUpdateCallbackGUI::SetNumFiles(UInt64 numFiles) {
    if (enviro.env) {
        enviro.env->CallLongMethod(enviro.obj, updateSetNumFiles, (jlong) numFiles);
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::SetTotal(UInt64 total) {
    if (enviro.env) {
        enviro.env->CallLongMethod(enviro.obj, updateSetTotal, (jlong) total);
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::SetCompleted(const UInt64 *completeValue) {
    if (enviro.env) {
        enviro.env->CallLongMethod(enviro.obj, updateSetCompleted, (jlong) (*completeValue));
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::SetRatioInfo(const UInt64 *inSize, const UInt64 *outSize) {
    if (inSize && outSize) {
        if (enviro.env) {
            enviro.env->CallLongMethod(enviro.obj, updateSetRatioInfo, (jlong) (*inSize),
                                       (jlong) (*outSize));
        }
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::GetStream(const wchar_t *name, bool isAnti) {
    if (enviro.env) {
        jstring Name = enviro.env->NewStringUTF((LPCSTR) GetOemString(name));
        enviro.env->CallLongMethod(enviro.obj, getStream, Name, isAnti ? JNI_TRUE : JNI_FALSE);
        enviro.env->DeleteLocalRef(Name);
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::OpenFileError(const wchar_t *name, DWORD systemError) {
    FailedFiles.Add(name);
    //// if (systemError == ERROR_SHARING_VIOLATION)
    {
        AddErrorMessage(name, systemError);
        return S_FALSE;
    }
    //// return systemError;
}

HRESULT CUpdateCallbackGUI::SetOperationResult(Int32 /* operationResult */) {
    NumFiles++;
    if (enviro.env) {
        //jstring Name =enviro.env->NewStringUTF((LPCSTR)GetOemString(name));
        enviro.env->CallLongMethod(enviro.obj, updateSetOperationResult, (jlong) NumFiles);
        //enviro.env->DeleteLocalRef(Name);
    }
    return S_OK;
}

HRESULT CUpdateCallbackGUI::CryptoGetTextPassword2(Int32 *passwordIsDefined, BSTR *password) {
    *password = NULL;
    if (!PasswordIsDefined) {
        if (passwordIsDefined == 0 || AskPassword) {
            //CPasswordDialog dialog;
            //ProgressDialog->WaitCreating();
            //if (dialog.Create(*ProgressDialog) == IDCANCEL)
            //return E_ABORT;
            //Password = dialog.Password;
            PasswordIsDefined = true;
        }
    }
    if (passwordIsDefined != nullptr)
        *passwordIsDefined = BoolToInt(PasswordIsDefined);
    return StringToBstr(Password, password);
}

HRESULT CUpdateCallbackGUI::CryptoGetTextPassword(BSTR *password) {
    return CryptoGetTextPassword2(NULL, password);
}

/*
It doesn't work, since main stream waits Dialog
HRESULT CUpdateCallbackGUI::CloseProgress()
{
  ProgressDialog->MyClose();
  return S_OK;
}
*/


HRESULT CUpdateCallbackGUI::Open_CheckBreak() {
    return CheckBreak();
}

HRESULT
CUpdateCallbackGUI::Open_SetTotal(const UInt64 * /* numFiles */, const UInt64 * /* numBytes */) {
    //// if (numFiles != NULL) ProgressDialog->Sync.SetNumFilesTotal(*numFiles);
    return S_OK;
}

HRESULT CUpdateCallbackGUI::Open_SetCompleted(const UInt64 * /* numFiles */,
                                              const UInt64 * /* numBytes */) {
    if (enviro.env) {
        enviro.env->CallLongMethod(enviro.obj, openSetCompleted, 0, 0);
    }
    return CheckBreak();
}

#ifndef _NO_CRYPTO

HRESULT CUpdateCallbackGUI::Open_CryptoGetTextPassword(BSTR *password) {
    PasswordWasAsked = true;
    return CryptoGetTextPassword2(NULL, password);
}

HRESULT CUpdateCallbackGUI::Open_GetPasswordIfAny(UString &password) {
    if (PasswordIsDefined)
        password = Password;
    return S_OK;
}

bool CUpdateCallbackGUI::Open_WasPasswordAsked() {
    return PasswordWasAsked;
}

void CUpdateCallbackGUI::Open_ClearPasswordWasAskedFlag() {
    PasswordWasAsked = false;
}

/*
HRESULT CUpdateCallbackGUI::ShowDeleteFile(const wchar_t *name)
{
  ProgressDialog->Sync.SetCurrentFileName(name);
  return S_OK;
}
*/

#endif
