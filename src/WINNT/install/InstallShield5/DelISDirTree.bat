@echo off

rem
rem Copyright (C) 1998  Transarc Corporation.
rem All rights reserved.
rem
rem
rem This file deletes the IS5 dir tree.  First it will delete all the files we
rem know about.  Then it will delete the IS5 dirs.  For dirs that contain
rem output files that we don't care about, we deltree them.  For all other dirs
rem we use rmdir, and if IS5 added some file to one of those dirs, it will fail
rem to delete.  We can then look at the dir and see if the file needs to be added
rem to CML.

rem First delete all the files we know about.

del "Component Definitions\Default.cdf"
del "Component Definitions\Default.fgl"
del "File Groups\Default.fdf"
del "File Groups\GenFileGroups.bat"
del "File Groups\Server_Program_Files.fgl"
del "File Groups\Server_Common_Files.fgl"
del "File Groups\Server_WinDir_Files.fgl"
del "File Groups\Server_System32_Files.fgl"
del "File Groups\Client_Program_Files.fgl"
del "File Groups\Client_Common_Files.fgl"
del "File Groups\Client_WinDir_Files.fgl"
del "File Groups\Client_System32_Files.fgl"
del "File Groups\Client_Afs_Header_Files.fgl"
del "File Groups\Client_Rx_Header_Files.fgl"
del "File Groups\Client_Main_Header_Files.fgl"
del "File Groups\Client_Sample_Files.fgl"
del "File Groups\Light_Client_Program_Files.fgl"
del "File Groups\Light_Client_Common_Files.fgl"
del "File Groups\Light_Client_WinDir_Files.fgl"
del "File Groups\Light_Client_System32_Files.fgl"
del "File Groups\Light95_Client_Program_Files.fgl"
del "File Groups\Light95_Client_Common_Files.fgl"
del "File Groups\Light95_Client_WinDir_Files.fgl"
del "File Groups\Light95_Client_System32_Files.fgl"
del "File Groups\Control_Center_Program_Files.fgl"
del "File Groups\Control_Center_Common_Files.fgl"
del "File Groups\Control_Center_WinDir_Files.fgl"
del "File Groups\Control_Center_System32_Files.fgl"
del "File Groups\Cmd_Ref_English_Files.fgl"
del "File Groups\Install_Guide_English_Files.fgl"
del "File Groups\Release_Notes_English_Files.fgl"
del "File Groups\Sys_Admin_Guide_English_Files.fgl"
del "File Groups\Cmd_Ref_Japanese_Files.fgl"
del "File Groups\Install_Guide_Japanese_Files.fgl"
del "File Groups\Release_Notes_Japanese_Files.fgl"
del "File Groups\Sys_Admin_Guide_Japanese_Files.fgl"
del "File Groups\Cmd_Ref_Korean_Files.fgl"
del "File Groups\Install_Guide_Korean_Files.fgl"
del "File Groups\Release_Notes_Korean_Files.fgl"
del "File Groups\Sys_Admin_Guide_Korean_Files.fgl"
del "File Groups\Cmd_Ref_Trad_Chinese_Files.fgl"
del "File Groups\Install_Guide_Trad_Chinese_Files.fgl"
del "File Groups\Release_Notes_Trad_Chinese_Files.fgl"
del "File Groups\Sys_Admin_Guide_Trad_Chinese_Files.fgl"
del "File Groups\Cmd_Ref_Simp_Chinese_Files.fgl"
del "File Groups\Install_Guide_Simp_Chinese_Files.fgl"
del "File Groups\Release_Notes_Simp_Chinese_Files.fgl"
del "File Groups\Sys_Admin_Guide_Simp_Chinese_Files.fgl"
del "File Groups\Doc_Misc_English_Files.fgl"
del "File Groups\Doc_Misc_Japanese_Files.fgl"
del "File Groups\Doc_Misc_Korean_Files.fgl"
del "File Groups\Doc_Misc_Trad_Chinese_Files.fgl"
del "File Groups\Doc_Misc_Simp_Chinese_Files.fgl"
del "File Groups\Doc_Files.fgl"
del "File Groups\Lang_English_Files.fgl"
del "File Groups\Lang_Simp_Chinese_Files.fgl"
del "File Groups\Lang_Trad_Chinese_Files.fgl"
del "File Groups\Lang_Korean_Files.fgl"
del "File Groups\Lang_Japanese_Files.fgl"
del "Media\Transarc AFS\GenDefault.mda.bat"
del "Registry Entries\Default.rge"
del "Script Files\setup.rul"
del "Setup Files\Uncompressed Files\Language Independent\OS Independent\setup.bmp"
del "Setup Files\Compressed Files\Language Independent\OS Independent\InstallUtils.dll"
del "Setup Files\Compressed Files\Language Independent\OS Independent\afs_setup_utils_*.dll"
del "Shell Objects\Default.shl"
del "String Tables\Default.shl"
del "String Tables\0009-English\value.shl"
del "String Tables\0011-Japanese\value.shl"
del "String Tables\0012-Korean\value.shl"
del "String Tables\0404-Chinese (Taiwan)\value.shl"
del "String Tables\0804-Chinese (PRC)\value.shl"
del "String Tables\0007-German\value.shl"
del "String Tables\0416-Portuguese (Brazilian)\value.shl"
del "String Tables\000a-Spanish\value.shl"
del "Text Substitutions\Build.tsb"
del "Text Substitutions\Setup.tsb"
del "File Groups\Cmd_Ref_German_Files.fgl"
del "File Groups\Install_Guide_German_Files.fgl"
del "File Groups\Release_Notes_German_Files.fgl"
del "File Groups\Sys_Admin_Guide_German_Files.fgl"
del "File Groups\Doc_Misc_German_Files.fgl"
del "File Groups\Lang_German_Files.fgl"
del "File Groups\Cmd_Ref_Spanish_Files.fgl"
del "File Groups\Install_Guide_Spanish_Files.fgl"
del "File Groups\Release_Notes_Spanish_Files.fgl"
del "File Groups\Sys_Admin_Guide_Spanish_Files.fgl"
del "File Groups\Doc_Misc_Spanish_Files.fgl"
del "File Groups\Lang_Spanish_Files.fgl"
del "File Groups\Cmd_Ref_Portuguese_Files.fgl"
del "File Groups\Install_Guide_Portuguese_Files.fgl"
del "File Groups\Release_Notes_Portuguese_Files.fgl"
del "File Groups\Sys_Admin_Guide_Portuguese_Files.fgl"
del "File Groups\Doc_Misc_Portuguese_Files.fgl"
del "File Groups\Lang_Portuguese_Files.fgl"
del InstallShield5.ipr


rem Now delete the directories.

rmdir /s /q "Setup Files\Compressed Files\0009-English"
rmdir /s /q "Setup Files\Compressed Files\0011-Japanese"
rmdir /s /q "Setup Files\Compressed Files\0012-Korean"
rmdir /s /q "Setup Files\Compressed Files\0404-Chinese (Taiwan)"
rmdir /s /q "Setup Files\Compressed Files\0804-Chinese (PRC)"
rmdir /s /q "Setup Files\Compressed Files\0007-German"
rmdir /s /q "Setup Files\Compressed Files\0416-Portuguese (Brazilian)"
rmdir /s /q "Setup Files\Compressed Files\000a-Spanish"

rmdir /s /q "Setup Files\Uncompressed Files\0009-English"
rmdir /s /q "Setup Files\Uncompressed Files\0011-Japanese"
rmdir /s /q "Setup Files\Uncompressed Files\0012-Korean"
rmdir /s /q "Setup Files\Uncompressed Files\0404-Chinese (Taiwan)"
rmdir /s /q "Setup Files\Uncompressed Files\0804-Chinese (PRC)"
rmdir /s /q "Setup Files\Uncompressed Files\0007-German"
rmdir /s /q "Setup Files\Uncompressed Files\0416-Portuguese (Brazilian)"
rmdir /s /q "Setup Files\Uncompressed Files\000a-Spanish"

rmdir "Component Definitions"
rmdir "File Groups"
rmdir /s /q "Media\Transarc AFS"
rmdir Media
rmdir "Registry Entries"
rmdir /s /q "Script Files"
rmdir "Setup Files\Uncompressed Files\Language Independent\OS Independent"
rmdir "Setup Files\Uncompressed Files\Language Independent"
rmdir "Setup Files\Uncompressed Files"
rmdir "Setup Files\Compressed Files\Language Independent\OS Independent"
rmdir "Setup Files\Compressed Files\Language Independent"
rmdir "Setup Files\Compressed Files"
rmdir "Setup Files"
rmdir "Shell Objects"
rmdir "String Tables\0009-English"
rmdir "String Tables\0011-Japanese"
rmdir "String Tables\0012-Korean"
rmdir "String Tables\0404-Chinese (Taiwan)"
rmdir "String Tables\0804-Chinese (PRC)"
rmdir "String Tables\0007-German"
rmdir "String Tables\0416-Portuguese (Brazilian)"
rmdir "String Tables\000a-Spanish"
rmdir "String Tables"
rmdir "Text Substitutions"



