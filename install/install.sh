#!/bin/sh

# Make the data directory 
mkdir /home/root/data

# Setup bluetooth and sketch starup
if [ ! -d /etc/init.d]; then
	mkdir /etc/init.d
fi

cp ./enableBT.sh /etc/init.d
cp ./startSketch.sh /etc/init.d
cp ./startSPP.sh /etc/init.d

chmod +x /etc/init.d/enableBT.sh
chmod +x /etc/init.d/startSketch.sh
chmod +x /etc/init.d/startSPP.sh

cd /etc/init.d

update-rc.d enableBT.sh default
update-rc.d startSketch.sh default
update-rc.d startSPP.sh default
