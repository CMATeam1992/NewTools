package vn.cma.newtools

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import vn.cma.extract.ExtractCallback
import vn.cma.extract.extension.ExtractUtils

class MainActivity : AppCompatActivity(), ExtractCallback {

    private var pass = "123456890"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        //val filePath = "/storage/emulated/0/ZipExtractor/Compressed/testpass.zip"
        val filePath = "/storage/emulated/0/Download/Zalo/TestRar.rar"
       val list = ExtractUtils.getListFileInArchive(filePath)
        list.forEach {
            Log.e(
                "AAAAA result = ",
                it.itemPath.toString() + " Time = " + it.itemDateTime
            )
        }
//        ExtractUtils.extractFile(
//            filePath,
//            "/storage/emulated/0/ZipExtractor/Extracted/ATestPass6",
//            this
//        )
    }

    override fun guiGetPassword(): String {
        return pass
    }

    override fun guiIsPasswordSet(): Boolean {
        return pass.isNotEmpty()
    }

    override fun setCurrentFilePath(filePath: String?, numFilesCur: Long): Long {
        Log.e("BBBBB tFilePath", filePath + "")
        return 0L
    }

    override fun setOperationResult(
        operationResult: Int,
        numFilesCur: Long,
        encrypted: Boolean
    ): Long {
        Log.e("BBBBB Result", operationResult.toString())
        return 0L
    }

    override fun cryptoGetTextPassword(password: String?): String {
        if (password == null) {
            Log.e("BBBBB TextPassword", "NULL")
        }
        return pass
    }

    override fun setTotal(total: Long): Long {
        totalBytes = total
        return 0
    }

    var totalBytes: Long = 0
    var currentPercent = 0
    override fun setCompleted(value: Long): Long {
        if (totalBytes == 0L) totalBytes = 1L
        val percentValue = (value * 100 / totalBytes).toInt()
        if (percentValue != Int.MAX_VALUE) {
            if (percentValue != 0 && currentPercent != percentValue) {
                currentPercent = percentValue
                Log.e("BBBBB setCompleted", currentPercent.toString() + "")
            }
        }
        return 0
    }

    override fun addErrorMessage(message: String?) {
        Log.e("AAAAA addErrorMessage", message + "")
    }
}