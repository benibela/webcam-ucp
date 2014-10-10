#/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/../../../manageUtils.sh

mirroredProject webcam-ucp

BASE=$HGROOT/programs/graphics/WebcamTrackedUCP

case "$1" in
mirror)
  syncHg  
;;

esac

