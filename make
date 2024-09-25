#!/bin/bash -i

CC="clang -w"
CFLAGS="-I./deps -I./src $(pkg-config --cflags freetype2)"
LDFLAGS="-lglfw -lGL -lGLEW -lm -lfreetype"
TARGET="grafenic"

need() {
    package="$1"
    if pacman -Q "$package" &> /dev/null; then
        return 0
    fi
    if command -v yay &> /dev/null; then
        echo "Installing $package using yay."
        yay --needed -S "$package"
        return $?
    fi
    if command -v pacman &> /dev/null; then
        echo "Installing $package using pacman."
        sudo pacman --needed -Sy "$package"
        return $?
    fi
    echo "Cannot install $package. Please install it manually using your distro's package manager."
    return 1
}

need clang
need fzf

clean() {
    echo -e "rm -rf ./$TARGET ./build/"
    rm -rf ./$TARGET ./build/
}

install() {
    sudo cp -r ./deps/* /usr/local/include/
    sudo mkdir -p /usr/local/include/grafenic
    sudo cp ./build/libgrafenic.so /usr/local/lib/
    sudo cp ./src/window.h /usr/local/include/grafenic/
    sudo ldconfig
}

uninstall() {
    sudo rm -f /usr/local/lib/libgrafenic.so
    sudo rm -rf /usr/local/include/grafenic
}

fzf-splitted () {
    if [[ -n "$TMUX" ]]
    then
        fzf --color=16 --reverse --ansi "$@"
    else
        fzf-tmux --color=16 -x --height ${FZF_TMUX_HEIGHT:-40%} --reverse --cycle --ansi "$@"
    fi
}

compile-library() {
    echo -e "$CC $CFLAGS -fPIC -c src/window.c -o ./build/window.o"
    $CC $CFLAGS -fPIC -c src/window.c -o ./build/window.o
    echo -e "$CC $CFLAGS -shared -o ./build/libgrafenic.so ./build/window.o $LDFLAGS"
    $CC $CFLAGS -shared -o ./build/libgrafenic.so ./build/window.o $LDFLAGS
}

build() {
    clean
    uninstall
    
    if [ -z "$1" ]; then
        SOURCES="./src/examples/$(ls ./src/examples | grep -v '^modules$' | sed 's/\.c$//' | fzf-splitted).c"
    else
        SOURCES="./src/examples/$1.c"
    fi

    mkdir -p ./build/

    compile-library
    
    if [ ! -f "/usr/local/lib/libgrafenic.so" ]; then
        install
    fi

    export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
    echo -e "$CC $CFLAGS $SOURCES -o $TARGET -lgrafenic $LDFLAGS"
    $CC $CFLAGS $SOURCES -o $TARGET -lgrafenic $LDFLAGS
}

run() {
    build $1
    ./"$TARGET"
}

debug() {
    CC="clang -w -g"
    build $1
    gdb -q $TARGET --eval-command=run
}

case $1 in
    run)
        run $2
        ;;
    debug)
        debug $2
        ;;
    clean)
        clean
        ;;
    install)
        install $2
        ;;
    uninstall)
        uninstall
        ;;
    *)
        echo "Usage: $0 {install|uninstall|run|debug|clean}"
        exit 1
        ;;
esac
