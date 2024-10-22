#!/bin/bash

project_name="git utils"
git_exe=$(whereis -b git | awk '{print $2}')

handle_diff_line() {
    local line=$1
    local first_char=$(printf %.1s "$1")

    if [[ $first_char == [DR] ]]; then
        return 0
    fi

    local result=$(sed -n 's/^[ACDMTUXB][[:space:]]\{1,\}\([[:print:]]\{1,\}\.\(cpp\|h\)\)$/\1/p' <<< "${line}")
    if [[ -n $result ]]; then
        echo $result | xargs
    fi
}

if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <src-branch> <dst-branch>"
    exit 1
fi

if [[ -z "${git_exe}" ]]; then
    echo "[${project_name}] ${git_exe} isn't found" >&2
    exit 1
fi

# echo "git executor: ${git_exe}"
#${git_exe} fetch --depth=1 origin $1 &> /dev/null

command_result=$(${git_exe} diff --name-status --no-color --ignore-submodules=all $1..$2 2>&1)
return_code=$?

if [[ $return_code -ne 0 ]]; then
    echo "git command failed: ${command_result}" >&2
    exit 1
fi

readarray -t git_diff <<< "${command_result}"

if [[ -z "${git_diff}" ]]; then
    echo ""
    exit 0
fi

for line in "${git_diff[@]}"; do
    handle_diff_line "$line"
done
