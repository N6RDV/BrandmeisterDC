[Back to Readme](README.md "Back to Readme")

# Usage

## Flags
Run `/usr/local/bin/BrandmeisterDC -h` for details

## Startup

### Console mode
```
sudo /usr/local/bin/BrandmeisterDC
```
In console mode BrandmeisterDC verbose output is directed to terminal

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
Wants=mmdvmhost.service

[Service]
User=root
Type=forking
WorkingDirectory=/usr/local/etc/
ExecStart=/usr/local/bin/BrandmeisterDC -d
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
4. Enable autostart
```
sudo systemctl daemon-reload
sudo systemctl enable brandmeisterdc.service
```
5. Reboot
```
sudo reboot
```
6. Check status after reboot
```
sudo systemctl status brandmeisterdc.service
```

Daemon service can be stoppeb/started by running commands
```
sudo service brandmeisterdc stop
sudo service brandmeisterdc start
```
Output should be seen in `/var/log/syslog`
