#!/bin/bash

# lol (LOLCODE program execution front-end)
#
# Copyright (c) 2017 Brown Deer Technology, LLC.  All Rights Reserved.
#
# This software was developed by Brown Deer Technology, LLC.
# For more information contact info@browndeertechnology.com
#
# /* DAR */

verbose=0
keep=0
arch_e32=0

version() {
	echo "lol version 0.7.0";
	echo "Copyright (C) 2017 Brown Deer Technology";
}

usage() {
	echo "Usage: lol [options] file...";
	echo "Options:";
	echo "  -march=e32            Target Epiphany 32-bit architecture.";
	echo "  -k, --keep            Keep intermediate files.";
	echo "  -v                    Show details of compilation.";
}

debug() {
	if [ $verbose -eq 1 ];
	then
		echo
		echo $*
	fi
}

chkerr() {
	debug $*
	$*
	if [ $? -ne 0 ];
	then
		echo lcc: error
		exit -1
	fi
}

main() {

	no_link=0

	outfile="a.out"

	incs=""
	libs=""
	opts=""
	src=""

	while [[ $# > 0 ]]
	do

		arg="$1"

		case $arg in

			--) shift; break
  			;;

			--version) version; exit 0;
			;;

			-h|--help) usage; exit 0;
			;;

			-v) verbose=1;
			;;

			-q) verbose=0;
			;;

			-k|--keep) keep=1;
			;;

         -I|--include) shift; incs+=" -I $1";
         ;;

         -I*|--include=*) incs+=" $1";
         ;;

         -L) shift; libs+=" -L $1";
         ;;

         -L*) libs+=" $1";
         ;;

         -l*) libs+=" $1";
         ;;

			*.lol) src+=" $1";
			;;

			-march=e32) arch_e32=1;
			;;

		 	*) opts+=" $1";

		esac

	shift

	done

	debug ----------------------
   debug INCS $incs
   debug
   debug LIBS $libs
   debug
	debug OPTS $opts 
	debug
	debug SRC $src
	debug ----------------------


	if [ $arch_e32 -eq 1 ]; then

		LCC=lcc
		LCCFLAGS="-march=e32 -w"

	else

		LCC=lcc
		LCCFLAGS=-w

	fi

	INCS+=$incs
	LIBS+=$libs

	if [ "x$src" == "x" ]; then
		echo lol: no input file
		exit -1
	fi

	tmpxfile=`mktemp --tmpdir XXXXXX.x`

	debug $LCC $LCCFLAGS -o $tmpxfile $src $INCS $LIBS
	$LCC $LCCFLAGS -o $tmpxfile $src $INCS $LIBS
	if [ $? -ne 0 ];
	then
		echo lcc: error
		exit -1
	fi

	if [ -x $tmpxfile ]; then

		if [ $keep -eq 1 ];
		then
			cp $tmpxfile $oufile
		fi

		debug $tmpxfile
		$tmpxfile
		if [ $? -ne 0 ];
		then
			echo program: error
			exit -1
		fi

	fi

}

main 
