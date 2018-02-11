package ai.pointcloud.demo.depthcamera;

import android.content.Context;
import android.content.res.AssetManager;
import android.text.TextUtils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class FileStorageHelper {
    private static final String SEPARATOR = File.separator;

    /*
     * copy files in asstes to specific directory
     *
     * @param context
     * @param assetsPath
     * @param storagePath target directory
     */
    public static void copyFilesFromAssets(Context context, String assetsPath, String storagePath) {
        String temp = "";

        if (TextUtils.isEmpty(storagePath)) {
            return;
        } else if (storagePath.endsWith(SEPARATOR)) {
            storagePath = storagePath.substring(0, storagePath.length() - 1);
        }

        if (TextUtils.isEmpty(assetsPath) || assetsPath.equals(SEPARATOR)) {
            assetsPath = "";
        } else if (assetsPath.endsWith(SEPARATOR)) {
            assetsPath = assetsPath.substring(0, assetsPath.length() - 1);
        }

        AssetManager assetManager = context.getAssets();
        try {
            File file = new File(storagePath);
            if (!file.exists()) {//if dir not exist, then create it
                file.mkdirs();
            }

            // get all files and directory under assets
            String[] fileNames = assetManager.list(assetsPath);
            if (fileNames.length > 0) {//if directory is an .apk
                for (String fileName : fileNames) {
                    if (!TextUtils.isEmpty(assetsPath)) {
                        temp = assetsPath + SEPARATOR + fileName;//accomplish the path of file under assets
                    }

                    String[] childFileNames = assetManager.list(temp);
                    if (!TextUtils.isEmpty(temp) && childFileNames.length > 0) {//find out if it a directory
                        copyFilesFromAssets(context, temp, storagePath + SEPARATOR + fileName);
                    } else {//if it is a file
                        InputStream inputStream = assetManager.open(temp);
                        readInputStream(storagePath + SEPARATOR + fileName, inputStream);
                    }
                }
            } else {//if doc_test.txt OR apk/app_test.apk
                InputStream inputStream = assetManager.open(assetsPath);
                if (assetsPath.contains(SEPARATOR)) {//apk/app_test.apk
                    assetsPath = assetsPath.substring(assetsPath.lastIndexOf(SEPARATOR), assetsPath.length());
                }
                readInputStream(storagePath + SEPARATOR + assetsPath, inputStream);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /*
     * read from input stream and write to output stream
     *
     * @param storagePath target directory path
     * @param inputStream input stream
     */
    public static void readInputStream(String storagePath, InputStream inputStream) {
        File file = new File(storagePath);
        try {
            if (!file.exists()) {
                // create pipe object
                FileOutputStream fos = new FileOutputStream(file);
                // define the storage
                byte[] buffer = new byte[inputStream.available()];
                // start to read stream
                int lenght = 0;
                while ((lenght = inputStream.read(buffer)) != -1) {//read bytes from input stream
                    // write data to  output stream from buffer
                    fos.write(buffer, 0, lenght);
                }
                fos.flush();//flush the cache
                // close stream
                fos.close();
                inputStream.close();
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
