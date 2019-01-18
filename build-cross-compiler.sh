#!/bin/bash

GCCVER=8.2.0
BINUTILSVER=2.31.1
GDBVER=8.2.1

function error() {
  echo -e "\033[1;31m$@\\033[0m";
}

function highlight() {
  echo -e "\033[1;32m$@\\033[0m";
}

function progress() {
  echo -n "$1... ";
  shift;
  TEMPFILE=`mktemp`;
  eval $@ >$TEMPFILE 2>&1;
  if [ $? -ne 0 ]; then
    echo -e "\033[1;5;31merror\033[0m";
    cat $TEMPFILE;
    rm $TEMPFILE;
    exit 1;
  fi
  rm $TEMPFILE;
  echo "done";
}

if [ $# -ne 2 ]; then
  echo "[i] Usage: $0 installation_prefix target";
  echo "[i] ie: $0 /home/lj/Apps/cross-compiler/i686-elf i686-elf";
  exit 1;
fi

PREFIX=$1;
TARGET=$2;

highlight "[i] Installation parameters:";
echo "[i] Installation prefix: ${PREFIX}";
echo "[i] Installation target: ${TARGET}";
echo;
highlight "[i] For the build to succeed you need to have the following packages installed:";
echo "[i] ]==> sudo apt-get install build-essential libgmp-dev libmpc-dev libmpfr-dev";
echo;

read -p  "[?] Do you want to continue? (y/n) " -n 1 -r;
echo;
echo;

if [ x"$REPLY" != xy ]; then
  echo "[i] Bye.";
  exit 0;
fi

TEMPDIR=`mktemp -d`;

(
  echo "[i] Working in ${TEMPDIR}";
  cd ${TEMPDIR};

  progress "[i] Donwloading binutils" wget http://ftp.gnu.org/gnu/binutils/binutils-${BINUTILSVER}.tar.bz2;
  progress "[i] Downloading gcc" wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-${GCCVER}/gcc-${GCCVER}.tar.xz;
  progress "[i] Downloading gdb" wget https://ftp.gnu.org/gnu/gdb/gdb-${GDBVER}.tar.xz;

  progress "[i] Unpacking binutils" tar jxf binutils-${BINUTILSVER}.tar.bz2;
  mkdir binutils-build;
  cd binutils-build;
  progress "[i] Configuring binutils" ../binutils-${BINUTILSVER}/configure --target=${TARGET} --prefix=${PREFIX} \
                                      --with-sysroot --disable-nls --disable-werror;
  progress "[i] Building binutils" make -j 4;
  progress "[i] Installing binutils" make install;

  cd ..;
  progress "[i] Unpacking gcc" tar xf gcc-${GCCVER}.tar.xz;
  mkdir gcc-build;
  cd gcc-build;
  export PATH=${PREFIX}/bin:$PATH;
  progress "[i] Configuring gcc" ../gcc-${GCCVER}/configure --target=${TARGET} --prefix=${PREFIX} \
                                 --disable-nls --enable-languages=c,c++ --without-headers;
  progress "[i] Building gcc" make -j4 all-gcc;
  progress "[i] Building libgcc" make -j4  all-target-libgcc;
  progress "[i] Installing gcc" make install-gcc;
  progress "[i] Installing libgcc" make install-target-libgcc;
  progress "[i] Installing unwind-pe.h" cp ../gcc-${GCCVER}/libgcc/unwind-pe.h ${PREFIX}/lib/gcc/${TARGET}/${GCCVER}/include

  cd ..;
  progress "[i] Unpacking gdb" tar xf gdb-${GDBVER}.tar.xz;
  mkdir gdb-build;
  cd gdb-build;
  progress "[i] Configuring gdb" ../gdb-${GDBVER}/configure --target=${TARGET} --prefix=${PREFIX}
  progress "[i] Building gdb" make -j 4;
  progress "[i] Installing gdb" make install;

  echo;
  highlight "[i] Installation successfull. Add the following to your shell profile:";
  echo "[i] export PATH=${PREFIX}/bin:\$PATH";
)
