#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

function assert() {
    if [ $1 -ne 0 ]; then
        echo "assertion failed"
        exit;
    fi
}

function create() {
    local argc=$#

    if [ $argc -ge 2 ];then
        if [ "$1" = "path" ]; then
            readarray -d "/" -t arr <<< $2
            assert $?

            i=0
            p=""
            len=${#arr[@]}

            while [ $i -lt $len ]
            do
                p="$p/${arr[$i]}"
                i=$(expr $i + 1)
                if [ $i -ne $len ];
                then
                    mkdir ".$p"
                    assert $?
                fi
            done
            return
        elif [ "$1" = "array" ]; then
            i=2
            c=$(expr $
            a="($len:"

            while [ $i -lt $argc ]
            do
                val=$[$i]
                a="$a(${#val[@]}:$val)"
                i=$(expr $i + 1)
            done

            echo $a
            return
        fi

    fi

    assert 1
}

create array 1 2 3 4
