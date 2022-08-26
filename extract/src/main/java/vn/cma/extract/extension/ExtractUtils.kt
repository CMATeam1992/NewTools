package vn.cma.extract.extension

import vn.cma.extract.Archive
import vn.cma.extract.ExtractCallback
import vn.cma.extract.data.ArchiveData

object ExtractUtils {

    fun getListFileInArchive(filePath: String, password: String = "1234567"): ArchiveData {
        return Archive().getArchiveItemsList(filePath, "-p$password")
    }

    fun extractFile(filePath: String, outputPath: String, extractCallback: ExtractCallback) {
        Archive().extractArchive(filePath, outputPath, extractCallback)
    }
}