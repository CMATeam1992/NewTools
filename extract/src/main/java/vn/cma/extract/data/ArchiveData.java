package vn.cma.extract.data;

import java.util.List;

public class ArchiveData {
    public List<ArchiveItemsList.ArchiveItem> list;
    public Boolean errorPass;

    public ArchiveData(List<ArchiveItemsList.ArchiveItem> list, Boolean errorPass) {
        this.list = list;
        this.errorPass = errorPass;
    }
}
