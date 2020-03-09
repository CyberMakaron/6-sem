@echo off
REM �� �������਩.
REM %�������� ��६�����% - ���饭�� � ��६�����.

REM ---------------------------------------------
REM �� ��६���� ����� � �㦭� ������.
REM ---------------------------------------------
REM project_name - ��� �ᯮ��塞��� 䠩��
REM tc_path - ��᮫��� ���� � ����� � Turbo C++ (ᬮ�� � .conf 䠩��)
REM source_files - ��室�� .c 䠩�� �१ �஡��
REM obj_files - 䠩�� ����� ��������� ��᫥ �������樨, � ���� ᮢ���� � .c 䠩����
REM ---------------------------------------------
set project_name=client
set tc_path=c:\TC
set source_files=%tc_path%\ipx.c %tc_path%\client.c
set obj_files=ipx_init.obj ipx.obj client.obj

REM ---------------------------------------------
REM �� ��६���� ������ �� �⮨�.
REM ---------------------------------------------
REM tc_bin_path - ���� � ����� bin, Turbo C++
REM tc_lib_path - ���� � ����� lib, Turbo C++
REM tc_inc_path - ���� � ����� include, Turbo C++
REM cxx_flags - 䫠�� �������樨 tcc
REM link_flags - 䫠�� �������� tlink
REM ---------------------------------------------
set tc_bin_path=%tc_path%\bin
set tc_lib_path=%tc_path%\lib
set tc_inc_path=%tc_path%\include
REM set cxx_flags=-I%tc_bin_path% -c -mt %source_files%
set cxx_flags=-I%tc_inc_path% -c -mt %source_files%
REM set link_flags=-L%tc_lib_path% %tc_lib_path%\c0t.obj %obj_files%,%project_name%.com,%project_name%.map,cs.lib 
set link_flags=-L%tc_lib_path% %tc_lib_path%\c0t.obj %obj_files%,%tc_path%\%project_name%.com,%tc_path%\%project_name%.map,cs.lib 

REM ---------------------------------------------
REM �����।�⢥��� �ਯ� ᡮન.
REM ---------------------------------------------

%tc_bin_path%\tcc %cxx_flags% 
%tc_bin_path%\tlink %link_flags% 

pause