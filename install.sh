#!/bin/bash

cd `dirname $0`
FIREPATH=`pwd`
gcc shufenet.c -o shufenet -l pcap -l net
gcc eap_logoff.c -o eap_logoff -l pcap -l net

rm -f /etc/init.d/shufenet

echo 'SHUFENET_HOME='$FIREPATH$'\12' >> /etc/init.d/shufenet
cat sn_serv >> /etc/init.d/shufenet

chown root ./sn.profile
chmod 755 ./shufenet
chmod 755 ./eap_logoff
chmod 700 ./sn.profile
chmod 700 /etc/init.d/shufenet
