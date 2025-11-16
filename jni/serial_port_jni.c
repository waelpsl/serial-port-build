// serial_port_jni.c
#include <jni.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

static speed_t getBaudrate(int baudrate) {
    switch (baudrate) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
#ifdef B460800
        case 460800: return B460800;
#endif
#ifdef B921600
        case 921600: return B921600;
#endif
        default: return B9600;
    }
}

/*
 JNI names chosen to match the binary you provided:
 Java_androidserialport_SerialPort_open
 Java_androidserialport_SerialPort_close
*/
JNIEXPORT jobject JNICALL
Java_androidserialport_SerialPort_open(JNIEnv *env, jclass clazz,
                                       jstring path, jint baudrate,
                                       jint dataBits, jint stopBits, jchar parity)
{
    const char *path_utf = (*env)->GetStringUTFChars(env, path, 0);
    if (path_utf == NULL) return NULL;

    int flags = O_RDWR | O_NOCTTY | O_NONBLOCK;
    int fd = open(path_utf, flags);
    if (fd == -1) {
        (*env)->ReleaseStringUTFChars(env, path, path_utf);
        return NULL;
    }

    struct termios cfg;
    if (tcgetattr(fd, &cfg) != 0) {
        close(fd);
        (*env)->ReleaseStringUTFChars(env, path, path_utf);
        return NULL;
    }

    speed_t speed = getBaudrate((int)baudrate);
    cfsetispeed(&cfg, speed);
    cfsetospeed(&cfg, speed);

    cfg.c_cflag &= ~CSIZE;
    switch (dataBits) {
        case 6: cfg.c_cflag |= CS6; break;
        case 7: cfg.c_cflag |= CS7; break;
        case 8:
        default: cfg.c_cflag |= CS8; break;
    }

    if (stopBits == 2) cfg.c_cflag |= CSTOPB;
    else cfg.c_cflag &= ~CSTOPB;

    if (parity == 'N') {
        cfg.c_cflag &= ~PARENB;
    } else if (parity == 'O') {
        cfg.c_cflag |= PARENB;
        cfg.c_cflag |= PARODD;
    } else if (parity == 'E') {
        cfg.c_cflag |= PARENB;
        cfg.c_cflag &= ~PARODD;
    }

    cfg.c_iflag &= ~(INPCK | ISTRIP | IXON | IXOFF | IXANY);
    cfg.c_oflag &= ~OPOST;
    cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    cfg.c_cc[VMIN] = 1;
    cfg.c_cc[VTIME] = 0;

    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &cfg) != 0) {
        close(fd);
        (*env)->ReleaseStringUTFChars(env, path, path_utf);
        return NULL;
    }

    jclass clsFileDescriptor = (*env)->FindClass(env, "java/io/FileDescriptor");
    jmethodID ctor = (*env)->GetMethodID(env, clsFileDescriptor, "<init>", "()V");
    jobject fileDescriptor = (*env)->NewObject(env, clsFileDescriptor, ctor);

    jfieldID fid = (*env)->GetFieldID(env, clsFileDescriptor, "descriptor", "I");
    if (fid == NULL) {
        fid = (*env)->GetFieldID(env, clsFileDescriptor, "fd", "I");
    }
    (*env)->SetIntField(env, fileDescriptor, fid, (jint)fd);

    (*env)->ReleaseStringUTFChars(env, path, path_utf);
    return fileDescriptor;
}

JNIEXPORT void JNICALL
Java_androidserialport_SerialPort_close(JNIEnv *env, jclass clazz, jobject fileDescriptor)
{
    if (fileDescriptor == NULL) return;

    jclass cls = (*env)->GetObjectClass(env, fileDescriptor);
    jfieldID fid = (*env)->GetFieldID(env, cls, "descriptor", "I");
    if (fid == NULL) {
        fid = (*env)->GetFieldID(env, cls, "fd", "I");
    }
    jint fd = (*env)->GetIntField(env, fileDescriptor, fid);
    if (fd >= 0) {
        close((int)fd);
    }
}