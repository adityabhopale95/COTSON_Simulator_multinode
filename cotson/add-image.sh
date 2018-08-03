#!/bin/bash
#
# AUTHOR:	: SV (viola@dii.unisi.it)
# TITLE		: add-images
# LICENSE	: MIT
# VERSION	: 0.0.4
# DATE		: 20160509
# NOTES		: NULL
#
#===============================================================================

### GLOBAL DEFINITIONS ###
RUN_USERNAME=$(logname)
PNAME="add-images"
PVERSION="0.0.4"
LPATH_IMAGES="/opt/cotson"
DIST_DIR="dist"
IMAGES_DIR="images"
BSDS_DIR="bsds"
ROMS_DIR="roms"

IMAGES_LIST_DEFAULT_PATH="images.list"
G_MENU_CHOISE=""

IMAGE_LZMA_COMPRESSION_EXTENSION=".img.lzma"
IMG_LZMA_EXT=$IMAGE_LZMA_COMPRESSION_EXTENSION

IMAGE_DECOMPRESSED_EXTENSION=".img"
IMG_EXT=$IMAGE_DECOMPRESSED_EXTENSION

BASEPATH_STATUS_FILE="/var/lib"
DIR_NAME_STATUS_FILE="cotson"
STATUS_FILE_PATH="$BASEPATH_STATUS_FILE/$DIR_NAME_STATUS_FILE"

# ToDo: decidere da dove prendere il path di cotson
COTSON_DATA_PATH="data/"

TEMPORARY_DIR="$HOME/add-image_tmp_dir"

SIMNOW_SUPPORTED_VER="4.6.2pub"
G_SIMNOW_DIR=""

DEBUG=0
VERBOSE=0
INTERACTIVE=0
LIB_MODE=0
CALL_LIST_IMAGE=0

declare -a A_IMAGES

trap "exit 1" TERM
export TOP_PID=$$

### FUNCTIONS ###

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
		verbose_str="* "
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

###

function usage()
{
	echo_d "$PNAME $PVERSION"
	echo -e "
  \n  COTSon $PNAME utiliy\n
  example: sudo ./$PNAME.sh [OPTIONS] IMAGE-NAME
  OPTIONS:
  \t-h,  --help: show this message
  \t-d, --debug: run in debug mode
  \t-l,  --list: list of IMAGE-NAME available\n
  EXAMPLE:
  \t./add-image.sh karmic64
  \t./add-image.sh --debug karmic64\n"
	exit 1
}

function _get_name()
{
	line=$1
	local ret=`echo $line | awk {'print $1'}`
	echo "$ret"
}

function _get_version()
{
	line=$1
	local ret=`echo $line | awk {'print $2'}`
	echo "$ret"
}
function _get_time()
{
	line=$1
	local ret=`echo $line | awk {'print $3'}`
	echo "$ret"
}

function _get_url()
{
	line=$1
	local ret=`echo $line | awk {'print $4'}`
	echo "$ret"
}

function _get_lpath()
{
	line=$1
	local ret=`echo $line | awk {'print $5'}`
	echo "$ret"
}

function _get_md5sum()
{
	line=$1
	local ret=`echo $line | awk {'print $6'}`
	echo "$ret"
}

function _get_other()
{
	line=$1
	local ret=`echo $line | awk {'print $7'}`
	echo "$ret"
}

#
# Function: images_list_parser
#  This function parse the file images.list and generate the data for the ARRAY
#  A_IMAGES shared globally
#
function images_list_parser()
{
	IFS=$' \t\n'
	IFS=$'\n'
	all_content=`grep -v "#" < $IMAGES_LIST_DEFAULT_PATH`
	A_IMAGES=()
	index=0
	for line in $all_content; do
		A_IMAGES[$index]=$line
		index=$(($index + 1))
	done
	IFS=$' \t\n'
}

#
# Function: prepare_image
#  This function extract the image from lzma container
#
function prepare_image()
{
	local image_path=$1
	local local_path=$(_get_lpath "$G_MENU_CHOISE")
	local_path=`eval echo $local_path`
	echo_d "Checking compression type: to be implement.."
	local compression_type=`file $image_path`
	$(lzma -dc -S .lzma $image_path > $TEMPORARY_DIR/$(_get_name "$G_MENU_CHOISE").img; touch ok_ ) &
	local index=0
	animation_progress_pattern=("-" "\\" "|" "/")
	while [ 1 ]; do
		echo -ne "\r* Uncompressing image [${animation_progress_pattern[$index]}]"
		if [ -e "ok_" ]; then
			rm "ok_"
			echo -ne "\r"
			echo_i "   Uncompression end END!"
			echo_i "Copy uncompressed image to $local_path/images/"
			sudo cp $TEMPORARY_DIR/$(_get_name "$G_MENU_CHOISE").img $local_path/images/
			return
		fi
		sleep 0.2
		index=$(( $index+1 ))
		if [ $index -gt 3 ]; then
			index=0
		fi
	done
}

#
# Function: _download_image
#  This function download from url the system image for cotson
#
function _download_image()
{
	local url="$1/cotson_os_images/dists/$(_get_name "$G_MENU_CHOISE").img.lzma"
	local destination=$2
	echo_d "DESTINATION: $destination"
	echo_i "Try to download it..."
	if [ -e "data/$(_get_name "$G_MENU_CHOISE").img.lzma" ]; then
		echo_i "The image is present in data path: data/$(_get_name "$G_MENU_CHOISE").img.lzma"
		kill -s TERM $TOP_PID
		return 0
	fi
	if [ ! -e $destination ]; then
		# for save with same name use -O -J
		curl --insecure --progress-bar $url > "$TEMPORARY_DIR/$(_get_name "$G_MENU_CHOISE").img.lzma"
		echo_d "Get md5sum from downloaded image"
		local local_md5sum=`md5sum $TEMPORARY_DIR/$(_get_name "$G_MENU_CHOISE").img.lzma | awk {'print $1'}`
	else
		echo_i "The image is present in local path: $destination"
		echo_d "Get md5sum from local path image: $destination"
		local local_md5sum=`md5sum $destination | awk {'print $1'}`
	fi
	echo_i "Checking the MD5SUM..."
	echo_d "Get md5sum from images.list"
	local original_md5sum=$(_get_md5sum "$G_MENU_CHOISE")
	echo_d "   local_md5sum = $local_md5sum"
	echo_d "original_md5sum = $original_md5sum"
	if [ $local_md5sum = $original_md5sum ]; then
		echo_i "MD5SUM OK"
		echo_i "Copy image to destination dir: $destination"
		sudo cp $TEMPORARY_DIR/$(_get_name "$G_MENU_CHOISE").img.lzma $destination
	else
		echo_e "MD5SUM ERROR: Calculated: $local_md5sum - expected: $original_md5sum"
		exit 1
	fi
}

function _download_reset_bsds()
{
	distro=$2
	#echo_w "Create a GLOBAL VAR with repository path"
	url_repo="$1/cotson_os_images/bsd_reset"
	destination_path=$3
	echo_d "Assume only 1 2 4 8 16 32 reset.bsd"
	bsds_version="1 2 4 8 16 32"
	check_reset_bsds
	ret=$?
	if [ $ret = 0 ]; then
		echo_i "BSD Reset for $distro already present... exit"
		return 1
	fi
	echo_w "In the next routine maybe have permission problems for download reset.bsd"
	for elem in $bsds_version;
	do
		echo_d "Try with $elem..."
		ret=`curl -f -I --insecure --progress-bar $url_repo/${elem}p-$distro-reset.bsd 2>/dev/null | grep 200`
		echo_d "RETURN: $ret"
		if [ "$ret" != "" ]; then
			echo_i "Try to download: ${elem}p-$distro-reset.bsd"
			curl --insecure --progress-bar $url_repo/${elem}p-$distro-reset.bsd > $TEMPORARY_DIR/${elem}p-$distro-reset.bsd
			sudo cp $TEMPORARY_DIR/${elem}p-$distro-reset.bsd $destination_path/bsds/${elem}p-$distro-reset.bsd
		fi
	done
	#curl -f -I --insecure --progress-bar $url_repo
	echo_d "START function _download_reset_bsds"
	echo_d "1. Compose the URL from url_repo: $url_repo "
	echo_d "2. Get distro name: $distro"
	echo_d "3. Get destination path: $destination_path"
	echo_d "4. Download the *reset.bsd"
}

#
# Function: _check_exist_images
#   This function check if in the system are already present the system image
#   Return value:
#      3: if the image already exist in path like /opt/cotson/images
#      2: if the image already exist in path like cotson/data/
#      1: if the image.lzma already exist in path like cotson/data/
#      4: if the image not exist in any path
#      5: in all other cases...
#
function _check_exist_images()
{
	local local_path=$(_get_lpath "$G_MENU_CHOISE")
	local_path=`eval echo $local_path`
	if [ -e "$local_path/images/$(_get_name "$G_MENU_CHOISE").img" ]; then
		echo_i "Image already downloaded"
		echo_d "IMAGE: $local_path/images/$(_get_name "$G_MENU_CHOISE").img"
		return 3
		kill -s TERM $TOP_PID
	elif [ -e "data/$(_get_name "$G_MENU_CHOISE").img" ]; then
		echo_ok "Image already exist in data/ path"
		sudo cp data/$(_get_name "$G_MENU_CHOISE").img $local_path/images/
		rm data/$(_get_name "$G_MENU_CHOISE").img
		return 2
	fi
	if [ -e "data/$(_get_name "$G_MENU_CHOISE").img.lzma" ]; then
		echo_ok "Image lzma already exist in data/ path"
		sudo cp data/$(_get_name "$G_MENU_CHOISE").img.lzma $local_path/dist
		rm data/$(_get_name "$G_MENU_CHOISE").img.lzma
		return 1
		#kill -s TERM $TOP_PID
	else
		echo_i "Image: $(_get_name "$G_MENU_CHOISE") not found in local directory: $(_get_lpath "$G_MENU_CHOISE")"
		echo_d ">>> $G_MENU_CHOISE"
		echo_d "Get information of Local path from the images.list: $local_path"
		echo_i "Need download image from repository: $(_get_url "$G_MENU_CHOISE")"
		return 4
	fi
	return 5
}

function check_cotson_dir_structure()
{
	if [[ -e "$COTSON_IMAGES_PATH" && 
		-e "$COTSON_IMAGES_PATH/$COTSON_DIST_DIR_NAME" &&
		-e "$COTSON_IMAGES_PATH/$COTSON_IMAGES_DIR_NAME" &&
		-e "$COTSON_IMAGES_PATH/$COTSON_BSDS_DIR_NAME" ]]; then
		echo_i "Destination dir $(_get_lpath "$G_MENU_CHOISE") OK"
		return 0
	else
		echo_e "Destination dir not found. Need to run bootstrap.sh script."
		return 1
	fi
}

function list_images()
{
	DIST_IMAGE="karmic64"
	images_list_parser
	local index=0
	echo_i "Image list available in repository:"
	for elem in "${!A_IMAGES[@]}";
	do
		echo "    $index. $(_get_name ${A_IMAGES[$elem]})"
		index=$(( $index + 1 ))
	done
	G_MENU_CHOISE=${A_IMAGES[1]} # force to first record of repository list
	echo_d "G_MENU_CHOISE=$G_MENU_CHOISE"
	echo ""
	echo_i "Image list already downloaded:"
	local installed_images
	installed_images=$(ls -1 $(_get_lpath "$G_MENU_CHOISE")/images)
	index=0
	for elem in $installed_images;
	do
		echo -e "    $index. $elem"
		index=$(( $index + 1 ))
	done
	
	echo ""
	echo_i "BSDs ready:"
	local installed_bsds
	installed_bsds=$(ls $(_get_lpath "$G_MENU_CHOISE")/bsds)
	index=0
	for elem in $installed_bsds;
	do
		echo -e "    $index. $elem"
		index=$(( $index + 1 ))
	done
	echo ""
	exit 0
}

function check_reset_bsds()
{
	echo_i "Checking reset BSDs for $(_get_name "$G_MENU_CHOISE") image..."
	local list_bsds
	list_bsds=$(ls $(_get_lpath "$G_MENU_CHOISE")/bsds | grep $(_get_name "$G_MENU_CHOISE") | grep "reset")
	if [ "$list_bsds" = "" ]; then
		echo_i "Reset BSDs of $(_get_name "$G_MENU_CHOISE") not found."
		return 1
	fi
	echo_i "Reset BSDs already present..."
	return 0
}

function check_bsds()
{
	echo_i "Checking BSDs for $(_get_name "$G_MENU_CHOISE") image..."
	local list_bsds
	list_bsds=$(ls $(_get_lpath "$G_MENU_CHOISE")/bsds | grep $(_get_name "$G_MENU_CHOISE") | grep -v "reset" | grep -v "boot")
	echo_d "OUTPUT: list_bsds=$list_bsds"
	if [ "$list_bsds" = "" ]; then
		echo_i "BSDs of $(_get_name "$G_MENU_CHOISE") not found."
		return 1
	fi
	echo_i "BSDs already present..."
	return 0
}

function check_simnow()
{
	echo_d "PARMS: $@"
	if [[ "$SIMNOW_DIR" != "" ]]; then
		G_SIMNOW_DIR=$SIMNOW_DIR
	fi
	if [[ "$G_SIMNOW_DIR" == "" ]]; then
		bdir1=$(pwd)
		bdir2=`dirname $bdir1`
		bdir3=`dirname $bdir2`
		bdir4=`dirname $bdir3`
		echo_d "bdir1=$bdir1"
		echo_d "bdir2=$bdir2"
		echo_d "bdir3=$bdir3"
		echo_d "bdir4=$bdir4"
		sdir="simnow-linux64-${SIMNOW_SUPPORTED_VER}"
		# Make some attempt to automatically locate a simnow installation
		echo_d " 1: $bdir4/$sdir"
		echo_d " 1: $bdir3/$sdir"
		echo_d " 1: $bdir2/$sdir"
		echo_d " 1: $bdir1/$sdir"
		
		if [[ -d "$bdir4/$sdir" ]]; then G_SIMNOW_DIR="$bdir4/$sdir"; fi
		if [[ -d "$bdir3/$sdir" ]]; then G_SIMNOW_DIR="$bdir3/$sdir"; fi
		if [[ -d "$bdir2/$sdir" ]]; then G_SIMNOW_DIR="$bdir2/$sdir"; fi
		if [[ -d "$bdir1/$sdir" ]]; then G_SIMNOW_DIR="$bdir1/$sdir"; fi
	fi

	if [[ "$G_SIMNOW_DIR" == "" ]]; then
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
	if [[ ! -d "$G_SIMNOW_DIR" ]]; then
		echo_e "ERROR: Directory '$G_SIMNOW_DIR' not found..."
		exit 1
	fi

	#info
	echo_i "Detected SIMNOW_DIR: '$G_SIMNOW_DIR'"
	return 0
}

function generate_bsds()
{
	echo_d "Before to start make, connect image to cotson"
	ln -sf $(_get_lpath "$G_MENU_CHOISE")/images/$(_get_name "$G_MENU_CHOISE").img $COTSON_DATA_PATH 
	echo_d "Connect reset.bsd to cotson"
	ln -sf $(_get_lpath "$G_MENU_CHOISE")/bsds/*p-$(_get_name "$G_MENU_CHOISE")-reset.bsd $COTSON_DATA_PATH
	echo_d "Fix images.mk variable"
	sed -i "s/DIST=.*/DIST=$(_get_name "$G_MENU_CHOISE")/g" $COTSON_DATA_PATH/images.mk
	sed -i "s/HDD=.*/HDD=$(_get_name "$G_MENU_CHOISE").img/g" $COTSON_DATA_PATH/images.mk
	sed -i "s/secs=.*/secs=$(_get_time "$G_MENU_CHOISE")/g" $COTSON_DATA_PATH/images.mk
	cd data
	echo_d "SIMNOW: $G_SIMNOW_DIR"
	local bsd_reset_present
	bsd_reset_present=$(ls *p-$(_get_name "$G_MENU_CHOISE")-reset.bsd)
	if [ "$bsd_reset_present" = "" ]; then
		echo_e "Impossibile generate the BSDS: $(_get_name "$G_MENU_CHOISE")-reset.bsd not found in data/ path."
		exit 1
	fi
	echo_d "bsd_reset_present= $bsd_reset_present"
	#add for for generating bsd for every reset.bsd
	for elem in $bsd_reset_present
	do
		 machine_id=`echo "$elem" | cut -d "-" -f 1`
		#./initbsd $G_SIMNOW_DIR $machine_id-$(_get_name "$G_MENU_CHOISE") $(_get_name "$G_MENU_CHOISE").img
		# Try to run in parallel the BSD generation
		####################################################################
		echo_d "COMMAND: ./initbsd "$G_SIMNOW_DIR" "$machine_id"-"$(_get_name "$G_MENU_CHOISE")" "$(_get_name "$G_MENU_CHOISE")".img"
		$(./initbsd "$G_SIMNOW_DIR" "$machine_id"-"$(_get_name "$G_MENU_CHOISE")" "$(_get_name "$G_MENU_CHOISE")".img > add-image_"$machine_id"-"$(_get_name "$G_MENU_CHOISE")".log 2>&1; touch ok_ ) &
		local index=0
		local s_index=0
		local seconds=0
		animation_progress_pattern=("-" "\\" "|" "/")
		while [ 1 ]; do
			echo -ne "\r* Generating BSDs: $machine_id-$(_get_name "$G_MENU_CHOISE") [${animation_progress_pattern[$index]}] TIME: ${seconds}s"
			if [ -e "ok_" ]; then
				rm "ok_"
				echo ""
				echo -ne "\r"
				echo_i "BSD: $machine_id-$(_get_name "$G_MENU_CHOISE") completed.                                 "
				break
			fi
			sleep 0.2
			index=$(( $index+1 ))
			s_index=$(( $s_index+1 ))
			if [ $index -gt 3 ]; then
				index=0
			fi
			if [ $s_index -gt 4 ]; then
				seconds=$(( $seconds+1 ))
				s_index=0
			fi
				
		done
		###########################################
		# add the error code check
	done
}

function _main()
{
	if [ $# -lt 1 ]; then
		echo_e "ERROR PARAMETERS!"
		usage
		exit 1
	fi
	for last; do true; done
	#echo $last
	
	if [[ "$last" = "-d" || "$last" = "-h" || "$last" = "--help" || "$last" = "-v" ]]; then
		echo_e "Not found IMAGE-NAME in parameters"
		usage
		exit 0
	fi
	if [ ! -e "$IMAGES_LIST_DEFAULT_PATH" ]; then
		echo_e "Repository file: images.list not found."
		exit 1
	fi
	images_list_parser
	local parm=$last
	local index=0
	local find="ko"
	for elem in "${!A_IMAGES[@]}";
	do
		local name=$(_get_name ${A_IMAGES[$elem]})
		echo_d "_get_name=$name"
		echo_d "shell parmeter=$parm"
		if [[ "$name" = "$parm" ]]; then
			echo_d "PARM: $parm OK"
			echo_d "finded @ index=$index"
			find="ok"
			break
		else
			find="ko"
		fi
		index=$(( $index + 1 ))
	done
	if [ "$find" = "ko" ]; then
		echo_e "IMAGE-NAME: \"$parm\" not found in images.list"
		exit 1
	fi
	G_MENU_CHOISE="${A_IMAGES[$index]}"

	if [ -e $STATUS_FILE_PATH/bootstrap.status ]; then
		source "$STATUS_FILE_PATH/bootstrap.status"
		echo_d "$STATUS_FILE_PATH/bootstrap.status information:"
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
			exit 100
		else
			echo_i "Checking bootstrap.status: OK"
			#echo_w "Need to add the check on SO version and name!"
		fi
	else
		echo_i "Checking bootstrap.status: not found"
		echo_e "Need to run bootstrap.sh"
		exit 1
	fi

	_download_reset_bsds $(_get_url "$G_MENU_CHOISE") $(_get_name "$G_MENU_CHOISE") $(_get_lpath "$G_MENU_CHOISE")
	_check_exist_images
	ret=$?
	echo_d "return _check_exist_images: $ret"
	if [ $ret = 3 ]; then
		check_bsds
		ret=$?
		if [ $ret = 1 ]; then
			echo_d "check_bsds returned 1 call generate_bsds"
			generate_bsds
			# check the precise number of bsd
			check_bsds_creation=$(ls -l *p-$(_get_name "$G_MENU_CHOISE").bsd 2>/dev/null)
			if [ "$check_bsds_creation" = "" ]; then
				echo_e "ATTENTION: BSDs haven't been generated!"
				exit 1
			else
				echo_i "Creation of the BSDs: OK"
			fi
			echo_i "Copy bsds to $(_get_lpath "$G_MENU_CHOISE")/bsds..."
			sudo cp *p-$(_get_name "$G_MENU_CHOISE").bsd $(_get_lpath "$G_MENU_CHOISE")/bsds
			sudo cp *p-$(_get_name "$G_MENU_CHOISE")-boot $(_get_lpath "$G_MENU_CHOISE")/bsds
		fi
		echo_ok "Everything is ok ... exit"
		exit 0
	fi
	local local_path=$(_get_lpath "$G_MENU_CHOISE")
	local_path=`eval echo $local_path`
	_download_image $(_get_url "$G_MENU_CHOISE") "$local_path/dist/$(_get_name "$G_MENU_CHOISE").img.lzma"
	prepare_image "$local_path/dist/$(_get_name "$G_MENU_CHOISE").img.lzma"
	generate_bsds # this function change directory to data/ (cd data/)
	ls -l *p-$(_get_name "$G_MENU_CHOISE").bsd
	echo_i "Copy bsds to $(_get_lpath "$G_MENU_CHOISE")/bsds..."
	sudo cp *p-$(_get_name "$G_MENU_CHOISE").bsd $(_get_lpath "$G_MENU_CHOISE")/bsds
	ret=$?
	if [ $ret = 0 ]; then
		echo_i "BSDs copy: DONE"
		echo_d "Removing local files"
		rm *p-$(_get_name "$G_MENU_CHOISE").bsd
	else
		echo_e "Something is gone wrong during the copy in: $(_get_lpath "$G_MENU_CHOISE")/bsds"
		echo_d "COMMAND: sudo cp *p-$(_get_name "$G_MENU_CHOISE").bsd $(_get_lpath "$G_MENU_CHOISE")/bsds"
	fi
	sudo cp *p-$(_get_name "$G_MENU_CHOISE")-boot $(_get_lpath "$G_MENU_CHOISE")/bsds
	ret=$?
	if [ $ret = 0 ]; then
		echo_i "BSDs copy: DONE"
		echo_d "Removing local files"
		rm *p-$(_get_name "$G_MENU_CHOISE")-boot
	else
		echo_e "Something is gone wrong during the copy in: $(_get_lpath "$G_MENU_CHOISE")/bsds"
		echo_d "COMMAND: sudo cp *p-$(_get_name "$G_MENU_CHOISE")-boot $(_get_lpath "$G_MENU_CHOISE")/bsds"
	fi
	echo_i "Remember: for connect new images or bsds generated, need to re-run ./configure"
}

function lib_mode()
{
	echo_d "Started add-images in lib mode"
	images_list_parser
	for elem in "${!A_IMAGES[@]}";
	do
		local name=$(_get_name ${A_IMAGES[$elem]})
		echo_d "name=$name"
		index=$(( $index + 1 ))
	done
	if [ "$find" = "ko" ]; then
		echo_e "PARM: $parm WRONG"
		exit 1
	fi
	echo_i "Parsing ./images.list: OK"
	return 0
}

######################################################################## MAIN ##

parms=$@
while [ $# -gt 0 ]
do
	case $1 in
			--simnow_dir) G_SIMNOW_DIR=$2; shift;;
			-l|--list) CALL_LIST_IMAGE=1; shift;;
			-d|--debug) DEBUG=1; shift;;
			-v|--verbose) VERBOSE=1; shift;;
			--help|-h) usage;;
			--lib) LIB_MODE=1; lib_mode; shift;; 
			*) shift;;
	esac
done

check_simnow "NULL"

echo_d "SIMNOW_DIR=$G_SIMNOW_DIR"
echo_d "CALL_LIST_IMAGE=$CALL_LIST_IMAGE"
if [ $CALL_LIST_IMAGE = 1 ]; then echo_d "Calling: list_images"; list_images; fi

if [ $LIB_MODE = 0 ]; then
	echo_i "Creating Temporary directory: $TEMPORARY_DIR"
	mkdir -p $TEMPORARY_DIR
	_main $parms
fi
echo_i "Cleaning temporary directory: $TEMPORARY_DIR"
rm -rf $TEMPORARY_DIR
echo_d "Unset the common name functions"
#unset -f echo_d
#unset -f echo_e
#unset -f echo_i
#unset -f echo_w
#unset -f echo_ok
unset -f usage


# END SCRIPT
