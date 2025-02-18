#!/bin/bash

echo "🔧 Iniciando manutenção do projeto vitor_game..."

# Caminho da pasta
PASTA=$(dirname "$0")

# Evitar tentar modificar arquivos de compilação
find "$PASTA" -path "$PASTA/raylib/build*" -prune -o -type f \( -name "*.log" -o -name "*.tmp" -o -name "*.cache" \) -delete

# Remover pastas temporárias __pycache__
find "$PASTA" -path "$PASTA/raylib/build*" -prune -o -type d -name "__pycache__" -exec rm -rf {} +

# Otimizar permissões (opcional)
chmod -R 755 "$PASTA"

echo "✅ Manutenção concluída!"
