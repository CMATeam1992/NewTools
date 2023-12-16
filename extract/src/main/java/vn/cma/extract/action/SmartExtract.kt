package vn.cma.extract.action

import vn.cma.extract.Archive
import vn.cma.extract.ExtractCallback
import vn.cma.extract.data.ArchiveData

object SmartExtract {

    fun deleteFileInZip(
        filePath: String,
        fileDelete: String
    ): Int {
        return Archive().deleteFileInZip(filePath, fileDelete)
    }

    fun getListFileInArchive(filePath: String, password: String = "123456"): ArchiveData {
        return Archive().getArchiveItemsList(filePath, "-p$password")
    }

    fun extractOnlyFile(
        filePath: String,
        fileExtract: String,
        outputPath: String,
        password: String,
        extractCallback: ExtractCallback
    ): Int {
        return Archive().executeCommand(
            filePath,
            outputPath,
            fileExtract,
            password,
            extractCallback
        )
    }

    fun extractMultipleFileInZip(
        filePath: String,
        listFileNeedExtract: List<String>,
        outputPath: String,
        password: String,
        extractCallback: ExtractCallback
    ): Int {
        return Archive().executeCommandList(
            filePath,
            outputPath,
            listFileNeedExtract.toTypedArray(),
            listFileNeedExtract.size,
            password,
            extractCallback
        )
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