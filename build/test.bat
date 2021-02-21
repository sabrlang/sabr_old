@echo off
cd C:\Project\csabr\build
echo ----Compile----
sabrc test.sabr test.sabre
if %errorlevel% NEQ 0 goto failure
echo ----Interprete----
sabre test.sabre
:failure
echo.
pause