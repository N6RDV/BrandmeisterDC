#!/bin/bash

CONF_SECTION="Brandmeister DC"
DEFAULT_CONF="[$CONF_SECTION]\nVerbosityLevel=5\nDisconnectGroup=4000\nScanInterval=1000\nMMDVMLog=/var/log/pi-star/MMDVM-%%F.log"
MMDVMHOST=/etc/mmdvmhost
BMAPI=/etc/bmapi.key
GIT_REPO=https://github.com/N6RDV/BrandmeisterDC.git
WORKING_DIR=/tmp/BrandmeisterDC/

printf "\n1. CLONING GITHUB REPOSITORY\n\n"
if [ -d "$WORKING_DIR" ];
then
  rm -Rf $WORKING_DIR;
fi
git clone $GIT_REPO $WORKING_DIR
cd $WORKING_DIR

printf "\n2. CHECKING $MMDVMHOST CONFIGURATION FILE\n\n"
error=0

if grep -Pzq "Id=[0-9]{7,9}" <(grep -Pzo "\[DMR\][^\[]+" $MMDVMHOST);
then
  printf "ESSID: OK\n"
else
  printf "ESSID: ERROR!\n\tGo to Pi-Star configuration and make sure 'CCS7/DMR ID' is set.\n"
  error=1
fi

if grep -Pzq "Duplex=1" <(grep -Pzo "\[General\][^\[]+" $MMDVMHOST);
then
  printf "DUPLEX: OK\n"
else
  printf "DUPLEX: ERROR!\n\tGo to Pi-Star Configuration and switch 'Controller Mode' to 'Duplex Repeater'.\n"
  error=1
fi

if grep -Pzq "Password=\".+\"" <(grep -Pzo "\[DMR Network\][^\[]+" $MMDVMHOST);
then
  printf "PASSWORD: OK\n"
else
  printf "PASSWORD: ERROR!\n\tGo to Pi-Star Configuration and make sure 'Hotspot Security' is set.\n\tSee more details at http://wiki.pistar.uk/Pi-Star_Brandmeister_Hotspot_Security\n"
  error=1
fi

if grep -Pzq "apikey=.+" <(grep -Pzo "\[key\][^\[]+" $BMAPI);
then
  printf "APIKEY: OK\n"
else
  printf "APIKEY: ERROR!\n\tGo to Pi-Star Configuration > Expert > BM API and make sure 'api key' is set.\n\tSee more details at http://wiki.pistar.uk/PI-Star_integration_with_BrandMeister_API\n"
  error=1
fi

if [ "$error" -ne "0" ];
then
  printf "\nFix problems above and re-run this script.\n\n"
  exit 1
fi

sudo mount -o remount,rw / ; sudo mount -o remount,rw /boot
printf "\n3. ADDING SECTION TO $MMDVMHOST CONFIGURATION FILE\n\n"

if grep -q "\[$CONF_SECTION\]" $MMDVMHOST;
then
  printf "Aleady contains configuration for Brandmeister DC\n"
  printf "Consider updating it or replacing with default configuration manually if needed by running command 'sudo nano $MMDVMHOST'\n\n"
  printf "Default configuration\n\n"
  printf "$DEFAULT_CONF\n\n"
else
  printf "Adding configuration to the end of $MMDVMHOST\n\n"
  printf "$DEFAULT_CONF\n\n" >> $MMDVMHOST
  printf "Done.\n\n"
fi

printf "\n4. MAKING AND COPYING EXECUTABLE FILE\n\n"
make
sudo cp -f BrandmeisterDC /usr/local/bin/

printf "Success!\n\n"
printf "BrandmeisterDC can be started:\n"
printf " - in daemon mode 'sudo /usr/local/bin/BrandmeisterDC &' (output to `/var/log/syslog`);\n"
printf " - in debug mode 'sudo /usr/local/bin/BrandmeisterDC -d' (verbose output to terminal);\n"
printf " - as a service with autostart (see https://github.com/N6RDV/BrandmeisterDC/blob/main/README.md).\n"
