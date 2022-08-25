package vn.cma.compress;

public class Archive {
    static {
        System.loadLibrary("Rar");
        System.loadLibrary("7z");
        System.loadLibrary("cma_zip_tools");
        init();
    }


    /**
     * Must be called before any other API
     */
    static native void init();


    /**
     * Start creating a new archive by compressing the supplied files
     */
    public native int createArchive(String archFullName, String[] itemsPaths, int length, int level,
                                    int dictionary, int wordSize, boolean orderMode, boolean solidDefined, long SolidBlockSize,
                                    String method, String encryptionMethod, int formatIndex, boolean encryptHeaders
            , boolean encryptHeadersAllowed, String pass, boolean multiThread, ZipCallback callback);

    public interface Constants {
        int S_OK = 0x00000000;
        int S_FALSE = 0x00000001;
        int E_NOTIMPL = 0x80004001;
        int E_NOINTERFACE = 0x80004002;
        int E_ABORT = 0x80004004;
        int E_FAIL = 0x80004005;
        int STG_E_INVALIDFUNCTION = 0x80030001;
        int E_OUTOFMEMORY = 0x8007000E;
        int E_INVALIDARG = 0x80070057;
        int ERROR_NO_MORE_FILES = 0x100123;
    }
}
