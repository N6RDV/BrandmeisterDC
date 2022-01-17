[Back to Readme](README.md "Back to Readme")

# Usage

## Flags
Run `/opt/BrandmeisterDC -h` for details

## Startup

### Debug mode
```
sudo /usr/local/bin/BrandmeisterDC -d
```
In debug mode BrandmeisterDC verbose output is directed to terminal

## Daemon mode
1. Create service file
To create service file run following commands
```
rpi-rw
sudo touch /lib/systemd/system/brandmeisterdc.service
sudo nano /lib/systemd/system/brandmeisterdc.service
```
2. Copy/paste service definition
```
[Unit]
Description=BrandmeisterDC service
DefaultDependencies=no
After=local-fs.target wifi-country.service network-online.target
Before=timers.target
Requires=mmdvmhost.service

[Service]
User=root
Type=forking
WorkingDirectory=/usr/local/etc/
ExecStart=/usr/local/bin/BrandmeisterDC
ExecStop=/usr/bin/killall BrandmeisterDC

[Install]
WantedBy=multi-user.target
```
3. Save changes and exit by pressing
```
Ctrl-O
Enter
Ctrl-X
```

4. Reboot
```
sudo reboot
```

Daemon service can be stoppeb/started by running commands
```
service nextiondriver stop
service nextiondriver start
```