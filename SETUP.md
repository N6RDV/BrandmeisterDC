[Back to Readme](README.md "Back to Readme")

# Setup

## Installation script
Simplest way is to use the installation script.
```
rpi-rw
curl -H 'Cache-Control: no-cache' https://raw.githubusercontent.com/N6RDV/BrandmeisterDC/main/install.sh | bash
```

## Manual setup
Alternatively BranmeisterDC can be compiled, installed and configured manually.
1. Clone repository
```
rpi-rw
rm -rf /tmp/BrandmeisterDC
git clone https://github.com/N6RDV/BrandmeisterDC.git /tmp/BrandmeisterDC
cd /tmp/BrandmeisterDC
```
2. Compile and install executable
```
make
cp -i BrandmeisterDC /opt/
```
3. Start BrandmeisterDC manually, or configure a service.
See [USAGE](USAGE.md) for more details.

## Configuration
BranmeisterDC uses pahs to MMDVM Host configuration and BM API key files relevant for the `Pi-Star v4.1.6` by default.

MMDVM Host configuration is expected to be found in `/etc/mmdvmhost`

MMDVM Host logs are expected to be found in `/var/log/pi-star/MMDVM-%F.log` where `%F` is replaced with current UTC date in `Y-m-d` format.

BM API key is execpted to be found in `/etc/bmapi.key`

**Important!** 

Make sure DM API key is set by running commad `cat /etc/bmapi.key`. If it's not set - wollow [these instructions](http://wiki.pistar.uk/PI-Star_integration_with_BrandMeister_API).

Additional parameters are initialized with default values too, but can be added to the MMDVM Host configuration file and conveniently edited on the Pi-Star `Configruation > Expert > MMDVMHost` page.
BrandmeisterDC settings are added to MMDVM Host configuration file automaticaly by the installation script or can be added manually.
```
[Brandmeister DC]
VerbosityLevel=5
DisconnectGroup=4000
ScanInterval=1000
MMDVMLog=/var/log/pi-star/MMDVM-%F.log
```
