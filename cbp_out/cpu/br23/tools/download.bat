@echo off

@echo ********************************************************************************
@echo 			SDK BR23			
@echo ********************************************************************************
@echo %date%

cd  %~dp0



set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set ELFFILE=sdk.elf

REM %OBJDUMP% -D -address-mask=0x1ffffff -print-dbg $1.elf > $1.lst
%OBJCOPY% -O binary -j .text %ELFFILE% text.bin
%OBJCOPY% -O binary -j .data %ELFFILE% data.bin


%OBJDUMP% -section-headers -address-mask=0x1ffffff %ELFFILE%
%OBJDUMP% -t %ELFFILE% >  symbol_tbl.txt

copy /b text.bin+data.bin app.bin


::if exist uboot.boot del uboot.boot
::type uboot.bin > uboot.boot

::copy rslib.bin .\res\
::copy bt_cfg.bin .\res\
::copy dspboot.bin .\res\
::copy sbc_dec.bin .\res\
::copy mp3_dec_lib.bin .\res\
::copy aac_dec_dsp.bin .\res\
::copy eq.bflt .\res\
::copy commproc.bflt .\res\
::copy config.cfg .\res\
::copy convert.bflt .\res\




isd_download.exe -tonorflash -dev br23 -boot 0x12000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -res tone.cfg -key 168-AC690X-FA76.key -format all
:: -format all
::-reboot 2500







@rem ɾ����ʱ�ļ�-format all
if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty



@rem ���ɹ̼������ļ�
fw_add.exe -noenc -fw jl_isd.fw  -add ota.bin -type 100 -out jl_isd.fw
@rem ������ýű��İ汾��Ϣ�� FW �ļ���
fw_add.exe -noenc -fw jl_isd.fw -add script.ver -out jl_isd.fw


ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw

@REM ���������ļ������ļ�
::ufw_maker.exe -chip AC800X %ADD_KEY% -output config.ufw -res bt_cfg.cfg

::IF EXIST jl_695x.bin del jl_695x.bin 

@rem ����У����
out_cfg.bat

@rem ��������˵��
@rem -format vm        //����VM ����
@rem -format cfg       //����BT CFG ����
@rem -format 0x3f0-2   //��ʾ�ӵ� 0x3f0 �� sector ��ʼ�������� 2 �� sector(��һ������Ϊ16���ƻ�10���ƶ��ɣ��ڶ�������������10����)

ping /n 2 127.1>null
IF EXIST null del null
::pause

