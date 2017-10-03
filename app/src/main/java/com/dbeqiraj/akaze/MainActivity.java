package com.dbeqiraj.akaze;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.apache.commons.lang3.ArrayUtils;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        List<String> permissions = new ArrayList<>();
        if ( Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP_MR1 && !permissionIsGranted(this, Manifest.permission.READ_EXTERNAL_STORAGE)) {
            permissions.add(Manifest.permission.READ_EXTERNAL_STORAGE);
        }

        if ( Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP_MR1 && permissions.size() > 0 ) {
            requestPermissions( permissions.toArray(new String[permissions.size()]), 1);
        } else {
            compare();
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native HashMap compareImages(String path_img1, String path_img2, String path_h, long mat);


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String permissions[], @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length > 0 && ArrayUtils.contains(grantResults, PackageManager.PERMISSION_GRANTED)) {
           compare();
        } else {
            Toast.makeText(getApplicationContext(), getString(R.string.permission_required), Toast.LENGTH_SHORT).show();
        }
    }

    private void compare(){
        String downloads = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();

        // Build the full path for all required resources
        String path_img1 = downloads + "/graf1.png";
        String path_img2 = downloads + "/graf3.png";
        String path_h = downloads + "/h1to3p.xml";

        Mat matResult = new Mat();

        HashMap map = compareImages(path_img1, path_img2, path_h, matResult.getNativeObjAddr());

        String statistics = "A-KAZE Matching Results\n";
        statistics += "# Keypoints 1:\t" + map.get("keypoints1") + "\n";
        statistics += "# Keypoints 2:\t" + map.get("keypoints2") + "\n";
        statistics += "# Matches:\t" + map.get("matches") + "\n";
        statistics += "# Inliers:\t" + map.get("inliers") + "\n";
        statistics += "# Inliers Ratio:\t" + map.get("inliers_ratio") + "\n";

        Bitmap bitmap = Bitmap.createBitmap(matResult.cols(), matResult.rows(),Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(matResult, bitmap);

        ((ImageView) findViewById(R.id.result)).setImageBitmap(bitmap);
        ((TextView) findViewById(R.id.statistics)).setText(statistics);
    }

    private static boolean permissionIsGranted(Context context, String permission) {
        int res = ContextCompat.checkSelfPermission(context, permission);
        return ( res == PackageManager.PERMISSION_GRANTED);
    }
}
