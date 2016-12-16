@REM *********************************************************************
@REM **** Sets up the environment so that the CLR Runtime Profiling 
@REM **** Services are activated.
@REM ****
@REM **** The batch file will setup the profiling environment and register
@REM **** the profiler.  
@REM ****   
@REM **** Running any managed program or any unmanaged client that is 
@REM **** using a managed server will cause the EE to send notifications 
@REM **** to the registered profiler.
@REM ****
@REM *********************************************************************

@set DBG_PRF_LOG=0x1
@set Cor_Enable_Profiling=0x1
@regsvr32 /s Debug\ProfilerHST.dll
@set COR_PROFILER={FA0C6E77-AFF4-4c74-95F9-6216AFDE992C}

@echo on  
@set Cor_Enable_Profiling
@@set COR_PROFILER
@@title Hot Spots Tracker Profiler
