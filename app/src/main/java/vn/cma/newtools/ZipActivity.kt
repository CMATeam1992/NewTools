package vn.cma.newtools

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers
import io.reactivex.rxjava3.core.Single
import io.reactivex.rxjava3.schedulers.Schedulers
import vn.cma.compress.ZipCallback
import vn.cma.compress.action.CompressLevel
import vn.cma.compress.action.SmartCompress
import vn.cma.compress.action.SmartCompressData
import vn.cma.compress.action.SmartCompressType

class ZipActivity : AppCompatActivity(), ZipCallback {

    companion object {
        fun createIntent(context: Context) =
            Intent(context, ZipActivity::class.java).apply {

            }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_zip)
        val list = listOf<String>(
            "/storage/emulated/0/DCIM/Screenshots/Screenshot_2022-08-26-11-11-14-658_com.woowahan.vn.baemin.jpg",
            "/storage/emulated/0/DCIM/Screenshots/Screenshot_2022-08-26-10-47-48-734_com.deliverynow.jpg",
            "/storage/emulated/0/DCIM/Screenshots/Screenshot_2022-08-26-08-06-41-402_lockscreen.jpg"
        )
        val outPath = "/storage/emulated/0/ZipExtractor/Compressed/ok.zip"
        val smartCompressData = SmartCompressData(
            compressType = SmartCompressType.ZIP,
            listPath = list,
            password = "",
            outputPath = outPath,
            compressLevel = CompressLevel.NORMAL
        )
       Single.fromCallable {
           SmartCompress.zipFile(
               smartCompressData = smartCompressData,
               this
           )
       }.subscribeOn(Schedulers.io())
           .observeOn(AndroidSchedulers.mainThread())
           .subscribe({
               Log.e("AAAA success %s", "success")
           },{
               Log.e("AAAA error %s", it.message.orEmpty())
           })
    }

    override fun addErrorMessage(message: String?) {

    }

    override fun startArchive(name: String?, updating: Boolean): Long {
        return 0L
    }

    override fun checkBreak(): Long {
        return 0L
    }

    override fun scanProgress(numFolders: Long, numFiles: Long, path: String?): Long {
        return 0L
    }

    override fun setNumFiles(numFiles: Long): Long {
        return 0L
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

    override fun setRatioInfo(inSize: Long, outSize: Long): Long {
        Log.e("BBBBB setRatioInfo", "$inSize $outSize")
        return 0L
    }

    override fun getStream(name: String?, isAnti: Boolean): Long {
       return 0L
    }

    override fun setOperationResult(operationResult: Long): Long {
        return 0L
    }

    override fun openCheckBreak(): Long {
        return 0L
    }

    override fun openSetCompleted(numFiles: Long, numBytes: Long): Long {
        return 0L
    }
}