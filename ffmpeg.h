#ifndef _INCLUDE_FFMPEG_H_
#define _INCLUDE_FFMPEG_H_

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include "config.h"

#define TIMELAPSE_NONE   0  /* No timelapse, regular processing */
#define TIMELAPSE_APPEND 1  /* Use append version of timelapse */
#define TIMELAPSE_NEW    2  /* Use create new file version of timelapse */

#ifdef HAVE_FFMPEG

#include <errno.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>

#if (LIBAVFORMAT_VERSION_MAJOR >= 56)

#define MY_PIX_FMT_YUV420P   AV_PIX_FMT_YUV420P
#define MY_PIX_FMT_YUVJ420P  AV_PIX_FMT_YUVJ420P
#define MyPixelFormat AVPixelFormat

#else

#define MY_PIX_FMT_YUV420P   PIX_FMT_YUV420P
#define MY_PIX_FMT_YUVJ420P  PIX_FMT_YUVJ420P
#define MyPixelFormat PixelFormat

#endif

struct ffmpeg {
    AVFormatContext *oc;
    AVStream *video_st;
    AVCodecContext *c;

    AVFrame *picture;       /* contains default image pointers */
    uint8_t *video_outbuf;
    int video_outbuf_size;

    void *udata;            /* U & V planes for greyscale images */
    int vbr;                /* variable bitrate setting */
    char codec[20];         /* codec name */
    int tlapse;
    int64_t last_pts;
    struct timeval start_time;
};

AVFrame *my_frame_alloc(void);
void my_frame_free(AVFrame *frame);
int ffmpeg_put_frame(struct ffmpeg *, AVFrame *, const struct timeval *tv1);
void ffmpeg_cleanups(struct ffmpeg *);
AVFrame *ffmpeg_prepare_frame(struct ffmpeg *, unsigned char *,
                              unsigned char *, unsigned char *);
int my_image_get_buffer_size(enum MyPixelFormat pix_fmt, int width, int height);
int my_image_copy_to_buffer(AVFrame *frame,uint8_t *buffer_ptr,enum MyPixelFormat pix_fmt,int width,int height,int dest_size);
int my_image_fill_arrays(AVFrame *frame,uint8_t *buffer_ptr,enum MyPixelFormat pix_fmt,int width,int height);
void my_packet_unref(AVPacket pkt);


#else /* No FFMPEG */

struct ffmpeg {
    void *udata;
    int dummy;
};

#endif /* HAVE_FFMPEG */

/* Now the functions that are ok for both situations */
void ffmpeg_init(void);
void ffmpeg_finalise(void);
struct ffmpeg *ffmpeg_open(
    const char *ffmpeg_video_codec,
    char *filename,
    unsigned char *y,    /* YUV420 Y plane */
    unsigned char *u,    /* YUV420 U plane */
    unsigned char *v,    /* YUV420 V plane */
    int width,
    int height,
    int rate,            /* framerate, fps */
    int bps,             /* bitrate; bits per second */
    int vbr,             /* variable bitrate */
    int tlapse,
    const struct timeval *tv1
    );
int ffmpeg_put_image(struct ffmpeg *, const struct timeval *tv1);
int ffmpeg_put_other_image(
    struct ffmpeg *ffmpeg,
    unsigned char *y,
    unsigned char *u,
    unsigned char *v,
    const struct timeval *tv1
    );
void ffmpeg_close(struct ffmpeg *);
void ffmpeg_avcodec_log(void *, int, const char *, va_list);


#endif /* _INCLUDE_FFMPEG_H_ */
