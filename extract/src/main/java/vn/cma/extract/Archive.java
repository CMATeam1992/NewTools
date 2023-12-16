
package vn.cma.extract;

import vn.cma.extract.data.ArchiveData;
import vn.cma.extract.data.ArchiveItemsList;


public class Archive {
    static {
        System.loadLibrary("p7zip");
        init();
    }

    /**
     * Must be called before any other API
     */
    static native void init();

    /**
     * opens th archive in listing mode and returns a list of items in {@link ArchiveItemsList }
     *
     * @param archPath  archive name including path
     * @param itemsList passed {@link ArchiveItemsList } object. After successful return this should
     *                  contain a flatten list of all archive items
     * @param password
     * @return error code or 0
     */
    public native int listArchive2(String archPath, ArchiveItemsList itemsList, String password);

    public native int executeCommand(String archPath, String outFolder, String fileName, String password, ExtractCallback callback);


    public native int executeCommandList(String archPath, String outFolder, String[] itemsPaths, int length, String password, ExtractCallback callback);

    /**
     * Extracts the supplied archive into the given directory
     *
     * @param archPath
     * @param extractionPath
     * @param callback
     * @return 0 on success ,otherwise error state
     */
    public native int extractArchive(String archPath, String extractionPath, String password, ExtractCallback callback);

    public native int deleteFileInZip(String filePath, String fileDelete);


    public ArchiveData getArchiveItemsList(String filePath, String password) {
        ArchiveItemsList archiveItemsList = new ArchiveItemsList();
        int result = listArchive2(filePath, archiveItemsList, password);
        return new ArchiveData(
                archiveItemsList.getItems(),
                result == 888
        );
    }

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
