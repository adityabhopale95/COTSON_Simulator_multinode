#!/bin/bash
#
# AUTHORS:      : SV (viola@dii.unisi.it), RG (giorgi@dii.unisi.it)
# TITLE         : bootstrap.sh
# LICENSE       : MIT
# VERSION       : 0.0.2
# DATE          : 20160509
# NOTES         : NULL
#
#===============================================================================
#
# PURPOSE: Prepare the system for the COTSon installation
#


### NOTE ###
# echo "${BASH_LINENO[$FUNCNAME]}"
# echo "$BASH_SOURCE"
# echo "$LINENO"
### #### ###

### GLOBAL VARIABLES DEFINITIONS ###
SIMNOW_SUPPORTED_VER="4.6.2pub"
USERNAME=$(logname)
DIST_IMAGE="karmic64"
DIST=""
DIST1=""
VER=""
VERNUM=""
DEBUG=0
FORCE=0
SIMNOW_DIR="" # ENV VARIABLE NEED elimina
g_simnow_dir=""
root=$(pwd)
VERBOSE=0
ALL_MODE=0


BASEPATH_STATUS_FILE="/var/lib"
DIR_NAME_STATUS_FILE="cotson"
STATUS_FILE_PATH="$BASEPATH_STATUS_FILE/$DIR_NAME_STATUS_FILE"

###
trap "exit 1" TERM
export TOP_PID=$$
###

DEBIAN_BASED_PKGS="g++ g++-multilib subversion genisoimage bison flex vnc4server  \
rxvt xfwm4 xfonts-100dpi xfonts-75dpi zsh sharutils build-essential xvnc4viewer screen \
liblua5.1-0 liblua5.1-0-dev zlib1g-dev indent xutils-dev libsqlite3-dev \
sqlite3 libdbd-sqlite3-perl libdbd-pg-perl gnuplot libboost-dev libboost-thread-dev lzma \
libxcursor1 libxrender1 libsm6 libxi6 libfontconfig1 gnuplot-x11 subversion procmail rpcbind genisoimage gawk \
gedit mc quota"

FEDORA_BASED_PKGS="gcc gcc-c++ make subversion genisoimage bison flex ruby rubygems rxvt zsh sharutils screen \
gnuplot indent zlib-devel imake xorg-x11-utils ruby-libs openssl \
rxvt flex lua* lua-devel libpqxx* boost-devel boost-thread sqlite-devel sqlite lzma \
perl-Class-DBI-SQLite xorg-x11-fonts-100dpi.noarch xorg-x11-fonts-75dpi \
tigervnc tigervnc-server tigervnc-server-module xorg-x11-twm glibc-devel.i686 libstdc++.i686 \
glibc-static glibc-devel libstdc++ libtool subversion flex procmail gawk rpcbind genisoimage \
execstack qpdfview gedit mc quota"

DEBIAN_SUPPORTED_VERS="lenny | squeeze | wheezy | jessie"
FEDORA_SUPPORTED_VERS="Werewolf | Leonidas | Goddard | Laughlin | Lovelock | Verne | BeefyMiracle | SphericalCow | Schrödinger’sCat | Heisenbug | TwentyOne | TwentyTwo | TwentyThree | TwentyFour | TwentyFive"
UBUNTU_SUPPORTED_VERS="intrepid | jaunty | karmic | lucid | maverick | natty | oneiric | precise | quantal | raring | trusty | utopic | wily | xenial | yakkety"

COTSON_IMAGES_BASE_PATH="/opt"
COTSON_IMAGES_PATH="${COTSON_IMAGES_BASE_PATH}/cotson"
COTSON_DIST_DIR_NAME="dist"
COTSON_IMAGES_DIR_NAME="images"
COTSON_BSDS_DIR_NAME="bsds"
# OLD
#COTSON_STATUS_PATH="log"	#put inside the bootstrap.status file
COTSON_ROM_DIR_NAME="roms"

FEDORA_INSTALLER="yum"
FEDORA_INSTALLER_CONF="yum.conf"

### ############################ ###
function echo_d()
{
        if [ $VERBOSE = 1 ]; then
                verbose_str="[`date +"%T"` - ${FUNCNAME[1]}:${BASH_LINENO[$FUNCNAME[1]]} - D ]: "
        else
                verbose_str="* "
        fi
        if [ $DEBUG = 1 ]; then
                echo -e "\e[33m$verbose_str$1\e[39m"
        else
                return
        fi
}

function echo_w()
{
        if [ $VERBOSE = 1 ]; then
                verbose_str="[`date +"%T"` - ${FUNCNAME[1]}:${BASH_LINENO[$FUNCNAME[1]]} - W ]: "
        else
                verbose_str="* WARNING: "
        fi
        echo -e "\e[33m$verbose_str$1\e[39m"
}

function echo_e()
{
        if [ $VERBOSE = 1 ]; then
                verbose_str="[`date +"%T"` - ${FUNCNAME[1]}:${BASH_LINENO[$FUNCNAME[1]]} - E ]: "
        else
                verbose_str="* "
        fi
        echo -e "\e[31m$verbose_str$1\e[39m"
}

function echo_ok()
{
        if [ $VERBOSE = 1 ]; then
                verbose_str="[`date +"%T"` - OK]: "
        else
                verbose_str="* "
        fi
        echo -e "\e[32m$verbose_str$1\e[39m"
}

function echo_i()
{
        local verbose_str=""
        if [ $VERBOSE = 1 ]; then
                verbose_str="[`date +"%T"` - I ]: "
        else
                verbose_str="* "
        fi
        echo -e "$verbose_str$1"
}



### include add-image.sh lib ###
if [ "$ALL_MODE" = 1 ]; then
	if [ -s add-image.sh ]; then
		source add-image.sh --lib

		# Choose the default image to be used
		index=0
		for elem in "${!A_IMAGES[@]}";
		do
			name=$(_get_name ${A_IMAGES[$elem]})
			if [ "$DIST_IMAGE" = "$name" ]; then
				break
			fi
			index=$(( $index + 1 ))
		done
		G_MENU_CHOISE=${A_IMAGES[$index]}
		COTSON_IMAGES_PATH=$(_get_lpath "$G_MENU_CHOISE")
	else
		echo_w "add-image.sh not found (maybe it is not necessary)"
	fi
fi

#
OUT_FILE="${STATUS_FILE_PATH}/bootstrap.status"

function checkconnection
{
	local myservice="$1"
	local myhost="$2"
	local rc="1"
	local d
	local c
	local r
	local rpm
	if [ "$myhost" != "" -a "$myservice" != "" ]; then
		echo_d "Checking '$myservice' service..."
		case $myservice in 
			web)
				wget --spider --timeout=5 --tries=3 ${myhost}/ >/dev/null 2>/dev/null
				rc=$?
				if [ "$rc" = "0" ]; then
					echo_d "$myhost has web service active."
					return 0
				else
					echo_e "$myhost has web service not active."
					return 1
				fi
			;;
			nfs)
				rpcinfo=`which rpcinfo 2>/dev/null`
				if [ "$rpcinfo" = "" ]; then
					if [ ! -x /usr/sbin/rpcinfo ]; then
						echo_w "Cannot find 'rpcinfo'" 1>&2
					else
						rpcinfo="/usr/sbin/rpcinfo"
					fi
				fi
				d=`echo "send escape"|nc -vw 5 $myhost $nfsport 2>&1|$grep succeeded |$awk '{print $7}'`
				c=`echo "send escape"|nc -vw 5 $myhost $portmapperport 2>&1|$grep succeeded |$awk '{print $7}'`
				if [ "$verbose" != "0" ]; then
					if [ "$c" != "" ]; then  
						echo "       Machine $myhost has PORTMAPPER port $portmapperport open.";
					else 
						echo "Machine $myhost has PORTMAPPER port ($portmapperport) CLOSED."; 
					fi
					if [ "$d" != "" ]; then
						echo "       Machine $myhost has NFS port $nfsport open.";
					else echo "Machine $myhost has NFS port $nfsport CLOSED."; 
					fi
				fi
				rpm=""
				if [ "$d" = "succeeded!" -a "$c" = "succeeded!" ]; then
					if [ "$rpcinfo" != "" ]; then
						r=`${rpcinfo} -p $myhost 2>/dev/null`
						rpm=`echo "$r"|$grep $nfsport|$awk '{print $4}'|$tail -1`
						if [ "$verbose" != "0" ]; then
							if [ "$rpm" != "" ]; then
								echo "       Machine $myhost has NFS server OK.";
							else
								echo "Machine $myhost has no NFS service running."; 
							fi
						fi
					else # rpcinfo not accessible: assume that NFS is not available
						echo_w "assuming NFS is not avialable" 1>&2
					fi
				fi
				if [ "$d" = "succeeded!" -a "$c" = "succeeded!" -a "$rpm" != "" ]; then # portcheck
					rc="0"
				fi
			;;
			*)
				### sometime ping is filtered out and doesn't work ('web' check may be better)
				ping -c 2 -W 3 $myhost >/dev/null 2>/dev/null
				rc=$?
				if [ "$verbose" != "0" ]; then
					if [ "$rc" = "0" ]; then
						echo "       Machine $myhost responded to ping."; fi
					fi
			;;
		esac
	fi
	if [ "$rc" = "0" ]; then noconnection="0"; else noconnection="1"; fi
}

function sv_exit()
{
	kill -s TERM $TOP_PID
}


# FUNCTION: sysctl_tuning()
# Configure the system parameters with the correct values for running COTSon
# ROOT required!
# Tune /etc/sysctl.conf values
#
function sysctl_tuning()
{
	#
	# sysctl -w vm.max_map_count=65535
	# REFERENCE: https://www.kernel.org/doc/Documentation/sysctl/vm.txt
	# max_map_count:
	# This file contains the maximum number of memory map areas a process may 
	# have. Memory map areas are used as a side-effect of calling malloc, 
	# directly by mmap and mprotect, and also when loading shared libraries.
	# While most applications need less than a thousand maps, certain programs, 
	# particularly malloc debuggers, may consume lots of them, e.g., up to one or 
	# two maps per allocation.
	# The default value is 65536.
	#
	echo_i "Tuning /etc/sysctl.conf..."
	mapcurrent=`/sbin/sysctl -n vm.max_map_count`
	# calculate an optimal value as totmem/4 (min 4M)
	total_ram_mem=`free |awk '/Mem/{printf("%d", ($2+1048575)/1048576)}'` #round to GB
	echo_i "  Total RAM Memory system     = about $total_ram_mem GB"
	mapoptim=`expr $total_ram_mem \/ 4`
	mapmin=4194304
	echo_i "  mapoptim   = $mapoptim"
	echo_i "  mapmin     = $mapmin"
	
	if [ $mapoptim -lt 4 ]; then 
		mapok=$mapmin; 
	else 
		mapok=`expr $mapoptim \* 1048576`; 
	fi
	echo_i "  mapok      = $mapok"
	
	mapconfig=`egrep "[ \t]*[^#]*[ \t]*vm.max_map_count" /etc/sysctl.conf| tail -1 |awk -F= '{print $2}'`
	if [ "$mapconfig" = "" ]; then mapconfig="0"; fi
	echo_i "  mapconfig  = $mapconfig"
	echo_i "  mapcurrent = $mapcurrent"
	#Tune vm.max_map_count
	if [ $mapcurrent -lt $mapok ]; then
		echo_i "Tuning vm.max_map_count : $mapcurrent --> $mapok"
		#case 1: the sysctl.conf contains a vm.max_map_count line
		echo_w "New implementation with sudoers command enabled"
		local tmp_file_name
		tmp_file_name=`mktemp tmp.XXXXXXXXXX`
		cp /etc/sysctl.conf $tmp_file_name
		sudo cp $tmp_file_name /etc/sysctl.conf
		echo_d "Removing the temporary file: $tmp_file_name"
		rm $tmp_file_name
		#case 2: the sysctl.conf DOES NOT contaiin a vm.max_map_count
		if [ "$mapconfig" = 0 ]; then 
			local tmp_file_name
			tmp_file_name=`mktemp tmp.XXXXXXXXXX`
			cp /etc/sysctl.conf $tmp_file_name
			sed -i "\$avm.max_map_count=$mapok" $tmp_file_name
			sudo cp $tmp_file_name /etc/sysctl.conf
			echo_d "Removing here the temporary file: $tmp_file_name"
			rm $tmp_file_name
		fi
		sudo /sbin/sysctl -p 2>&1 >/dev/null # make it permanent
		# verify settings
		echo_i "Verifying settings"
		mapconfig2=`egrep "[ \t]*[^#]*[ \t]*vm.max_map_count" /etc/sysctl.conf| tail -1 |awk -F= '{print $2}'`
		mapcurrent2=`/sbin/sysctl -n vm.max_map_count` # this may not require sudo access
		echo_i "  mapconfig2 = $mapconfig2"
		echo_i "  mapcurrent2= $mapcurrent2"
		echo_i "* New VM configuration is: vm.max_map_count = $mapcurrent2"
	else
			echo_i "Detected that VM configuration is ok: vm.max_map_count = $mapcurrent"
	fi
	#echo "sysctl_tuning=\"OK\"" >> $OUT_FILE
	write_status_file "sysctl_tuning=\"OK\""
}

function _detect_os()
{
	echo_d "CALL: $FUNCNAME"
	ret=`which lsb_release 2>&1`
	check_status1=`echo "$ret"|grep "not found"`
	check_status2=`echo "$ret"|grep "no lsb_release"`
	ec1=0
	ec2=0
	if [[ "$check_status1" != "" || "$check_status2" != "" ]]; then
		#don't know yet which distro
		#try
		echo_w "lsb_release not found... try to install"
		sudo yum -y install redhat-lsb 2>/dev/null
		ec1=$?
		if [ $ec1 != 0 ]; then
			sudo dnf -y install redhat-lsb 2>/dev/null
			ec1=$?
		fi
		#try
		sudo apt-get -q -q install lsb 2>/dev/null
		ec2=$?
	fi

	if [[ $ec1 != 0 && $ec2 != 0 ]]; then
		echo_e "Couldn't find 'lsb_release'command"
		echo_e "Plese check your environment"
		echo "BOOTSTRAP=\"KO\"" > $OUT_FILE
		exit 1
	fi

	local dist
	dist=`lsb_release -i -s`
	DIST=$dist
	dist1="$dist"
	DIST1=$dist1
	ver=`lsb_release -c -s`
	VER=$ver
	vernum=`lsb_release -r -s`
	VERNUM=$vernum

	#compatibility patch for CentOS
	if [ "$DIST" = "CentOS" ]; then
		dist="Fedora"
		dvn1=${vernum%.*}
		if [ "$dvn1" = "6" ]; then vernum="14"; ver="Laughlin"; fi
	fi
	# Keep attention, if you have a CentOS system, will save in status file "Fedora"
	write_status_file "DIST=$DIST"
	write_status_file "DIST1=$DIST1"
	write_status_file "VER=$VER"
	write_status_file "VERNUM=$VERNUM"

	echo_i "Detected Distribution '$dist' - Version '$ver'"
	ok=0
	shopt -s extglob
	local ubuntu_check='@('${UBUNTU_SUPPORTED_VERS// /}')'
	local debian_check='@('${DEBIAN_SUPPORTED_VERS// /}')'
	local fedora_check='@('${FEDORA_SUPPORTED_VERS// /}')'
	case $dist in
		Ubuntu)
			case "$ver" in
				$ubuntu_check) ok=1 ;;
			esac
		;;
		Debian)
			case "$ver" in
				$debian_check) ok=1 ;;
			esac
		;;
		Fedora)
			case "$ver" in
				$fedora_check) ok=1 ;;
			esac
		;;
	esac
	if [[ $ok -eq 0 ]]; then
		echo ""
		echo "Sorry: $dist $ver not supported"
		echo "Supported versions are:"
		echo "   UBUNTU: $UBUNTU_SUPPORTED_VERS"
		echo "   DEBIAN: $DEBIAN_SUPPORTED_VERS"
		echo "   FEDORA: $FEDORA_SUPPORTED_VERS"
		echo ""
		write_status_file "BOOTSTRAP=\"KO\""
		exit 1;
	fi

	echo_i "$dist $ver supported"
	write_status_file "_detect_os=\"OK\""
}

function make_dependencies()
{
	_detect_os
	if [[ $DIST == "Debian" ||  $DIST == "Ubuntu" ]]; then
		pqxx=libpqxx-dev 
		ruby="ruby ruby1.8 rubygems libopenssl-ruby libsqlite3-ruby "

		if [[ $VER == "trusty" || $VER == "utopic" || $VER == "jessie" || $VER == "wheezy" || "$VER" == "wily" || "$VER" == "xenial" || "$VER" == "yakkety" ]]; then
			pkgs+=" qpdfview exfat-fuse"
		fi
		if [[ "$VER" == "wily" || "$VER" == "xenial" || "$VER" == "yakkety" ]]; then
			check_repo=`sudo apt-add-repository universe`
			ret=`echo "$check_repo"|grep "distribution component is already enabled"`
			echo_d "UNIVERSE: $ret"
			ruby="ruby"
		fi

		# Let's make sure that we refresh the package db
		sudo apt-get -q -q -y update

		#
		if [[ $VER == "precise" || $VER == "quantal" || $VER == "raring" ]]; then 
			pqxx=libpqxx3-dev
		fi
		if [[ $VER == "trusty" || $VER == "utopic" || $VER == "jessie" || $VER == "wheezy" ]]; then
			pqxx=libpqxx3-dev
			if [ "$VER" = "utopic" ]; then ruby=""; else ruby="ruby1.8"; fi
			if [ "$VER" = "trusty" ]; then ruby="ruby"; fi
		fi
		pkgs="$DEBIAN_BASED_PKGS"
		pkgs+=" $pqxx"
		pkgs+=" $ruby"
		echo_d "$pkgs"

		cmdi="apt-get -q -q --fix-missing"
		cmdt1=""
		cmdt2="dpkg -s"
		cmdcheckstr="^Status.*installed$"
		xvnc="Xvnc4"
		xvncservctrl="vnc4server"
		vncargs1="-ac -SecurityTypes None -br -pn -httpd /usr/share/"
		vncargs2="-desktop="
		llua="lua5.1"

		if [ "$VER" != "utopic" -a "$VER" != "wily" -a "$VER" != "trusty" -a "$VER" != "xenial" -a "$VER" != "yakkety" ]; then
			ruby --version | grep 1.8
			if [[ $? -ne 0 ]]; then
				echo "### Installing ruby1.8"
				sudo apt-get -q -q install ruby1.8 -y
				ruby18=`update-alternatives --list ruby | grep 1.8`
				sudo update-alternatives --set ruby $ruby18
			fi
		fi
	elif [[ $DIST == "Fedora" ]]; then
		# Add here the check of yum or dnf
		ret_command=`which dnf 2>&1`
		echo_d "RET of which: $?"
		if [ $? = 0 ]; then
			echo_i "dnf found... Configuring dnf"
			FEDORA_INSTALLER="dnf"
			FEDORA_INSTALLER_CONF="dnf/dnf.conf"
		fi
		if [[ $VERNUM -lt 17 ]]; then
			rubystuff="ruby-sqlite3 rubygem-test-unit"
		else
			rubystuff="rubygem-sqlite3 rubygem-bigdecimal rubygem-test-unit"
		fi
		if [ "$DIST1" = "Fedora" ]; then # avoid the following pkgs in CentOs
			extrapkgs="dpkg xfwm4 perl-pgsql_perl5 libstdc++-static"
		fi
		if [[ $VERNUM -ge 20 ]]; then
			extrapkgs="$extrapkgs compat-lua* mariadb"
		fi
		if [[ $VERNUM -ge 24 ]]; then
			pkgs+=" fuse-exfat"
		fi
		pkgs="$FEDORA_BASED_PKGS"
		pkgs+=" ${rubystuff}"
		pkgs+=" $extrapkgs"

		if [[ `uname -m` == "x86_64" ]]; then 
			screen_downgrade="ftp://ftp.pbone.net/mirror/archive.fedoraproject.org/fedora/linux/releases/13/Fedora/x86_64/os/Packages/screen-4.0.3-15.fc12.x86_64.rpm"
		else
			screen_downgrade="ftp://ftp.pbone.net/mirror/archive.fedoraproject.org/fedora/linux/releases/13/Fedora/i386/os/Packages/screen-4.0.3-15.fc12.i686.rpm"
		fi
		cmdi="$FEDORA_INSTALLER -y"
		#cmdt1="yum list install"
		cmdt1="$FEDORA_INSTALLER list installed"
		cmdt2=""
		cmdcheckstr=""
		xvnc="Xvnc"
		xvncservctrl="vncserver"
		vncargs1="-ac -SecurityTypes None -br -pn -httpd /usr/share/"
		vncargs2="-desktop "
		if [[ $VERNUM -ge 20 ]]; then
			llua="lua-5.1"
		else
			llua="lua"
		fi

		#SELinux complains if the SimNow LinuxLibs execute code in the stack
		#The following command clears the stack-execute bit in the SimNow LinuxLibs
		#(alternatively, SELinux can be disabled)
		execstack -c $g_simnow_dir/linuxlibs/lib*.so.*
	fi
	########################################################################

	echo_i "Checking if extra packages should be installed ..."
	ret="0"
	echo -n "  "
	[ -z "$cmdt1" ] || pkginst=`$cmdt1`
	for p in $pkgs; do
		#echo -n "$p "
		echo -n ". "
		[ -z "$cmdt2" ] && cmdcheckstr="$p$cmdcheckstr" || pkginst=`$cmdt2 $p 2>/dev/null`
		(echo "$pkginst" |grep -q "$cmdcheckstr") >/dev/null 2>/dev/null
		ec=$?
		if [ "$ec" != "0" ]; then
			ret=`expr $ret + 1`;
			if [ "$ppkgs" = "" ]; then
				ppkgs="$p"; 
			else
				ppkgs="$ppkgs $p";
			fi
		fi
	done
	echo ""
	ret1="0"
	if [ "$ret" != "0" ]; then
		echo ""
		echo "*** NOTE: for this step (installation of missing packages/dependencies: $ppkgs ), either ask your admin or be sure you have sudo access"
		echo "* Installing missing packages/dependencies..."
		echo_d "cmdi= $cmdi - ppkgs= $ppkgs"
		# If there is an error: try to install a second time (sometimes solves problems)
		ec="1"; instcount="0"; maxinsttry="2"
		while [ "$ec" != "0" -a "$instcount" -lt "$maxinsttry" ]; do
			sudo DEBIAN_FRONTEND=noninteractive $cmdi install $ppkgs
			ec="$?"
			instcount=`expr $instcount + 1`
		done
		echo_d "ec=$ec"
		if [ "$ec" != "0" ]; then
			ret1=`expr $ret1 + 1`;
		fi
		echo_i "Packages installation FINISHED"
	else
		echo_i "ALL needed packages are present"
	fi
	
	#@@@@@@@@@@@@@@@@@@@@
	echo_i "Checking ruby gems"
	if [[ `sudo gem list | grep test-unit` ]]; then 
		echo_i "Gem \"test-unit\" already installed"
	else
		echo_i "Installing missing gems"
		echo_i "Checking connection befor to install ruby test-unit"
		checkconnection "web" "google.com"
		if [[ $? -ne 0 ]]; then
			echo_e "Internet connection not available"
			exit 1
		else
			echo_ok "Connection OK"
		fi
		echo_i "Installing rubygems test-unit"
		sudo gem install test-unit
		echo_i "Gems installation FINISHED"
	fi
	#@@@@@@@@@@@@@@@@@@@@

	if [[ "$ret1" -ne 0 ]]; then
		echo ""
		echo "ERROR: Could not install one or more of $pkgs"
		echo "- Please check your distribution and edit 'configure' to reflect it"
		write_status_file "BOOTSTRAP=\"KO\""
		exit 1
	fi

	# Workaround for screen 4.1 bug
	if [[ $dist == "Fedora" && `screen -v | awk  '$3 != "4.00.03"'` ]]; then
		echo ""
		echo "Downgrade of package ($screen_downgrade)"
		echo "*** NOTE: sudo access required ***"
		echo ""
		sudo $cmdi downgrade $screen_downgrade
		grep screen /etc/$FEDORA_INSTALLER_CONF
		#grep screen /etc/dnf/dnf.conf
		if [[ $? -ne 0 ]]; then
			echo "Add screen to exclude entry in $FEDORA_INSTALLER_CONF"
			sudo sh -c "echo 'exclude=screen' >> /etc/$FEDORA_INSTALLER_CONF" 
		fi
	fi
	# end workaround 
	# Workaround for rubygems-psych bug
	if [[ $dist == "Fedora" && $vernum == 19 ]]; then
		sudo $FEDORA_INSTALLER update --enablerepo=updates-testing ruby-2.0.0.247-13.fc19
	fi
	# end workaround 
	write_status_file "make_dependencies=\"OK\""
}

function cotson_images_dirs()
{
	echo_i "Checking the cotson directory ..."
	if [ ! -e $COTSON_IMAGES_PATH ]; then
		echo_i "Cotson directory structure not exist"
		echo_i "Try to create them..."
		echo_d "mkdir $COTSON_IMAGES_PATH"
		sudo mkdir -p $COTSON_IMAGES_PATH
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_DIST_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_DIST_DIR_NAME
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_IMAGES_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_IMAGES_DIR_NAME
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_BSDS_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_BSDS_DIR_NAME
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_ROM_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_ROM_DIR_NAME
		# need to check ??

	elif [[ -e $COTSON_IMAGES_PATH/$COTSON_DIST_DIR_NAME && 
		-e $COTSON_IMAGES_PATH/$COTSON_IMAGES_DIR_NAME && 
		-e $COTSON_IMAGES_PATH/$COTSON_BSDS_DIR_NAME && 
		-e $COTSON_IMAGES_PATH/$COTSON_ROM_DIR_NAME ]]; then
		echo_i "COTSon directory for images already exist."
	else
		echo_e "ERROR in COTSon directory structure"
		echo_i "try to recreate the directory structure"
		echo_d "mkdir $COTSON_IMAGES_PATH"
		sudo mkdir $COTSON_IMAGES_PATH
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_DIST_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_DIST_DIR_NAME
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_IMAGES_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_IMAGES_DIR_NAME
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_BSDS_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_BSDS_DIR_NAME
		echo_d "mkdir -p $COTSON_IMAGES_PATH/$COTSON_ROM_DIR_NAME"
		sudo mkdir -p $COTSON_IMAGES_PATH/$COTSON_ROM_DIR_NAME
		ls -l $COTSON_IMAGES_PATH
		#exit 1
	fi
}

function usage()
{
	echo -e "
  \n  COTSon bootstrap utiliy\n
  ATTENTION: Require ROOT permitions!
  example: sudo ./bootstrap.sh [OPTIONS]
  OPTIONS:
  \t-h, --help: show this message
  \t    --simnow_dir simnow_location: set simnow dir
  \t-a, --all install packages, download image and generate bsds
  \t-d, --debug: run in debug mode
  \t-v, --verbose: run in verbose mode\n"
	exit 1
}

function check_simnow()
{
	echo_d "PARMS: $@"
	if [[ "$SIMNOW_DIR" != "" ]]; then
		g_simnow_dir=$SIMNOW_DIR
	fi
	if [[ "$g_simnow_dir" == "" ]]; then
		bdir1="${root}"
		bdir2=`dirname $bdir1`
		bdir3=`dirname $bdir2`
		bdir4=`dirname $bdir3`
		sdir="simnow-linux64-${SIMNOW_SUPPORTED_VER}"
		# Make some attempt to automatically locate a simnow installation
		if [[ -d "$bdir4/$sdir" ]]; then g_simnow_dir="$bdir4/$sdir"; fi
		if [[ -d "$bdir3/$sdir" ]]; then g_simnow_dir="$bdir3/$sdir"; fi
		if [[ -d "$bdir2/$sdir" ]]; then g_simnow_dir="$bdir2/$sdir"; fi
		if [[ -d "$bdir1/$sdir" ]]; then g_simnow_dir="$bdir1/$sdir"; fi
	fi

	if [[ "$g_simnow_dir" == "" ]]; then
		echo ""
		echo "ERROR: Cannot find SimNow"
		echo "       You have to define the SIMNOW_DIR env variable"
		echo "       or pass '--simnow_dir <simnow_directory>' to $0"
		echo ""
		echo "       To download a free copy of simnow, please visit"
		echo "       http://developer.amd.com/cpu/simnow"
		echo ""
		exit 1
	fi

	if [[ ! -d "$g_simnow_dir" ]]; then
		echo_e "ERROR: Directory '$g_simnow_dir' not found..."
		exit 1
	fi

	#info
	echo_i "Detected SIMNOW_DIR: '$g_simnow_dir'"
	return 0
}

function check_root_permission()
{
	if [ "$(id -u)" != "0" ]; then
		echo_e "This script must be run as root" 1>&2
		echo_i "Do you want continue as root? [y/n] "
		read answare
		if [ "$answare" = "y" ]; then
			echo_i "ASK PASSWORD"
		else
			echo_e "Unauthorized. Exit." 1>&2
			exit 1
		fi
	else
		echo_i "Permissions ok, you are root."
	fi
}

function reset_status_file()
{
        if [ -s $STATUS_FILE_PATH/bootstrap.status ]; then
		echo_d "* Reset bootstrap.status"
		echo "" > bootstrap.status
		sudo cp bootstrap.status $STATUS_FILE_PATH
		rm bootstrap.status
        fi
}

function write_status_file()
{
	local message
	message="$1"
	echo_d "Start function: write_status_file: message = $message"
	echo_d "If the file does not exist in $STATUS_FILE_PATH try to create"
	if [ ! -e "$STATUS_FILE_PATH/bootstrap.status" ]; then
		echo_i "* Creating $STATUS_FILE_PATH/bootstrap.status"
		echo_d "Status file bootstrap.status does not exist."
		echo "" > bootstrap.status
		sudo cp bootstrap.status $STATUS_FILE_PATH
		rm bootstrap.status
	fi
	local tmp_status_file_name
	tmp_status_filename=`mktemp -u tmp_status.XXXXXXXXXX`
	cp $STATUS_FILE_PATH/bootstrap.status $tmp_status_filename
	echo_d "Temporary name of status file: $tmp_status_filename"
	echo "$message" >> $tmp_status_filename
	echo_d "Copy $tmp_status_filename to $STATUS_FILE_PATH/bootstrap.status"
	sudo cp $tmp_status_filename $STATUS_FILE_PATH/bootstrap.status
	echo_d "Delete temporary file: $tmp_status_filename"
	rm $tmp_status_filename
}

function prepare_status_file_destination()
{
        if [ ! -d $STATUS_FILE_PATH ]; then
		echo_i "* Creating directory '$STATUS_FILE_PATH'"
		echo_d "STATUS_FILE_PATH=$STATUS_FILE_PATH"
		echo_d "Superuser permissions are needed for creating $STATUS_FILE_PATH"
		sudo mkdir $STATUS_FILE_PATH 2>/dev/null
	fi
	write_status_file "#INIT"
}

######################################################################## MAIN ##
while [ $# -gt 0 ]
do
	case $1 in
			--simnow_dir) g_simnow_dir=$2; shift;;
			-d|--debug) DEBUG=1; shift;;
			-v|--verbose) VERBOSE=1; shift;;
			-f|--force) FORCE=1; shift;;
			-a|--all) ALL_MODE=1; shift;;
			--help|-h) usage;;
			*) shift;;
	esac
done

if [ "$FORCE" = 0 ]; then
	if [ -e "$OUT_FILE" ]; then
		echo_i "bootstrap.status is present"
		#echo_w "need to check also the version of distribution inside the file and on the system"
		
		local_dist=`lsb_release -i -s`
		local_ver="$VER"
		local_vernum=`lsb_release -r -s`
		echo_d "LOCAL DIST=$local_dist"
		echo_d "LOCAL VER=$local_ver"
		echo_d "LOCAL VERNUM=$local_vernum"
		source $OUT_FILE
		echo_d "$OUT_FILE information:"
		echo_d "   DIST=$DIST"
		echo_d "   DIST1=$DIST1"
		echo_d "   VER=$VER"
		echo_d "   VERNUM=$VERNUM"
		echo_d "   _detect_os=$_detect_os"
		echo_d "   make_dependencies=$make_dependencies"
		echo_d "   BOOTSTRAP=$BOOTSTRAP"
		if [[ "$BOOTSTRAP" = "" || "$BOOTSTRAP" = "KO" ]]; then
			echo_w "problems detected in bootstrap.status"
			echo_i "Run again bootstrap.sh"
		elif [[ "$local_dist" != "$DIST" || "$local_vernum" != "$VERNUM" ]]; then
			echo_e "Your distribution is changed from $VERNUM to $VERNUM_UP"
			echo_e "I try to fix the problem"
			echo_d "Reset bootstrap.status file"
			reset_status_file
		else
			echo_i "Checking bootstrap.status: OK"
			echo_w "bootstrap.sh is already been launched"
			exit 0
		fi
	else
		echo_i "bootstrap.status isn't present"
	fi
else
	echo_d "FORCE mode enabled. Jumped check on bootstrap.status file"
	echo_i "Force mode enabled."
	reset_status_file
fi
prepare_status_file_destination
cotson_images_dirs
write_status_file "# bootstrap.sh status file"
sysctl_tuning
make_dependencies
write_status_file "BOOTSTRAP=\"OK\""

########################################################################
if [ -s ./configure ]; then
	# Fix missing rgb.txt if needed
	if [[ ! -f /etc/X11/rgb.txt ]]; then
		cp etc/rgb.txt /etc/X11/rgb.txt
	fi
fi
echo_i "System is ready for COTSon installation."

if [ "$ALL_MODE" = 1 ]; then
	if [ ! -s add-image.sh ]; then
        	echo_e "ERROR: add-image.sh not found (it is necessary!)"
		exit 1
        else
		check_simnow "NULL"
		echo_i "Download default image: $DIST_IMAGE"
		echo_d "Now RUN add-images"
		./add-image.sh --simnow_dir $g_simnow_dir $DIST_IMAGE
		ret=$?
		if [ $ret != 0 ]; then
			echo_e "Error in add-image.sh: code = $ret"
			exit 1
		fi
	fi
fi

if [ -s ./configure ]; then
   echo_ok "Now you can run: ./configure"
else
   echo "Goodbye."
fi
