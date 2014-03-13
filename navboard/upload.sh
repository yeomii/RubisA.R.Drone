#!/bin/sh

ftp -n <<EOF
open 192.168.1.1
user blua
cd bin
send ../bin/navboard
bye
EOF


