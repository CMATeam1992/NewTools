package vn.cma.extract

interface ExtractCallback {
    fun guiGetPassword(): String?
    fun guiIsPasswordSet(): Boolean
    fun setCurrentFilePath(filePath: String?, numFilesCur: Long): Long
    fun setOperationResult(operationResult: Int, numFilesCur: Long, encrypted: Boolean): Long
    fun cryptoGetTextPassword(password: String?): String?
    fun setTotal(total: Long): Long
    fun setCompleted(value: Long): Long
    fun addErrorMessage(message: String?)
}

interface DeleteCallBack {

}