echo "Rebase code..."
git pull --rebase

echo "Build pm2_cli..."
bazel build  -c opt cpp/tool:pm2_cli
echo "Build fileutil..."
bazel build  -c opt cpp/tool:fileutil
echo "Install python wrapper..."
python setup.py install
