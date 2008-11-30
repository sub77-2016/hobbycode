# Microsoft Developer Studio Project File - Name="opal_ode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=opal_ode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opal_ode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opal_ode.mak" CFG="opal_ode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opal_ode - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "opal_ode - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opal_ode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPAL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\src\external\ode\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPAL_DLL_EXPORTING" /D "OPAL_USE_DOUBLE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ode.lib opcode.lib /nologo /dll /machine:I386 /libpath:"ode\release" /libpath:"opcode\release"

!ELSEIF  "$(CFG)" == "opal_ode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPAL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\src\external\ode\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPAL_DLL_EXPORTING" /D "OPAL_USE_DOUBLE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ode.lib opcode.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"ode\debug" /libpath:"opcode\debug"

!ENDIF 

# Begin Target

# Name "opal_ode - Win32 Release"
# Name "opal_ode - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\AccelerationSensor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\AttractorMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Blueprint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\BlueprintInstance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\BlueprintManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CollisionEventHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\GearedMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\InclineSensor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Joint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Logger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Motor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Point3r.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\RaycastSensor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Sensor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ServoMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Simulator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Solid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\SolidData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Space.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\SpringMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ThrusterMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Vec3r.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VelocityMotor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VolumeSensor.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\AccelerationSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\AccelerationSensorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\AttractorMotor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\AttractorMotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Blueprint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BlueprintInstance.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BlueprintManager.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BoxShapeData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CapsuleShapeData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CollisionEventHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Defines.h
# End Source File
# Begin Source File

SOURCE=..\..\src\EventHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\GearedMotor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\GearedMotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\InclineSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\InclineSensorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Joint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\JointBreakEventHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\JointData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Logger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Mass.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Matrix44r.h
# End Source File
# Begin Source File

SOURCE=..\..\src\MeshShapeData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Motor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\MotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\MovementEventHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\OpalMath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\PlaneShapeData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Point3r.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Portability.h
# End Source File
# Begin Source File

SOURCE=..\..\src\PostStepEventHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Quaternion.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RaycastSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RaycastSensorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Rayr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Sensor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SensorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ServoMotor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ServoMotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Simulator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Singleton.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Solid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SolidData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Space.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SphereShapeData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SpringMotor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SpringMotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ThrusterMotor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ThrusterMotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Vec3r.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VelocityMotor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VelocityMotorData.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VolumeSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VolumeSensorData.h
# End Source File
# End Group
# Begin Group "ODE-Specific"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ODE\ODEJoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODEJoint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODESimulator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODESimulator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODESolid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODESolid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODESpace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODESpace.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ODE\ODETools.h
# End Source File
# End Group
# Begin Group "TinyXml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\external\tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\external\tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\external\tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\external\tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\..\src\external\tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\external\tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# End Target
# End Project
