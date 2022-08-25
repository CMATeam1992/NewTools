package vn.cma.extract.extension;

import java.util.List;

import vn.cma.extract.Archive;
import vn.cma.extract.ExtractCallback;
import vn.cma.extract.data.ArchiveData;
import vn.cma.extract.data.ArchiveItemsList;

public class ArchiveUtils {
    public static void getList(String filePath) {
        ArchiveData archiveData = new Archive().getArchiveItemsList(filePath, "-p$password");
        List<ArchiveItemsList.ArchiveItem> list = archiveData.list;
//        for (int i = 0; i < list.size(); i++) {
//            Log.e("AAAAA result = ", list.get(i).getItemPath() + " Time = " + list.get(i).getItemDateTime());
//        }
    }
    public static void extractFile(String filePath, ExtractCallback extractCallback) {
        new Archive().extractArchive(filePath, "/storage/emulated/0/ZipExtractor/Extracted/ATestPass6", extractCallback);
    }
}
