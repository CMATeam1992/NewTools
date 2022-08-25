/*
 * Copyright (c) 2021. Created by : JackyHieu.
 * Support: hieu1211.ictu@gmail.com
 */

package vn.cma.compress.utils.compression;


import vn.cma.compress.utils.Constants;

public class CFormatInfo
{
    public String Name;
    public int LevelsMask;
    public Constants.EMethodID[]MethodIDs;
    public int NumMethods;
    public boolean Filter;
    public boolean Solid;
    public boolean MultiThread;
    public boolean SFX;
    public boolean Encrypt;
    public boolean EncryptFileNames;

    public  CFormatInfo(String name, int levelsMask, Constants.EMethodID[] methodIDs,
                        int numMethods, boolean filter, boolean solid, boolean multiThread, boolean sFX,
                        boolean encrypt, boolean encryptFileNames)
    {
        Name=name;
        LevelsMask=levelsMask;
        MethodIDs=methodIDs;
        NumMethods=numMethods;
        Filter=filter;
        Solid=solid;
        MultiThread=multiThread;
        SFX=sFX;
        Encrypt=encrypt;
        EncryptFileNames=encryptFileNames;
    }
}
