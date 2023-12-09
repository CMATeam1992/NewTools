package vn.cma.newtools

import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.util.Log
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers
import io.reactivex.rxjava3.core.Single
import io.reactivex.rxjava3.schedulers.Schedulers
import vn.cma.extract.ExtractCallback
import vn.cma.extract.action.SmartExtract
import vn.cma.newtools.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity(), ExtractCallback {

    private lateinit var binding: ActivityMainBinding

    private var pass = "123456"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        //val filePath = "/storage/emulated/0/ZipExtractor/Compressed/testpass.zip"
        //  val filePath = "/storage/emulated/0/CMA_Zip/Compressed/testApp333.zip"
      //  val filePath = "/storage/emulated/0/CMA_Zip/Compressed/test cachtotal.zip"
        val filePath = "/storage/emulated/0/CMA_Zip/Compressed/test cachpass2.zip"
        val outFolder = "/storage/emulated/0/CMA_Zip/Compressed/Test3339"
        if (File(outFolder).exists().not()) {
            File(outFolder).mkdirs()
        }
        // val fileExtract = "testpass_20230331"
        val fileExtract = "test cach1.zip"
        //  val file2 = "2023_12_04_16_24_30.mp3"
        val file2 = "test cach2pasds.zip"
        val listOf = listOf(fileExtract, file2)
//        val list = SmartExtract.getListFileInArchive(filePath)
        Single.fromCallable {
            //        SmartExtract.getListFileInArchive(filePath)
            SmartExtract.extractMultipleFileInZip(
                filePath,
                listOf,
                outFolder,
                "123456",
                object : ExtractCallback {
                    override fun guiGetPassword(): String? {
                        return ""
                    }

                    override fun guiIsPasswordSet(): Boolean {
                        return false
                    }

                    override fun setCurrentFilePath(filePath: String?, numFilesCur: Long): Long {
                        return 0L
                    }

                    override fun setOperationResult(
                        operationResult: Int,
                        numFilesCur: Long,
                        encrypted: Boolean
                    ): Long {
                        return 0L
                    }

                    override fun cryptoGetTextPassword(password: String?): String? {
                        return ""
                    }

                    override fun setTotal(total: Long): Long {
                        return 0L
                    }

                    override fun setCompleted(value: Long): Long {
                        return 0L
                    }

                    override fun addErrorMessage(message: String?) {

                    }

                })
        }.subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe({
                if (it != 0) {
                    File(outFolder).deleteRecursively()
                }
//                it.list.forEach {
//                    Log.e("AAAAAA ===> success %s", it.itemPath)
//                }
                Log.e("AAAAAA ===> success %s", it.toString())
            }, {
                Log.e("AAAAAA === > %s", it.toString())
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