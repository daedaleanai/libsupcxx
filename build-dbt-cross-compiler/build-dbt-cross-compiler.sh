#!/bin/bash

GCCVER=11.1.0
BINUTILSVER=2.31.1
HOST=`uname -m`

function error() {
  echo -e "\033[1;31m$@\\033[0m";
}

function highlight() {
  echo -e "\033[1;32m$@\\033[0m";
}

function progress() {
  echo -n "$1... "
  shift
  TEMPFILE=`mktemp`
  eval $@ >$TEMPFILE 2>&1
  if [ $? -ne 0 ]; then
    echo -e "\033[1;5;31merror\033[0m"
    cat $TEMPFILE
    rm $TEMPFILE
    exit 1
  fi
  rm $TEMPFILE
  echo "done"
}

if [ $# -ne 1 ]; then
  echo "[i] Usage: $0 target"
  echo "[i] ie: $0 x86_64-elf"
  exit 1
fi

TARGET=$1;

SCRIPT_DIR=`dirname "$0"`
BUILD_GO=`readlink -f "${SCRIPT_DIR}/BUILD_go/${TARGET}/BUILD.go"`

if [ ! -f "${BUILD_GO}" ]; then
  error "[e] Could not find dbt build file ${BUILD_GO}. Is the target ${TARGET} supported?"
  exit 1
fi

OUT=`readlink -f "$TARGET-gcc-$GCCVER-binutils-$BINUTILSVER-$HOST.tar.gz"`

highlight "[i] Package parameters:"
echo "[i] Cross compiler target: ${TARGET}"
echo "[i] GCC version: ${GCCVER}"
echo "[i] Binutils version: ${BINUTILSVER}"
echo "[i] Compiling packages for: ${HOST}"
echo "[i] Output package: ${OUT}"
echo
echo

highlight "[i] For the build to succeed you need to have the following packages installed:"
echo "[i] ]==> sudo apt-get install build-essential libgmp-dev libmpc-dev libmpfr-dev"
echo

read -p  "[?] Do you want to continue? (y/n) " -n 1 -r
echo

if [ x"$REPLY" != xy ]; then
  echo "[i] Bye."
  exit 0
fi

TEMPDIR=`mktemp -d`
PREFIX="${TEMPDIR}/out"

(
  echo "[i] Working in ${TEMPDIR}"
  cd $TEMPDIR
  mkdir out
  echo

  highlight "[i] Downloading packages"
  progress "[i] Donwloading binutils" \
    wget http://ftp.gnu.org/gnu/binutils/binutils-${BINUTILSVER}.tar.bz2
  progress "[i] Downloading gcc" \
    wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-${GCCVER}/gcc-${GCCVER}.tar.xz
  highlight "[i] Downloads done"
  echo

  highlight "[i] Binutils"
  progress "[i] Unpacking binutils" tar jxf binutils-${BINUTILSVER}.tar.bz2
  mkdir binutils-build
  cd binutils-build
  progress "[i] Configuring binutils" ../binutils-${BINUTILSVER}/configure --target=${TARGET} --prefix=${PREFIX} \
                                      --with-sysroot --disable-nls --disable-werror
  progress "[i] Building binutils" make -j 4
  progress "[i] Adding binutils" make install
  export PATH=${PREFIX}/bin:$PATH
  cd ..
  highlight "[i] Binutils done"
  echo

  highlight "[i] GCC"
  progress "[i] Unpacking gcc" tar xf gcc-${GCCVER}.tar.xz;
  mkdir gcc-build
  cd gcc-build
  progress "[i] Configuring gcc" ../gcc-${GCCVER}/configure --target=${TARGET} --prefix=${PREFIX} \
                                 --disable-nls --enable-languages=c,c++ --without-headers
  progress "[i] Building gcc" make -j4 all-gcc
  progress "[i] Building libgcc" make -j4  all-target-libgcc
  progress "[i] Adding gcc" make install-gcc
  progress "[i] Adding libgcc" make install-target-libgcc
  progress "[i] Adding unwind-pe.h" cp ../gcc-${GCCVER}/libgcc/unwind-pe.h \
                                        ${PREFIX}/lib/gcc/${TARGET}/${GCCVER}/include
  cd ..
  highlight "[i] GCC done"
  echo
  

  cd out
  progress "[i] Adding BUILD.go" cp ${BUILD_GO} .
  progress "[i] Making package" tar -czvf ${OUT} *
  highlight "[i] Package ${OUT} created successfully"
)
