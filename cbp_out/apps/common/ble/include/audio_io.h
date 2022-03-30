#ifndef __RESAMPLE_8_H__
#define __RESAMPLE_8_H__


/* Accuracy */

#define Npc8 2
#define SUM_coff_Log8     13            //ϵ����
#define SUM_coff8     (1<<SUM_coff_Log8)             //ϵ����


#define	 Step_Factor8  8

#define  Scl_LogCoff8  13



#define RS_UP_FILT_PHASE8 8
#define RS_DOWN_FILT_LEN8 8
#define RS_DOWN_TAB_LEN8  4
#define RS_BUF_LEN8       24
#define OBUF_SIZE8        32*6

#define RS_INDATA_LEN8 32

#define RS_SIZE8 896     //256


//ÿ��RSP_CONTEXTֻ���һ������������ж�������������Ͱ�ÿ����������һ��������case
typedef struct RSP8_CONTEXT
{
	short  bufL[RS_BUF_LEN8 + 1];
	short  bufR[RS_BUF_LEN8 + 1];

	unsigned short  insample;
	unsigned short  outsample;    //in_out sample rate

	short  LpScl;                 //downSampleʱ���������ű�����,upSampleʱ�������λ�Ǹ����ӵ�

	int  coff_index;     //��������������λ��
	int  phase;
	int  sample_index;

	short nch;

	short obuf[OBUF_SIZE8 * 2];
	short obuf_cnt;

	short in_cnt;

} RSP8_CONTEXT;

extern int get_resample8_buf();
extern void resample8_init(void *workBuf, int inSampleRate, int outSampleRate, int channel);
extern int resample8_run(void *workBuf, short *inbuf, short len);

#endif