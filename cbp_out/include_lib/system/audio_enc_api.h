#ifndef audio_enc_api_h__
#define audio_enc_api_h__

#ifndef u8
#define u8  unsigned char
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef u32
#define u32 unsigned int
#endif

#ifndef s16
#define s16 signed short
#endif

extern int ima_code_buf();
extern void ima_code_init(void *workBuf);
extern int ima_code_run(void *work_buf, short *in, u8 *out, int npoint); // npoint 为输入样本长度， 输出长度为 npoint >> 1

#endif // audio_enc_api_h__