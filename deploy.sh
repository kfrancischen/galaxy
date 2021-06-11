# check whether to install bazel 3.7.2
if [$# -eq 1] && [$1 -eq "init_install"]
then
    echo "Installing bazel 3.7.2 and pm2...."
    sudo apt install npm
    sudo npm install pm2@latest -g
    pm2 install pm2-logrotate
    sudo npm install -g @bazel/bazelisk
fi

# bazel build
echo "Starting bazel build process..."
bazel build //cpp:galaxy_server

# generate pm2 instance
echo "Starting pm2 instance..."
pm2 start bazel-bin/cpp/galaxy_server --name galaxy_server --watch


# bazel build for ttl cleaner
echo "Starting bazel build process for ttl cleaner..."
bazel build //ext/ttl_cleaner:galaxy_ttl_cleaner

# generate pm2 instance
echo "Starting pm2 instance..."
pm2 start bazel-bin/ext/ttl_cleaner/galaxy_ttl_cleaner --name galaxy_ttl_cleaner --watch
