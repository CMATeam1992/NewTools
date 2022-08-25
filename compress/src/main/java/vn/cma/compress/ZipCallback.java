package vn.cma.compress;

public interface ZipCallback {
	void addErrorMessage(String message);
	long setNumFiles(long numFiles);
	long setTotal(long total);
	long setCompleted(long completeValue);
	long getStream(String name, boolean  isAnti );
}
