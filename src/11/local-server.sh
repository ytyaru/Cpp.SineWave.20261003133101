#!/bin/bash

# スクリプトのあるディレクトリに移動
cd "$(dirname "$0")"

# ポート番号（デフォルト: 8080）
PORT=8080

echo "----------------------------------------"
echo "ローカルサーバーを起動します..."
echo "URL: http://localhost:$PORT/index.html"
echo "終了するには [Ctrl + C] を押してください。"
echo "----------------------------------------"

# Python 3 の簡易HTTPサーバーを使用
if command -v python3 &>/dev/null; then
    python3 -m http.server $PORT
elif command -v python &>/dev/null; then
    python -m http.server $PORT
else
    echo "[Error] python3 または python が見つかりません。" >&2
    exit 1
fi
