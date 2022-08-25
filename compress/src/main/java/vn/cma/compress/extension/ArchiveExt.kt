package vn.cma.compress.extension

import vn.cma.compress.Archive
import vn.cma.compress.ArchiveData


// input file path
// format pass  is "-p123456"
fun String.getListFileInZip(password : String = "123456"): ArchiveData {
    return try {
        Archive().getArchiveItemsList(this, "-p$password")
    } catch (e: Exception) {
        throw RuntimeException(e.message)
    }
}