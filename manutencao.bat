@echo off
echo 🔧 Iniciando manutenção do projeto vitor_game...

:: Caminho da pasta
setlocal
set "PASTA=%~dp0"

:: Evitar tentar modificar arquivos de compilação
:: (não temos comando find no Windows, então iremos ignorar pastas específicas)
for /d /r "%PASTA%" %%d in (raylib\build*) do (
    echo Ignorando pasta de compilação: %%d
)

:: Remover arquivos temporários e cache
del /s /q "%PASTA%\*.log" "%PASTA%\*.tmp" "%PASTA%\*.cache"

:: Remover pastas __pycache__
for /d /r "%PASTA%" %%d in (__pycache__) do rd /s /q "%%d"

echo ✅ Manutenção concluída!
pause
