package vn.cma.extract.action

import vn.cma.extract.Archive
import vn.cma.extract.ExtractCallback
import vn.cma.extract.data.ArchiveData

object SmartExtract {

    fun getListFileInArchive(filePath: String, password: String = "123456"): ArchiveData {
        return Archive().getArchiveItemsList(filePath, "-p$password")
    }

    fun extractOnlyFile(
        filePath: String,
        fileExtract: String,
        outputPath: String,
        extractCallback: ExtractCallback
    ): Int {
        return Archive().executeCommand(filePath, outputPath, fileExtract, extractCallback)
    }

    fun extractFile(
        filePath: String,
        outputPath: String,
        password: String,
        extractCallback: ExtractCallback
    ): Int {
        return Archive().extractArchive(filePath, outputPath, "-p$password", extractCallback)
    }
}