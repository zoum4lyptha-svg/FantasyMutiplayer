@ECHO OFF
chcp 65001 >nul
TITLE UEProjectClearUtil
echo.
echo.
echo ================================================
echo   UE项目清理工具 - Grey项目
echo ================================================
echo.
echo 说明：清理编译缓存和临时文件
echo 保留：源代码、资源、配置文件、文档
echo.
echo 当前项目路径:%cd%
echo.
rem 先统计该路径下所有信息
dir /A/D /B
echo.
echo.
rem 虚幻项目编译文件
echo ------------------------------------------------------------------
echo 清理编译和缓存文件夹...
call :DeleteDirectory ".vs"
call :DeleteDirectory "Build"
call :DeleteDirectory "Binaries"
call :DeleteDirectory "DerivedDataCache"
call :DeleteDirectory "Intermediate"
call :DeleteDirectory "Saved"
call :DeleteDirectory "Package"
echo ------------------------------------------------------------------
echo 清理VS项目文件...
call :DeleteFile ".vsconfig"
call :DeleteFile "*.sln"
echo ------------------------------------------------------------------
echo 清理插件编译文件...
for /d %%i in (Plugins\*) do ( 
call :DeleteDirectory  %%i\Binaries
call :DeleteDirectory  %%i\Intermediate
)
echo ------------------------------------------------------------------
echo.
echo 以下内容已保留（不会被删除）：
echo   - Source/        (源代码)
echo   - Content/       (游戏资源)
echo   - Config/        (配置文件)
echo   - docs/          (技术文档)
echo   - README.md      (项目说明)
echo   - *.uproject     (项目文件)
echo   - .gitignore     (Git配置)
echo.
echo.

echo ------------------------------------------------------------------
echo 清理后项目结构：
echo.
echo 当前项目路径:%cd%
echo.
dir /A/D /B
echo.
echo.
echo ------------------------------------------------------------------
echo 清理完成！
echo.
echo 下一步：右键 Grey.uproject -^> Generate Visual Studio project files
echo.
echo.


pause

goto :eof

:DeleteFile
if exist %~1 (
echo DeleteFile %~1
del /s /q %~1
)
goto :eof

:DeleteDirectory
if exist %~1 (
echo DeleteDirectory %~1
rd /s /q %~1
)
goto :eof
