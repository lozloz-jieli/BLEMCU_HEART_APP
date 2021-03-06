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







@rem 删除临时文件-format all
if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty



@rem 生成固件升级文件
fw_add.exe -noenc -fw jl_isd.fw  -add ota.bin -type 100 -out jl_isd.fw
@rem 添加配置脚本的版本信息到 FW 文件中
fw_add.exe -noenc -fw jl_isd.fw -add script.ver -out jl_isd.fw


ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw

@REM 生成配置文件升级文件
::ufw_maker.exe -chip AC800X %ADD_KEY% -output config.ufw -res bt_cfg.cfg

::IF EXIST jl_695x.bin del jl_695x.bin 

@rem 生成校验难
out_cfg.bat

@rem 常用命令说明
@rem -format vm        //擦除VM 区域
@rem -format cfg       //擦除BT CFG 区域
@rem -format 0x3f0-2   //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)

ping /n 2 127.1>null
IF EXIST null del null
::pause

