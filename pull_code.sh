echo "Rebase code..."
git pull --rebase

echo "Build pm2_cli..."
bazel build cpp/tool:pm2_cli
echo "Build fileutil..."
bazel build cpp/tool:fileutil
echo "Install python wrapper..."
python setup.py install
