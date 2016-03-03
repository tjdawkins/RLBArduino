#!/bin/sh

# Make the data directory 
if [ ! -d /home/root/data ]; then
	mkdir /home/root/data
fi

# Setup bluetooth and sketch starup
if [ ! -d /etc/init.d ]; then
	mkdir /etc/init.d
fi

cp ./enableBT.sh /etc/init.d
cp ./startSketch.sh /etc/init.d
cp ./startSPP.sh /etc/init.d

chmod +x /etc/init.d/enableBT.sh
chmod +x /etc/init.d/startSketch.sh
chmod +x /etc/init.d/startSPP.sh

cd /etc/init.d

update-rc.d enableBT.sh defaults
update-rc.d startSketch.sh defaults
update-rc.d startSPP.sh defaults
