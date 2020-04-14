@echo off

set opts=-FC -GR- -EHa- -nologo -Zi
set code=%cd%
pushd ..\debug
cl %opts% %code%\windows -Fe..\release
popd
