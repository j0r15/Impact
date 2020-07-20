# Manual SatePi

Download full desktop image of raspbian

Checksum
~~~
sha256sum 2019-09-26-raspbian-buster-full.zip | grep put here the sha256sum from the download page
~~~
If you get an output then you can continue

### Burn image to sd card
~~~
sudo dd bs=4M if=~/Downloads/2019-09-26-raspbian-buster-full.img of=/dev/mmcblk0 conv=fsync
~~~

Go into the boot partition

Enabling ssh
~~~
sudo touch ssh
~~~

Headless wifi setup

~~~
sudo nano wpa_supplicant.conf
~~~

Append

~~~
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=ES

network={
    ssid="SSID"
    psk="PASSWORD"
    id_str="NICKNAME"
}

# For a second ssid with priority 2
#network={
#    ssid="HomeNetworkSSID"
#    psk="passwordHome"
#    priority=2
#    id_str="home"
#}

~~~

Put your SD in the RPi and give it power.

Now find it on the network using:
~~~
sudo nmap -sP 192.168.1.*

sudo nmap -sP 192.168.5.* | grep -B 2 'B8:27:EB:1A:81:71'
sudo nmap -sP 192.168.11.* | grep -B 2 'B8:27:EB:1A:81:71'
~~~

Connect to it

~~~
ssh pi@ip.address.of.rpi
~~~

Command below allow you to run programs with gui in a terminal through ssh

~~~
ssh -X pi@ip.address.of.rpi
~~~

Change passwd

~~~
sudo passwd pi
~~~


### Nodered

Make nodered start at boot as service
~~~
sudo systemctl enable nodered.service
~~~
~~~
sudo reboot
~~~

Go to ip.address.of.rpi:1880/

Import the flows and install the packages that are missing
(
osc
dmxusbpro
mongodb2
dashboard
midi
)

### VNC

~~~
crontab -e
~~~

~~~
@reboot sleep 60 && /usr/bin/vncserver -u pi -randr=1920x1080
~~~

### USB SOUNDCARD

~~~
sudo nano /usr/share/alsa/alsa.conf
~~~

~~~
defaults.ctl.card 1
defaults.pcm.card 1
~~~

### MOSQUITTO
~~~
sudo apt-get install mosquitto mosquitto-clients
~~~

### Make RPI an Access Point

~~~
sudo apt-get update
sudo apt-get upgrade
~~~

~~~
sudo apt-get install hostapd dnsmasq
~~~

~~~
sudo nano /etc/dhcpcd.conf
~~~

~~~
interface wlan1
static ip_address=192.168.42.10/24
static routers=192.168.42.0
nohook wpa_supplicant
~~~

~~~
sudo service dhcpcd restart
~~~

~~~
sudo nano /etc/network/interfaces
~~~

~~~
auto lo

allow-hotplug wlan0
iface wlan0 inet static
    address 192.168.42.10
    netmask 255.255.255.0
    network 192.168.42.0
    broadcast 192.168.42.255

auto wlan1
iface wlan1 inet dhcp
~~~

~~~
sudo nano /etc/hostapd/hostapd.conf
~~~

~~~
interface=wlan1
driver=nl80211
hw_mode=g
channel=2
wmm_enabled=1
ieee80211n=1
macaddr_acl=0
ignore_broadcast_ssid=0
# Use WPA2
auth_algs=1
wpa=2
wpa_key_mgmt=WPA-PSK
rsn_pairwise=CCMP
# This is the name of the network
ssid=SSID_HEADLESS_RPI
# The network passphrase
wpa_passphrase=PASSWORD_HEADLESS_RPI
~~~

~~~
sudo nano /etc/default/hostapd
~~~

~~~
DAEMON_CONF="/etc/hostapd/hostapd.conf"
~~~

~~~
sudo nano /etc/init.d/hostapd
~~~

~~~
DAEMON_CONF=/etc/hostapd/hostapd.conf
~~~

~~~
sudo mv /etc/dnsmasq.conf /etc/dnsmasq.conf.orig
~~~

~~~
sudo nano /etc/dnsmasq.conf
~~~

~~~
interface=wlan1      # Use interface wlan0 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  name of interface of USB WIFI
listen-address=192.168.42.10   # Specify the address to listen on
bind-interfaces      # Bind to the interface
server=              # Use DNS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Open DNSserver like dnswatch
domain-needed        # Don't forward short names
bogus-priv           # Drop the non-routed address spaces.
dhcp-range=192.168.42.50,192.168.42.150,12h # IP range and lease time
~~~

~~~
sudo nano /etc/sysctl.conf
~~~

Enable

~~~
net.ipv4.ip_forward=1
~~~

~~~
sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
~~~

~~~
sudo iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE
~~~

~~~
sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"
~~~

Make IP tables permanent (check if structure works)

~~~
sudo nano /etc/rc.local
~~~

Add above “exit 0”:

~~~
iptables-restore < /etc/iptables.ipv4.nat
~~~

~~~
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl start hostapd

sudo service hostapd unmask
sudo service hostapd enable
sudo service hostapd start
sudo service dnsmasq start
~~~

~~~
sudo reboot
~~~


sudo iptables -t nat -A POSTROUTING -o wlan1 -j MASQUERADE
sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"

<!--
rc.local:

iptables-restore < /etc/iptables.ipv4.nat


sudo apt-get install bridge-utils

sudo brctl addbr br0

sudo brctl addif br0 wlan1

sudo nano /etc/network/interfaces

auto br0
iface br0 inet manual
bridge_ports wlan1 wlan0 -->



### Pifi Hat

~~~bash
sudo apt-get update
~~~

~~~bash
sudo nano /boot/config.txt
~~~

append

~~~bash
dtparam=i2s=on
dtoverlay=hifiberry-dacplus
dtoverlay=lirc-rpi:gpio_in_pin=26
dtoverlay=lirc-rpi
~~~

disable or delete

~~~bash
dtparam=audio=on
~~~


### MongoDB2 For Nodered Saving DMX represents

~~~bash
sudo apt-get install mongodb
cd ~/.node-red
npm install node-red-node-mongodb
sudo /etc/init.d/mongodb start
mongo
use myDB
db.myCollection.insert({})
db.dmx_presets.insert({})
show dbs
show collections
~~~

example flow:

~~~json
 [{"id":"549e74e9.ab618c","type":"mongodb","z":"117df726.ee8209","hostname":"127.0.0.1","port":"27017","db":"myDB","name":""},{"id":"fd36e356.02c92","type":"mongodb out","z":"117df726.ee8209","mongodb":"549e74e9.ab618c","name":"","collection":"myCollection","payonly":true,"upsert":false,"multi":false,"operation":"insert","x":330,"y":40,"wires":[]},{"id":"2eb97340.d1468c","type":"mongodb in","z":"117df726.ee8209","mongodb":"549e74e9.ab618c","name":"","collection":"myCollection","operation":"find","x":330,"y":100,"wires":[["a87a8357.57858"]]},{"id":"85f4461.f7a0bb8","type":"http in","z":"117df726.ee8209","name":"GET /api/all","url":"/api/all","method":"get","swaggerDoc":"","x":90,"y":100,"wires":[["2eb97340.d1468c"]]},{"id":"a87a8357.57858","type":"function","z":"117df726.ee8209","name":"Content-Type","func":"msg.headers = {\"Content-Type\" : \"application/json\"};\nreturn msg;","outputs":1,"noerr":0,"x":580,"y":100,"wires":[["25f0a04d.da0f6"]]},{"id":"25f0a04d.da0f6","type":"http response","z":"117df726.ee8209","name":"","x":730,"y":100,"wires":[]}]
~~~

### Sources

<https://www.cyberciti.biz/faq/how-to-create-bridge-interface-ubuntu-linux/>

<https://www.instructables.com/id/How-to-make-a-WiFi-Access-Point-out-of-a-Raspberry/>

<https://scsynth.org/t/connecting-to-scsynth-running-on-a-raspberry-pi-wirelessly-from-scide-on-a-laptop/715/8>

<https://grantwinney.com/creating-music-with-sonic-pi-on-the-raspberry-pi/#what-is-osc>

<http://sam.aaron.name/2012/11/02/supercollider-on-pi.html>

<https://raspberrypi.stackexchange.com/questions/44/why-is-my-audio-sound-output-not-working>

<https://learn.adafruit.com/usb-audio-cards-with-a-raspberry-pi?view=all>

<https://stevenaeola.github.io/node-red-contrib-music/getting-started>

<http://reactivated.net/writing_udev_rules.html#udevinfo>

<https://raspberrypi.stackexchange.com/questions/89803/access-point-as-wifi-router-repeater-optional-with-bridge/89804#89804>

<https://mertarauh.com/2017/04/22/raspberry-pi-3-hifi-dj-mixxx-with-pifi-dac/>
