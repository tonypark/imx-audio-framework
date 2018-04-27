//*****************************************************************
// Copyright 2018 NXP
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*****************************************************************

#ifndef _DSP_WRAP_H_
#define _DSP_WRAP_H_

#include "fsl_unia.h"
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>
#include <malloc.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/types.h>
#include "xf-proxy.h"
#include "library_load.h"
#include "mxc_dsp.h"
#include "xaf-api.h"


/* ...size of auxiliary pool for communication with DSP */
#define XA_AUX_POOL_SIZE                32

/* ...length of auxiliary pool messages */
#define XA_AUX_POOL_MSG_LENGTH          128

/* ...number of max input buffers */
#define INBUF_SIZE                      4096
#define OUTBUF_SIZE                     16384


#define N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))

#define _STR(s)     #s
#define STR(s)      _STR(s)

/* Used to malloc inner buffer in create() function*/
#define AACD_6CH_FRAME_MAXLEN            (6*768)
#define MP3D_INPUT_BUF_PUSH_SIZE         (2048*4)

#define MP3D_FRAME_SIZE                  576
#define AAC_FRAME_SIZE                   1024


/* ANSI color print */
#define COLOR_RED       31
#define COLOR_GREEN     32
#define COLOR_YELLOW    33
#define COLOR_BLUE      34
#define COLOR_PURPLE    35
#define COLOR_CYAN      36

#define COLORFUL_STR(color, format, ...)\
    "\33[1;" STR(color) "m" format "\33[0m", ##__VA_ARGS__

#define YELLOW_STR(format,...)      COLORFUL_STR(COLOR_YELLOW,format, ##__VA_ARGS__)
#define RED_STR(format,...)         COLORFUL_STR(COLOR_RED,format, ##__VA_ARGS__)
#define GREEN_STR(format,...)       COLORFUL_STR(COLOR_GREEN,format, ##__VA_ARGS__)
#define BLUE_STR(format,...)        COLORFUL_STR(COLOR_BLUE,format,##__VA_ARGS__)
#define PURPLE_STR(format,...)      COLORFUL_STR(COLOR_PURPLE,format,##__VA_ARGS__)
#define CYAN_STR(format,...)        COLORFUL_STR(COLOR_CYAN,format,##__VA_ARGS__)

static uint32 aacd_1channel_layout[] = {
	/* FC */
	UA_CHANNEL_FRONT_CENTER,
};

static uint32 aacd_2channel_layout[] = {
	/* FL,FR */
	UA_CHANNEL_FRONT_LEFT,
	UA_CHANNEL_FRONT_RIGHT,
};

static uint32 aacd_3channel_layout[] = {
	/* FC,FL,FR */
	UA_CHANNEL_FRONT_CENTER,
	UA_CHANNEL_FRONT_LEFT,
	UA_CHANNEL_FRONT_RIGHT,
};

static uint32 aacd_4channel_layout[] = {
	/* FC,FCL,FCR,BC */
	UA_CHANNEL_FRONT_CENTER,
	UA_CHANNEL_FRONT_LEFT_CENTER,
	UA_CHANNEL_FRONT_RIGHT_CENTER,
	UA_CHANNEL_REAR_CENTER
};

static uint32 aacd_5channel_layout[] = {
	/* FC,FL,FR,BL,BR */
	UA_CHANNEL_FRONT_CENTER,
	UA_CHANNEL_FRONT_LEFT,
	UA_CHANNEL_FRONT_RIGHT,
	UA_CHANNEL_REAR_LEFT,
	UA_CHANNEL_REAR_RIGHT
};

static uint32 aacd_6channel_layout[] = {
	/* FC,FL,FR,BL,BR,LFE */
	UA_CHANNEL_FRONT_CENTER,
	UA_CHANNEL_FRONT_LEFT,
	UA_CHANNEL_FRONT_RIGHT,
	UA_CHANNEL_REAR_LEFT,
	UA_CHANNEL_REAR_RIGHT,
	UA_CHANNEL_LFE
};

static uint32 aacd_8channel_layout[] = {
	/* FC,FCL,FCR,SL,SR,BL,BR,LFE */
	UA_CHANNEL_FRONT_CENTER,
	UA_CHANNEL_FRONT_LEFT_CENTER,
	UA_CHANNEL_FRONT_RIGHT_CENTER,
	UA_CHANNEL_SIDE_LEFT,
	UA_CHANNEL_SIDE_RIGHT,
	UA_CHANNEL_REAR_LEFT,
	UA_CHANNEL_REAR_RIGHT,
	UA_CHANNEL_LFE
};

static uint32 * aacd_channel_layouts[] = {
	NULL,
	aacd_1channel_layout, // 1
	aacd_2channel_layout, // 2
	aacd_3channel_layout,
	aacd_4channel_layout,
	aacd_5channel_layout,
	aacd_6channel_layout,
	NULL,
	aacd_8channel_layout,
};


typedef struct {
    int32 ErrType;
    char * ErrMsg;
} lastErr;


typedef struct {
    uint8 * data;               /* inner buffer */
    uint32 inner_size;          /* current inner buffer size */
    uint32 inner_offset;        /* offset of inner buffer */
    uint32 buf_size;            /* total buf size */
    uint32 threshold;           /* threshold of the input buffer*/
}innerBuf;

typedef struct{
    UniACodecMemoryOps sMemOps;

    uint32 samplerate;
    uint32 channels;
    uint32 depth;
    bool framed;
    uint32 SampleBits;
    bool downmix;
    UniACodecParameterBuffer codecData;
    UniAcodecOutputPCMFormat outputFormat;
    char * codcDesc;
    char codec_name[256];
    int32 bitrate;

    STREAM_TYPE stream_type;

    uint32 mpeg_layer;
    uint32 mpeg_version;

    innerBuf  inner_buf;
    bool  initialized;
    int32 last_err;
    int32 frame_nb;
    uint32 consumed_length;
    CHAN_TABLE chan_map_tab;
    uint32 layout_bak[UA_CHANNEL_MAX];
    bool ID3flag;
    uint32 tagsize;

/****************DSP******************/
    xaf_adev_t adev;
    xaf_pipeline_t pipeline;
    xaf_comp_t component;

    uint8 *dsp_out_buf;
    AUDIOFORMAT codec_type;
    uint32 outbuf_alloc_size;
    uint32 last_output_size;
    bool memory_allocated;
    bool depth_is_set;
    bool input_over;
    bool inptr_busy, outptr_busy;

}DSP_Handle;

UA_ERROR_TYPE  InputBufHandle (innerBuf * inner_buf, uint8 *InputBuf,
                               uint32 InputSize,uint32 *offset, bool framed);
UA_ERROR_TYPE  ResetInnerBuf (innerBuf * inner_buf, uint32 buf_size, uint32 threshold);
UA_ERROR_TYPE SetDefaultFeature(UniACodec_Handle pua_handle);
void channel_pos_convert(UniACodec_Handle pua_handle, uint8 *data_in,
                          int32 frameSize, int32 channels, int32 depth);
void cancel_unused_channel_data(uint8 *data_in, int32 length, int32 depth);

int comp_process(UniACodec_Handle pua_handle, uint8 *input, uint32 in_size, uint32 *in_off, uint8 *output, uint32 *out_size);

const char * DSPDecVersionInfo();
UniACodec_Handle DSPDecCreate(UniACodecMemoryOps * memOps, AUDIOFORMAT type);
UA_ERROR_TYPE DSPDecDelete(UniACodec_Handle pua_handle);
UA_ERROR_TYPE DSPDecReset(UniACodec_Handle pua_handle);
UA_ERROR_TYPE DSPDecSetPara(UniACodec_Handle pua_handle, UA_ParaType ParaType, UniACodecParameter * parameter);
UA_ERROR_TYPE DSPDecGetPara(UniACodec_Handle pua_handle, UA_ParaType ParaType, UniACodecParameter * parameter);
UA_ERROR_TYPE DSPDecFrameDecode( UniACodec_Handle pua_handle, uint8 *InputBuf, uint32 InputSize, uint32 * offset, uint8 ** OutputBuf, uint32 * OutputSize);
char * DSPDecLastErr(UniACodec_Handle pua_handle);


#endif //_DSP_WRAP_H_