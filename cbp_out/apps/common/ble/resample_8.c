

/* Definitions */
#include <string.h>
#include "resample_8.h"

int get_resample8_buf()
{
	return sizeof(RSP8_CONTEXT);
}

void resample8_init(void *workBuf, audio_io *io, int inSampleRate, int outSampleRate, int channel)
{
	RSP8_CONTEXT *RspCont_obj8 = (RSP8_CONTEXT *)workBuf;
	memset(RspCont_obj8, 0, sizeof(RSP8_CONTEXT));
	memcpy(&RspCont_obj8->io, io, sizeof(audio_io));
	RspCont_obj8->nch = channel;
	RspCont_obj8->insample = inSampleRate;
	RspCont_obj8->outsample = outSampleRate;
	
	RspCont_obj8->sample_index = 0;
	if (inSampleRate > outSampleRate)
	{
		RspCont_obj8->LpScl = (outSampleRate << Scl_LogCoff8) / inSampleRate;
		RspCont_obj8->coff_index = ((outSampleRate*Npc8) << Step_Factor8) / inSampleRate;  //这里的4是Npc
		RspCont_obj8->phase = 16777216 / outSampleRate;
	}
	else
	{
		RspCont_obj8->LpScl = (134217728 / outSampleRate) + 1;
	}
}

static int clip_8(int a, int amin, int amax)
{
	if (a < amin)
		return amin;
	else if (a > amax)
		return amax;
	else
		return a;
}


const short rs_sample8_coff1[72] =
{
	0,    0,    0,32767,    1,    0,    0,    0,
	-22,  347,-2263,31759, 3482, -584,   50,   -1,
	-25,  484,-3379,28865, 8069,-1378,  134,   -2,
	-20,  472,-3559,24465,13443,-2275,  248,   -6,
	-12,  374,-3090,19112,19112,-3090,  374,  -12,
	-6,  248,-2275,13443,24465,-3559,  472,  -20,
	-2,  134,-1378, 8069,28865,-3379,  484,  -25,
	-1,   50, -584, 3482,31759,-2263,  347,  -22,
	0,    0,    0,    0,32767,    1,    0,    0
};


//上采样
static void Resample8Up(RSP8_CONTEXT *RspCont_obj8, short *inbuf, short len)
{

	short *bufL = RspCont_obj8->bufL;
	short *bufR = RspCont_obj8->bufR;

	int i, nch, coff, div_ou, out_data;

	nch = RspCont_obj8->nch - 1;
	div_ou = RspCont_obj8->LpScl;
	RspCont_obj8->in_cnt = len * RspCont_obj8->nch;
	while (1)
	{
		int tmp_data[2];
		while (RspCont_obj8->sample_index >= RspCont_obj8->outsample)
		{

			if (RspCont_obj8->in_cnt <= 0)
			{
				//check_out_remain8();
				return;
			}
			for (i = 0; i < RS_UP_FILT_PHASE8 - 1; i++)
				bufL[i] = bufL[i + 1];
			bufL[RS_UP_FILT_PHASE8 - 1] = inbuf[len*RspCont_obj8->nch - RspCont_obj8->in_cnt];
			RspCont_obj8->in_cnt--;
			if (nch)
			{
				for (i = 0; i < RS_UP_FILT_PHASE8 - 1; i++)
					bufR[i] = bufR[i + 1];
				bufR[RS_UP_FILT_PHASE8 - 1] = inbuf[len*RspCont_obj8->nch - RspCont_obj8->in_cnt];
				RspCont_obj8->in_cnt--;
			}
			RspCont_obj8->sample_index -= RspCont_obj8->outsample;
		}
		RspCont_obj8->coff_index = (RspCont_obj8->sample_index*div_ou) >> 24;
		RspCont_obj8->phase = ((RspCont_obj8->sample_index*div_ou) >> 16) & 0xff;

		tmp_data[0] = 0;
		tmp_data[1] = 0;
		for (i = 0; i < RS_UP_FILT_PHASE8; i++)
		{
			coff = (rs_sample8_coff1[RspCont_obj8->coff_index*RS_UP_FILT_PHASE8 + i] * (0x100 - RspCont_obj8->phase) +
				rs_sample8_coff1[(RspCont_obj8->coff_index + 1)*RS_UP_FILT_PHASE8 + i] * RspCont_obj8->phase + (1 << 7)) >> 8;
			tmp_data[0] += bufL[i] * coff;
			if (nch)
			{
				tmp_data[1] += bufR[i] * coff;
			}
		}

		tmp_data[0] >>= 15;
		out_data = clip_8(tmp_data[0], -32768, 32767);
		RspCont_obj8->obuf[RspCont_obj8->obuf_cnt++] = out_data;
		//check_outfun8(out_data);
		if (nch)
		{
			tmp_data[1] >>= 15;
			out_data = clip_8(tmp_data[1], -32768, 32767);
			RspCont_obj8->obuf[RspCont_obj8->obuf_cnt++] = out_data;
			//check_outfun8(out_data);
		}

		RspCont_obj8->sample_index += RspCont_obj8->insample;
	}

}


int resample8_run(void *workBuf, short *inbuf, short len)
{
	int outlen;
	RSP8_CONTEXT *RspCont_obj8 = (RSP8_CONTEXT *)workBuf;
	RspCont_obj8->obuf_cnt = 0;

	RspCont_obj8->in_cnt = 32 * RspCont_obj8->nch;
	if (RspCont_obj8->insample < RspCont_obj8->outsample)
	{
		Resample8Up(RspCont_obj8, inbuf, len);
	}
	
	if (RspCont_obj8->nch == 1)
	{
		outlen =  RspCont_obj8->obuf_cnt;
	}
	else
	{
		outlen = (RspCont_obj8->obuf_cnt >> 1);
	}

	RspCont_obj8->io.output(RspCont_obj8->io.priv, RspCont_obj8->obuf, outlen * 2 * RspCont_obj8->nch);

	return 0;
	
	
}
