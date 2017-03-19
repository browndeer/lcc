#!/bin/bash

# lcc (LOLCODE compiler front-end)
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
	echo "lcc version 0.7.0";
	echo "Copyright (C) 2017 Brown Deer Technology";
}

usage() {
	echo "Usage: lcc [options] file...";
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

keep() {
	if [ $keep -eq 1 ];
	then
		cp $1 __coprcc_$2
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

	defs=""
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

			-o) shift; outfile="$1";
			;;

			-c) no_link=1;
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

  			-D) shift; defs+=" -I $1";
		  	;;

  			-D*) defs+=" $1";
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
	debug DEFS $defs 
	debug
	debug INCS $incs 
	debug
	debug LIBS $libs 
	debug
	debug OPTS $opts 
	debug
	debug SRC $src
	debug ----------------------


	debug outfile= $outfile

	if [ $arch_e32 -eq 1 ]; then

		LC2C=lc2c
		CC=coprcc
		CCFLAGS="-fhost -w -std=gnu99 -D__E32__"
		DEFS=-DSHMEM_USE_CTIMER
		INCS="-I/usr/local/browndeer/include -I/usr/local/browndeer/coprthr2/include"
		LIBS="-L/usr/local/browndeer/coprthr2/lib -lcoprthr2_dev -lcoprthr_hostcall -lesyscall -le-lib -L../../src/ -lshmem"

	else

		LC2C=lc2c
		CC=gcc
		CCFLAGS=-w
		DEFS=
		INCS=-I/usr/local/browndeer/include
		LIBS=

	fi

	INCS+=$incs
	LIBS+=$libs

	OBJS=""

	nobjs=0

#	for f in $srcs
#	do
#
#		tmpcfile=`mktemp --tmpdir XXXXXX.c`
#		tmpobjfile=`mktemp --tmpdir XXXXXX.o`
#
#		objfile=""
#
#		case $f in
#
#			*.lol) 
#				chkerr $LC2C -o $tmpcfile $f
#				if [ $keep -eq 1 ];
#				then
#					cp $tmpcfile ${f/.lol/.c}
#				fi
#				chkerr $CC $CCFLAGS -c -o $tmpobjfile $tmpcfile $DEFS $INCS;
#				objfile=${tmpcfile/.c/.o}
#				if [ $keep -eq 1 ];
#				then
#					cp $tmpobjfile ${f/.lol/.o}
#				fi
#				;;
#
#		esac
#
#		if [ -e $tmpobjfile ];
#		then
#			nobjs+=1
#			if [ $no_link -eq 1 ];
#			then
#				debug cp -f $tmpobjfile $objfile
#				cp -f $tmpobjfile $objfile
#			else
#				OBJS+=" $tmpobjfile"
#			fi
#		fi
#	
#	done

	if [ "x$src" == "x" ]; then
		echo lcc: no input file
		exit -1
	fi

	tmpcfile=`mktemp --tmpdir XXXXXX.c`

	chkerr $LC2C -o $tmpcfile $src

	if [ $keep -eq 1 ];
	then
		cp $tmpcfile ${src/.lol/.c}
	fi

	chkerr $CC $CCFLAGS -o $outfile $tmpcfile $DEFS $INCS $LIBS;

}

main 
