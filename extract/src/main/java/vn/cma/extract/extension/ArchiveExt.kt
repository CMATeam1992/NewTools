package vn.cma.extract.extension

import vn.cma.extract.Archive
import vn.cma.extract.data.ArchiveData


// input file path
// format pass  is "-p123456"
fun String.getListFileInZip(password : String = "123456"): ArchiveData {
    return try {
        Archive().getArchiveItemsList(this, "-p$password")
    } catch (e: Exception) {
        throw RuntimeException(e.message)
    }
}