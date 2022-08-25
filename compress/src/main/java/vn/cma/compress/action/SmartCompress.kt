package vn.cma.compress.action

import vn.cma.compress.Archive
import vn.cma.compress.ZipCallback
import vn.cma.compress.utils.compression.CInfo

object SmartCompress {
    fun zipFile(smartCompressData: SmartCompressData, callback: ZipCallback) {
        val listFile = smartCompressData.listPath
        val arc = Archive()
        val info = CInfo()
        when (smartCompressData.compressType) {
            SmartCompressType.ZIP -> info.initZip(
                smartCompressData.password,
                smartCompressData.outputPath,
                smartCompressData.compressLevel.rawValue
            )
            SmartCompressType.SEVEN_ZIP -> info.init7Z(
                smartCompressData.password,
                smartCompressData.outputPath,
                smartCompressData.compressLevel.rawValue
            )
            SmartCompressType.TAR -> info.initTar(
                smartCompressData.password,
                smartCompressData.outputPath
            )
        }
        arc.createArchive(
            info.ArchiveName,
            listFile.toTypedArray(),
            listFile.size,
            info.Level,
            info.Dictionary,
            info.Order,
            info.OrderMode,
            info.SolidIsSpecified,
            info.SolidBlockSize,
            info.Method,
            info.EncryptionMethod,
            info.FormatIndex,
            info.EncryptHeaders,
            info.EncryptHeadersIsAllowed,
            info.Password,
            info.MultiThreadIsAllowed,
            callback
        )
    }
}

data class SmartCompressData(
    val compressType: SmartCompressType,
    val listPath: List<String>,
    val password: String,
    val outputPath: String,
    val compressLevel: CompressLevel
)

enum class SmartCompressType {
    ZIP, SEVEN_ZIP, TAR
}

enum class CompressLevel(val rawValue: Int) {
    FAST(3), NORMAL(5), MAXIMUM(7)
}