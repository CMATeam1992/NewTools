package vn.cma.compress

import vn.cma.compress.data.ArchiveItemsList


data class ArchiveData(
	val list : List<ArchiveItemsList.ArchiveItem>,
	val errorPass : Boolean
)