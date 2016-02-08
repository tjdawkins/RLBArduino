Installing bluetooth as a service
1. Extract the bluetooth-service.tar.gz
2. mkdir /home/root/bluetooth
3. cd /home/root/bluetooth
4. mv /home/root/bluetooth-service.tar.gz ./
5. tar -xvf bluetooth-service.tar.gz
6. cp bluetooth-spp-pin.service /lib/systemd/system
7. systemctl enable bluetooth-spp-pin
8. reboot
9. systemctl status bluetooth-spp-pin

https://software.intel.com/en-us/blogs/2015/05/19/communicate-to-arduino-code-with-your-android-phone-by-bluetooth-serial-port



Add bluetooth startup to init

1. Create /etc/init.d/enableBT.sh
2. rfkill unblock Bluetooth
3. chmod +x 
4. update-rc.d enableBT.sh defaults
