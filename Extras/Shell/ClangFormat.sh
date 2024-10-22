#!/bin/bash

get_absolute_path() {
    local input_path="$1"
    local absolute_path

    # Check if the input path starts with "./" (relative path)
    if [[ "$input_path" == "./"* ]]; then
        absolute_path=$(pwd)"/${input_path#./}"
    else
        # Assume it's already an absolute path
        absolute_path="$input_path"
    fi

    echo "$absolute_path"
}

### args

clang_fmt_exe="$(whereis -b clang-format | awk '{print $2}')"
output_file="./out.diff"
file_list=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --output-file | -o)
            output_file=$(get_absolute_path "$2")
            if [[ -z "${output_file}" ]]; then
                echo "invalid output file name"
                exit 1
            fi
            shift 2 ;;
        --files)
            file_list=$(get_absolute_path "$2")
            shift 2 ;;
        *) echo "Script unknown option: '${1}'" >&2
           exit 1 ;;
    esac
done

### main

if [[ -z $file_list ]] || [[ ! -f $file_list ]]; then
    echo "Usage: $0 [OPTIONS] --files <file-list-filename>" >&2
    exit 1
fi

if [[ -z "$clang_fmt_exe" ]]; then
    echo "fatal: clang-format isn't found" >&2
    exit 1
fi

echo "clang-format: ${clang_fmt_exe}"
echo "file list: '${file_list}'"

readarray -t arr_files < ${file_list}

tmp_dir="/tmp/sorex/extras/"
tmp_file="${tmp_dir}/sh-clang-format.txt"

if [ ! -d "${tmp_dir}" ]; then
    mkdir -p "${tmp_dir}"
fi

rm -f $output_file
touch $output_file

exit_code=0
for filename in "${arr_files[@]}"; do
    if [[ -z $filename ]]; then
        continue
    fi

    echo -n "formatting: '${filename}' .. "
    ${clang_fmt_exe} --style=file ${filename} > ${tmp_file}
    return_code=$?

    if [[ $return_code -ne 0 ]]; then
        echo "${clang_fmt_exe} command failed: ${filename}" >&2
        exit 1
    fi

    command_result=$(diff -u $filename $tmp_file)
    if [[ -n $command_result ]]; then
        echo "diff"
        echo "$command_result" >> $output_file
        exit_code=1
    else
        echo "ok"
    fi
done

if [[ $exit_code -ne 0 ]]; then
    echo "diff output: ${output_file}"
fi

exit $exit_code

