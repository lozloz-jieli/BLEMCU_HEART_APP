@echo off & setlocal enabledelayedexpansion 

IF EXIST FW (
cd ./FW/
del /Q *.*
cd ../
)
REM ==================================================================================================================
SET WORK_DIR="zch"

SET SOFTWARE_DIR="NULL"

SET IC_TYPE_FLAG="CHIP_TYPE"

SET CUSTOMER="JMY"

SET PORJECT_NAME="NULL"

SET IC_TYPE="AC695X"

SET HEAD_PATH="NULL"

REM ==================================================================================================================

rename.exe -i !IC_TYPE_FLAG! -w !WORK_DIR! -s !SOFTWARE_DIR! -c !CUSTOMER! -p !PORJECT_NAME! -t !IC_TYPE! -h !HEAD_PATH!

::pause
