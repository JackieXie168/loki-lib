if not exist obj\ mkdir obj

::cl -c -Zm200 -O2 -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -I"..\..\include\loki" -Foobj\ Test.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" obj\Test.obj 
::..\..\lib\SmallObj.obj ..\..\lib\Singleton.obj 