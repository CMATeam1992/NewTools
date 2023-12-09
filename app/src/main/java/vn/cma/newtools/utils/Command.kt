package vn.cma.newtools.utils

import android.util.Log

fun getCompressCmd(filePaths: ArrayList<String>, outPath: String, type: String): String {
    val command = StringBuilder(String.format("7z a -t%s '%s'", type, outPath))
    for (i in 0 until filePaths.size) {
        command.append(" '").append(filePaths[i]).append("'")
    }
    return command.toString()
}

fun extractFileInArchive(filePath: String, outFolder: String, outFile: String): String {
    //7z e [archive.zip] -o [outputdir] [fileFilter]
    val cm =
        String.format("7z x \"%s\" \"-o%s\" \"%s\" -r", filePath, outFolder, "testpass_20230331")
    Log.e("AAAA CM = %s", cm)
    return cm
}