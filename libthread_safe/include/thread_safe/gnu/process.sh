# usage SINGLE FILE
#    PATH_TO_PROCESS_SH="../process.sh"
#    $PATH_TO_PROCESS_sh ./bits/stl_construct.h

# usage DIRECTORY
#    PATH_TO_PROCESS_SH="$(readlink -e ../process.sh)"
#    find -type f -exec bash -c "$PATH_TO_PROCESS_SH {}" \;

rel() {
    # both $1 and $2 are absolute paths beginning with /
    # returns relative path to $2/$target from $1/$source
    source=$(realpath -m "$1");
    target=$(realpath -m "$2");

    common_part="$source" # for now
    result="" # for now

    while [[ "${target#"$common_part"}" == "${target}" ]]; do
        # no match, means that candidate common part is not correct
        # go up one level (reduce common part)
        common_part="$(dirname "$common_part")"
        # and record that we went back, with correct / handling
        if [[ -z $result ]]; then
            result=".."
        else
            result="../$result"
        fi
    done

    if [[ $common_part == "/" ]]; then
        # special case for root (no common path)
        result="$result/"
    fi

    # since we now have identified the common part,
    # compute the non-common part
    forward_part="${target#"$common_part"}"

    # and now stick all parts together
    if [[ -n "$result" ]] && [[ -n "$forward_part" ]]; then
        result="$result$forward_part"
    elif [[ -n "$forward_part" ]]; then
        # extra slash removal
        result="${forward_part:1}"
    fi

    echo "$result"
}

sedify() { sed s/\\//\\\\\\//g <<< "$1"; }

process() {
    src="$(readlink -e "$1")";

    grep -q -E "(.*)#(.*)include(.*)<(.*)>" "$src"
    if [[ "$?" == 0 ]]
      then
        rm -v "${src}.tmp"
        lineNumber=1
        lineTotal="$(wc -l < "$src")"

        # only process what we need to, "cat" the rest
        processing=1
        lastLine="$(grep -E "(.*)#(.*)include(.*)<(.*)>" -n < "$src" | tail -n 1 | grep -o -E "(.*)\:")"
        lastLine=${lastLine%?}

        while IFS= read -r line; do
          if [[ "$processing" == 1 ]]
            then
              grep -q -E ".*#.*include.*<.*>" <<< "$line"
              hasInclude="$?"
              if [[ "$hasInclude" == 0 ]]
                then
                  directory=$(dirname "$src")
                  include_directive="$line"
                  extracted="$(sed -r s/"(.*)#(.*)include(.*)<(.*)>(.*)"/"\4"/g <<< "$include_directive")"
                  relative_extracted="$(rel "$directory" "$extracted")"
                  replacement_include_directive="$(sed s/\<$(sedify "$extracted")\>/\"$(sedify "$relative_extracted")\"/ <<< "$include_directive")"
                  echo "$replacement_include_directive" >> "${src}.tmp"
                  if [[ "$lineNumber" == "$lastLine" ]]
                    then
                      processing=0
                      tail -n +"$((lastLine+1))" < "$src" >> "${src}.tmp"
                      echo "line processed: $1:$lineNumber/$lineTotal (last include location: $lastLine) (current line has include)"
                      break
                  fi
                  echo "line processed: $1:$lineNumber/$lineTotal (last include location: $lastLine) (current line has include)"
              else
                  echo "$line" >> "${src}.tmp"
              fi
          fi
          lineNumber="$((lineNumber+1))";
        done < "$src"
        mv -v "${src}.tmp" "$src"
      else
        echo "line processed: $1:$lineTotal/$lineTotal (no includes)"
    fi
}

process_v2() {

    # this version ASSUMES that head, and tail, supports the -n argument

    # in v2, we optimize things further by utilising head's -n and tail's -n for line replacement, eliminating the need to process line by line

    # obtain include lines with line numbers

    file="$(grep -E -n "(.*)#(.*)include(.*)<(.*)>" "$1")"
    if [[ $? == 0 ]]
        then
            readarray lines <<< "$file"
            lines=("${lines[@]%?}")

            # split lines array into an array of line numbers and directives
            directives=("${lines[@]#*:}")
            directive_lines=("${lines[@]%%:*}")

            # extract paths from directives
            extractedFirst=("${directives[@]%%<*}")
            extracted=("${directives[@]#*<}")
            extracted=("${extracted[@]%%>*}")
            extractedLast=("${directives[@]#*>}")

            # now that we have our directives, lines, and paths, we can start processing

            # set the directory
            directory=$(dirname "$(readlink -e "$1")")

            # build file in pieces

            echo processing file "$1"

            line=${directive_lines[0]}

            head -n $(($line-1)) "$1" > "${1}.tmp"
            for i in "${!lines[@]}"
                do
                    relative_extracted="$(rel "$directory" "${extracted[i]}")"
                    echo "${extractedFirst[i]}\"$relative_extracted\"${extractedLast[i]}" >> "${1}.tmp"
                    echo "processed $1:$line"
                    if [[ $(($i+1)) == ${#lines[@]} ]]
                        then
                            tail -n +$line "$1" | tail -n $(($line-"$(wc -l < "$1")")) >> "${1}.tmp"
                            break
                    fi

                    line=${directive_lines[i+1]}

                    head -n $(($line-1)) "$1" | tail -n $(($line-directive_lines[i]-1)) >> "${1}.tmp"
            done

            # move tmp file to real file
            mv -v "${1}.tmp" "$1"
            echo processed file "$1"
    fi
}

process_v2 $1