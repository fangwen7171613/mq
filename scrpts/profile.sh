#!/bin/sh

umask 022

# Customize the shell prompt to show the current directory followed by a '>'
export PS1="\w > "

# Set WORK_ROOT to default if not already set
export WORK_ROOT="${WORK_ROOT:-$HOME/work}"
export WORK_DATA_ROOT=$WORK_ROOT

# Function to update PATH if a given directory exists and is not already in PATH
add_to_path_if_not_exists() {
    local dir=$1
    if [ -d "$dir" ] && [[ ":$PATH:" != *":$dir:"* ]]; then
        export PATH="$dir:$PATH"
    fi
}

# Function to update LD_LIBRARY_PATH if a given directory exists and is not already in LD_LIBRARY_PATH
add_to_ld_library_path_if_not_exists() {
    local dir=$1
    if [ -d "$dir" ] && [[ ":$LD_LIBRARY_PATH:" != *":$dir:"* ]]; then
        export LD_LIBRARY_PATH="$dir${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
    fi
}

# Update LD_LIBRARY_PATH for WORK_ROOT/lib if it exists
add_to_ld_library_path_if_not_exists "$WORK_ROOT/lib"

# Update PATH for various directories
add_to_path_if_not_exists "$WORK_ROOT/bin"
add_to_path_if_not_exists "$HOME/tools/cmake/bin"
add_to_path_if_not_exists "$HOME/tools/gcc10/bin"
add_to_path_if_not_exists "$HOME/tools/valgrind/bin"

# Ensure the current directory './' is in PATH
add_to_path_if_not_exists "."

# Set JAVA_HOME
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-11.0.21.9-1.ky10.x86_64

export SSL_ROOT=$HOME/tools/openssl
add_to_ld_library_path_if_not_exists "$SSL_ROOT/lib64"
# Set alias for history command
alias h='history'
