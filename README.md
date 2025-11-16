Serial Port NDK project for building libserial_port.so for multiple ABIs (including x86_64).

Files:
- serial_port_jni.c : JNI implementation (uses JNI names found in provided binary)
- Android.mk, Application.mk : for ndk-build
- CMakeLists.txt : for CMake/Android Studio

GitHub Actions workflow included under .github/workflows/build.yml to build using Android NDK.

Usage:
1) Push this project to a new GitHub repository.
2) On GitHub, go to Actions and run the workflow "build-ndk.yml".
3) After the workflow completes, download the artifact named "libs" which contains built .so files.

Important:
- The workflow downloads Android NDK r21e and uses ndk-build to compile. If Google changes download URLs you may need to update the workflow.
