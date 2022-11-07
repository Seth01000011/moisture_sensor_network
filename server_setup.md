# install tailscale

https://tailscale.com

# install apache

https://docs.fedoraproject.org/en-US/quick-docs/getting-started-with-apache-http-server/

- above link contains fast guide to getting started
  sudo dnf install httpd
  [x] completed

# edit config files

create config file at /etc/httpd/conf.d/z-foo
[ ]

# open up firewall for port 80 for local network

sudo firewall-cmd --permanent --add-service=http
sudo firewall-cmd --add-service=http
sudo firewall-cmd --add-service=mysql --permanent
sudo firewall-cmd --reload
[x] completed

# start httpd (apache) service

sudo systemctl start httpd.service
[x] completed

## set apache (httpd) to start on boot

sudo systemctl enable httpd.service
[x] completed

# move files desired to display to /var/www/html

sudo chown apache.apache \*

- above cmd will give apache ownership of the files for the webpage

[ ] always need to run this whenever modifying files on server

# install php stuff

https://computingforgeeks.com/how-to-install-lamp-stack-on-fedora/

[x] nope

trying to install lamp with python instead

https://www.digitalocean.com/community/tutorials/how-to-set-up-an-apache-mysql-and-python-lamp-server-without-frameworks-on-ubuntu-14-04

[x] mostly

# install mysql stuff

sudo dnf install mariadb-server
refer to computingforgeeks.com

- skipping unix_socket auth
- set root password
- removed anon users
- disabled remote root login
- allowed network login
- removed test database

login with $ mysql -u root -p

# test php info page with

[ip]/phpinfo.php
[x] completed

# install phpMyAdmin

sudo dnf install phpmyadmin -y
edit config file at /etc/httpd/conf.d/phpMyAdmin.conf
user is root
password is set during mariaDB installation
[x] completed

# set up sql database

[x] established database
[X] set up table to hold sensor data
[ ] set up code to analyze data in db table
[ ] code to alert and send message when values exceed parameters
[ ] create landing page to access and see current values

### left off on randomnerdtutorials esp8266 lamp server step 5 (esp32 code)

https://randomnerdtutorials.com/esp32-esp8266-raspberry-pi-lamp-server/

# add code for sensors

https://www.esp8266.com/viewtopic.php?f=32&t=4840
use POST example from
https://esp8266-shop.com/blog/how-to-http-get-and-post-requests-with-esp8266/

# look into using Python script

use to query sql database occasionally
if value if greater than certain amount
NOTIFY DAD, MOM, ME
email? Yeah, Gmail will work fine
Look into other potential services
