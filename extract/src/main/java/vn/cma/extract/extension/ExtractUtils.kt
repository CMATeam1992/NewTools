package vn.cma.extract.extension

import vn.cma.extract.Archive
import vn.cma.extract.ExtractCallback
import vn.cma.extract.data.ArchiveItemsList.ArchiveItem

object ExtractUtils {

    fun getListFileInArchive(filePath: String, password: String = "1234567"): List<ArchiveItem> {
        val archiveData = Archive().getArchiveItemsList(filePath, "-p$password")
        return archiveData.list
    }

    fun extractFile(filePath: String, outputPath: String, extractCallback: ExtractCallback?) {
        Archive().extractArchive(filePath, outputPath, extractCallback)
    }
}