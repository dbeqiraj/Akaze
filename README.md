# Akaze
Feature matching with Akaze (OpenCV). Implementation in Android.

## Description

This is an implentation of feature matching using Akaze from OpenCV in Android. 
In the documentation of OpenCV and other sources there are many examples in C++ or Python but, when it comes to Java or Android
many developers face problems about how to use C++ code in Android, how to pass complex objects (here you can see how to pass a <i>HashMap</i>) as parameters 
from Java to native code and vice versa, etc.

Starting from <a target="_blank" href="http://docs.opencv.org/3.3.0/db/d70/tutorial_akaze_matching.html">this</a>, I tried to bring the same output in Android.

## Output
  <img src="http://i.imgur.com/oQumb8V.png" width="350"/>
  
## Setup

<ol>
  <li>Download/Clone the project</li>
  <li>Download OpenCv <i>Android Pack</i> from <a target="_blank" href="http://opencv.org/releases.html">here</a> (I have used release 3.3.0)</li>
  <li>In your android project create two folders: <b>jniLibs</b> and <b>cppThirdParty</b> in the main directory. 
  You can use other names (most likely for <b>cppThirdParty</b> folder) but then you have to change the references in <b>CMakeLists.txt</b> and <b>build.gradle</b></li>
  <li>Browse to your <i>Android Pack</i> folder downloaded in step 2 and copy <i>libs</i> <b>content</b> ( folders in <i>sdk/native/libs</i>) 
  to the folder <b>jniLibs</b></li>
  <li>Browse to your <i>Android Pack</i> folder downloaded in step 2 and copy <i>include</i> (<i>sdk/native/jni/include</i>) 
  to the folder <b>cppThirdParty</b>. You should have this tree folder: <i>cppThirdParty/include/[content_of_include_folder]</i> </li>
  <li>You will need 3 file resources to successfully run the project: 2 images to compare and 1 xml file for the <b>homography</b> object
  . These files should be located under <b>Download</b> folder of <b>device storage</b>. You can change the location but then, you will have 
  also to change the references in <b>MainActivity.java</b>. You can find the resources <a target="_blank" href="https://github.com/dbeqiraj/Akaze/tree/master/resources">here</a></li>
  <li>Now, if you have followed <b>all</b> these steps you should be able to successfully build and run the App</li>
</ol>
