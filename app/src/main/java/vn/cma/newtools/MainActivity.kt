package vn.cma.newtools

import android.content.Intent
import android.net.Uri
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.util.Log
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers
import io.reactivex.rxjava3.core.Single
import io.reactivex.rxjava3.schedulers.Schedulers
import vn.cma.extract.ExtractCallback
import vn.cma.extract.action.SmartExtract
import vn.cma.newtools.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity(), ExtractCallback {

    private lateinit var binding: ActivityMainBinding

    private var pass = "123456"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        //val filePath = "/storage/emulated/0/ZipExtractor/Compressed/testpass.zip"
        val filePath = "/storage/emulated/0/CMA_Zip/Compressed/testApp333.zip"
//        val list = SmartExtract.getListFileInArchive(filePath)
        Single.fromCallable {
            SmartExtract.getListFileInArchive(filePath)
        }.map {
            it.list
        }.subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe({
                it.forEach {
                    Log.e(
                        "AAAAA result = ",
                        it.itemPath.toString() + " Size = " + it.itemPackedSize + "Size2 = " + it.itemUnPackedSize
                    )
                }
            }, {

            })

        binding.text.setOnClickListener {
            startActivity(ZipActivity.createIntent(this))
        }
//        ExtractUtils.extractFile(
//            filePath,
//            "/storage/emulated/0/ZipExtractor/Extracted/ATestPass6",
//            this
//        )
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            checkPermission()
        }
    }

    @RequiresApi(Build.VERSION_CODES.R)
    private fun checkPermission() {
        if (!Environment.isExternalStorageManager()) {
            requestStorageManager()
        }
    }
    @RequiresApi(Build.VERSION_CODES.R)
    private fun requestStorageManager() {
        val intent = Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
        intent.addCategory("android.intent.category.DEFAULT")
        intent.data = Uri.parse(String.format("package:%s", packageName))
        intent.putExtra(Intent.EXTRA_RETURN_RESULT, true)
        startForResult.launch(intent)
    }

    @RequiresApi(Build.VERSION_CODES.R)
    private val startForResult =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult())
        {
            if (Environment.isExternalStorageManager()) {

            }
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