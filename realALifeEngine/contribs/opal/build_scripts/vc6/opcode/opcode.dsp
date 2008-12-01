# Microsoft Developer Studio Project File - Name="opcode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=opcode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opcode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opcode.mak" CFG="opcode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opcode - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "opcode - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opcode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\src\external\opcode" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "opcode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\src\external\opcode" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "opcode - Win32 Release"
# Name "opcode - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceAABB.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceHPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceIndexedTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceMatrix3x3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceMatrix4x4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceOBB.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IcePlane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IcePoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceRandom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceRay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceRevisitedRadix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_AABBCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_AABBTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_BaseModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_BoxPruning.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Collider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_HybridModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_LSSCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_MeshInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Model.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_OBBCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_OptimizedTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Picking.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_PlanesCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_RayCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_SphereCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_SweepAndPrune.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_TreeBuilders.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_TreeCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_VolumeCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Opcode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceAABB.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceAxes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceBoundingSphere.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceFPU.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceHPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceIndexedTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceLSS.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceMatrix3x3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceMatrix4x4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceMemoryMacros.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceOBB.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IcePairs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IcePlane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IcePoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IcePreprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceRandom.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceRay.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceRevisitedRadix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceSegment.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceTriList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Ice\IceUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_AABBCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_AABBTree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_BaseModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_BoxBoxOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_BoxPruning.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Collider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_HybridModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_IceHook.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_LSSAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_LSSCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_LSSTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_MeshInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Model.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_OBBCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_OptimizedTree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Picking.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_PlanesAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_PlanesCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_PlanesTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_RayAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_RayCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_RayTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_Settings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_SphereAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_SphereCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_SphereTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_SweepAndPrune.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_TreeBuilders.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_TreeCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_TriBoxOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_TriTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\OPC_VolumeCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Opcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\external\opcode\Stdafx.h
# End Source File
# End Group
# End Target
# End Project
