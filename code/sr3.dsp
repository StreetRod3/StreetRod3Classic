# Microsoft Developer Studio Project File - Name="sr3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sr3 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sr3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sr3.mak" CFG="sr3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sr3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sr3 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sr3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/Release"
# PROP Intermediate_Dir "obj/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "sr3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj/Debug"
# PROP Intermediate_Dir "obj/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sr3 - Win32 Release"
# Name "sr3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\Cache.cpp
# End Source File
# Begin Source File

SOURCE=.\common\CEventQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\common\CInput.cpp
# End Source File
# Begin Source File

SOURCE=.\common\CommonFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ConfigHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\common\FindFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\InitSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\common\Keyword_Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\common\Loggin.cpp
# End Source File
# Begin Source File

SOURCE=.\common\MathLib.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\src\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StateManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TextureManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\bass.h
# End Source File
# Begin Source File

SOURCE=.\include\Cache.h
# End Source File
# Begin Source File

SOURCE=.\include\CEventQueue.h
# End Source File
# Begin Source File

SOURCE=.\include\CInput.h
# End Source File
# Begin Source File

SOURCE=.\include\coldet.h
# End Source File
# Begin Source File

SOURCE=.\include\CommonFunc.h
# End Source File
# Begin Source File

SOURCE=.\include\ConfigHandler.h
# End Source File
# Begin Source File

SOURCE=.\include\defs.h
# End Source File
# Begin Source File

SOURCE=.\include\Drawing.h
# End Source File
# Begin Source File

SOURCE=.\include\FindFile.h
# End Source File
# Begin Source File

SOURCE=.\include\KeywordManager.h
# End Source File
# Begin Source File

SOURCE=.\include\MathLib.h
# End Source File
# Begin Source File

SOURCE=.\include\Sound.h
# End Source File
# Begin Source File

SOURCE=.\include\StateManager.h
# End Source File
# Begin Source File

SOURCE=.\include\System.h
# End Source File
# Begin Source File

SOURCE=.\include\TextureManager.h
# End Source File
# Begin Source File

SOURCE=.\include\types.h
# End Source File
# End Group
# Begin Group "3D Files"

# PROP Default_Filter "h,cpp"
# Begin Source File

SOURCE=".\include\3D structures.h"
# End Source File
# Begin Source File

SOURCE=.\include\3D.h
# End Source File
# Begin Source File

SOURCE=.\src\CCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CCamera.h
# End Source File
# Begin Source File

SOURCE=.\src\CMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CMaterial.h
# End Source File
# Begin Source File

SOURCE=.\common\CMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CMatrix.h
# End Source File
# Begin Source File

SOURCE=.\common\CModel.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CModel.h
# End Source File
# Begin Source File

SOURCE=.\common\CQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CQuaternion.h
# End Source File
# Begin Source File

SOURCE=.\common\CRigidBody.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CRigidBody.h
# End Source File
# Begin Source File

SOURCE=.\include\CVec.h
# End Source File
# Begin Source File

SOURCE=.\src\Drawing.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Renderer.h
# End Source File
# End Group
# Begin Group "Library Files"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=.\lib\bass.lib
# End Source File
# Begin Source File

SOURCE=.\lib\SDL.lib
# End Source File
# Begin Source File

SOURCE=.\lib\SDL_image.lib
# End Source File
# Begin Source File

SOURCE=.\lib\SDLmain.lib
# End Source File
# Begin Source File

SOURCE=.\lib\coldet.lib
# End Source File
# End Group
# Begin Group "Game files"

# PROP Default_Filter "cpp,h"
# Begin Source File

SOURCE=.\common\CCar.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CCar.h
# End Source File
# Begin Source File

SOURCE=.\common\CCar_Driving.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CLocation.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CLocation.h
# End Source File
# Begin Source File

SOURCE=.\src\CNut.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CNut.h
# End Source File
# Begin Source File

SOURCE=.\src\COpponent.cpp
# End Source File
# Begin Source File

SOURCE=.\include\COpponent.h
# End Source File
# Begin Source File

SOURCE=.\common\CPart.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CPart.h
# End Source File
# Begin Source File

SOURCE=.\common\CPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CPlayer.h
# End Source File
# Begin Source File

SOURCE=.\src\CRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CRadio.h
# End Source File
# Begin Source File

SOURCE=.\src\CSceneScript.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CSceneScript.h
# End Source File
# Begin Source File

SOURCE=.\src\CSidebar.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CSidebar.h
# End Source File
# Begin Source File

SOURCE=.\src\CSky.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CSky.h
# End Source File
# Begin Source File

SOURCE=.\src\CTaskBar.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CTaskBar.h
# End Source File
# Begin Source File

SOURCE=.\src\Diner.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Diner.h
# End Source File
# Begin Source File

SOURCE=.\src\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Entity.h
# End Source File
# Begin Source File

SOURCE=.\src\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Game.h
# End Source File
# Begin Source File

SOURCE=.\src\Gar_Parts.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Garage.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Garage.h
# End Source File
# Begin Source File

SOURCE=.\src\Garage_Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Garage_Game.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Garage_Newspaper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Menu.h
# End Source File
# Begin Source File

SOURCE=.\common\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Options.h
# End Source File
# Begin Source File

SOURCE=.\src\PartPool.cpp
# End Source File
# Begin Source File

SOURCE=.\include\PartPool.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter "cpp,h"
# Begin Source File

SOURCE=.\src\CButton.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CButton.h
# End Source File
# Begin Source File

SOURCE=.\src\CCheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CCheckbox.h
# End Source File
# Begin Source File

SOURCE=.\src\CColourPicker.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CColourPicker.h
# End Source File
# Begin Source File

SOURCE=.\src\CColourSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CColourSlider.h
# End Source File
# Begin Source File

SOURCE=.\src\CDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CDialog.h
# End Source File
# Begin Source File

SOURCE=.\src\CGuiLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CGuiLayout.h
# End Source File
# Begin Source File

SOURCE=.\src\CLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CLabel.h
# End Source File
# Begin Source File

SOURCE=.\src\CListview.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CListview.h
# End Source File
# Begin Source File

SOURCE=.\src\CScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\src\CSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CSlider.h
# End Source File
# Begin Source File

SOURCE=.\src\CTextbox.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CTextbox.h
# End Source File
# Begin Source File

SOURCE=.\src\CWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\font.cpp
# End Source File
# Begin Source File

SOURCE=.\include\font.h
# End Source File
# Begin Source File

SOURCE=.\src\gui.cpp
# End Source File
# Begin Source File

SOURCE=.\include\gui.h
# End Source File
# Begin Source File

SOURCE=.\src\text.cpp
# End Source File
# Begin Source File

SOURCE=.\include\text.h
# End Source File
# End Group
# Begin Group "City"

# PROP Default_Filter "cpp,h"
# Begin Source File

SOURCE=.\common\CarSimulation.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CarSimulation.h
# End Source File
# Begin Source File

SOURCE=.\src\CLightManager.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CLightManager.h
# End Source File
# Begin Source File

SOURCE=.\src\CQuadTree.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CQuadTree.h
# End Source File
# Begin Source File

SOURCE=.\src\Race_main.cpp
# End Source File
# Begin Source File

SOURCE=.\include\racing.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\config.cfg
# End Source File
# Begin Source File

SOURCE=.\sr3_1.ico
# End Source File
# Begin Source File

SOURCE=.\sr3_2.ico
# End Source File
# End Target
# End Project
