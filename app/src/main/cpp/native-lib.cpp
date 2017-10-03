#include <jni.h>
#include <string>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "JHashMap.h"

using namespace std;
using namespace cv;

const float inlier_threshold = 2.5f; // Distance threshold to identify inliers
const float nn_match_ratio = 0.8f;   // Nearest neighbor matching ratio

template <typename T>
std::string to_string(T value){
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

extern "C"
{
    JNIEXPORT jobject JNICALL Java_com_dbeqiraj_akaze_MainActivity_compareImages(JNIEnv *env, jclass instance,
                                                                               jstring p1, jstring p2, jstring h, jlong mat) {
        // Extact full path for img1
        const char *ns1 = env->GetStringUTFChars(p1, JNI_FALSE);
        // Extact full path for img2
        const char *ns2 = env->GetStringUTFChars(p2, JNI_FALSE);
        // Extact full path for homography
        const char *nsh = env->GetStringUTFChars(h, JNI_FALSE);

        Mat img1 = imread(ns1, IMREAD_GRAYSCALE);
        Mat img2 = imread(ns2, IMREAD_GRAYSCALE);
        Mat homography;
        FileStorage fs(nsh, FileStorage::READ);
        fs.getFirstTopLevelNode() >> homography;

        // Release resources
        env->ReleaseStringUTFChars(p1, ns1);
        env->ReleaseStringUTFChars(p2, ns2);
        env->ReleaseStringUTFChars(h, nsh);

        vector<KeyPoint> kpts1, kpts2;
        Mat desc1, desc2;
        Ptr<AKAZE> akaze = AKAZE::create();
        akaze->detectAndCompute(img1, noArray(), kpts1, desc1);
        akaze->detectAndCompute(img2, noArray(), kpts2, desc2);
        BFMatcher matcher(NORM_HAMMING);
        vector< vector<DMatch> > nn_matches;
        matcher.knnMatch(desc1, desc2, nn_matches, 2);
        vector<KeyPoint> matched1, matched2, inliers1, inliers2;
        vector<DMatch> good_matches;
        for(size_t i = 0; i < nn_matches.size(); i++) {
            DMatch first = nn_matches[i][0];
            float dist1 = nn_matches[i][0].distance;
            float dist2 = nn_matches[i][1].distance;
            if(dist1 < nn_match_ratio * dist2) {
                matched1.push_back(kpts1[first.queryIdx]);
                matched2.push_back(kpts2[first.trainIdx]);
            }
        }
        for(unsigned i = 0; i < matched1.size(); i++) {
            Mat col = Mat::ones(3, 1, CV_64F);
            col.at<double>(0) = matched1[i].pt.x;
            col.at<double>(1) = matched1[i].pt.y;
            col = homography * col;
            col /= col.at<double>(2);
            double dist = sqrt( pow(col.at<double>(0) - matched2[i].pt.x, 2) +
                                pow(col.at<double>(1) - matched2[i].pt.y, 2));
            if(dist < inlier_threshold) {
                int new_i = static_cast<int>(inliers1.size());
                inliers1.push_back(matched1[i]);
                inliers2.push_back(matched2[i]);
                good_matches.push_back(DMatch(new_i, new_i, 0));
            }
        }

        Mat* res=(Mat*) mat;
        drawMatches(img1, inliers1, img2, inliers2, good_matches, *res);
        imwrite("akaze_result.png", *res);

        double inlier_ratio = inliers1.size() * 1.0 / matched1.size();

        JHashMap map;
        map.insert("keypoints1", to_string(kpts1.size()));
        map.insert("keypoints2", to_string(kpts2.size()));
        map.insert("matches", to_string(matched1.size()));
        map.insert("inliers", to_string(inliers1.size()));
        map.insert("inliers_ratio", to_string(inlier_ratio));

        return map.toJObject(env, instance);
    }

    void JHashMap::insert( std::string key, std::string value ) {
        map.insert( pair<string,string>( key, value ) );
    }

    jobject JHashMap::toJObject( JNIEnv* env, jclass ) {

        jclass hashMapClass= env->FindClass( "java/util/HashMap");
        jmethodID hashMapInit = env->GetMethodID(hashMapClass, "<init>", "(I)V");
        jobject hashMapObj = env->NewObject(hashMapClass, hashMapInit, map.size());
        jmethodID putMethod = env->GetMethodID(hashMapClass, "put",
                                                "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        for( auto it = map.begin(); it != map.end(); ++it )
        {
            jstring key = (*env).NewStringUTF( (*it).first.c_str() );
            jstring value = (*env).NewStringUTF( (*it).second.c_str() );

            (*env).CallObjectMethod(
                    hashMapObj,
                    putMethod,
                    key,
                    value
            );
        }

        return hashMapObj;
    }
}

