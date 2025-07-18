This is the Debian/Ubuntu release V4.9.0, 64bit

To launch CoppeliaSim, run FROM THE COMMAND LINE:

$ ./coppeliaSim

or alternatively:

$ ./coppeliaSim.sh 

Issues you might run into:

1.  When trying to start CoppeliaSim, following message
    displays: "Error: could not find or correctly load
    the CoppeliaSim library"
    a) Make sure you started CoppeliaSim with
       ./coppeliaSim.sh from the command line
    b) check what dependency is missing by running
       libLoadErrorCheck.sh or with the ldd command

2.  You are using a dongle license key, but CoppeliaSim
    displays 'No dongle was found' at launch time.
    a) lsusb
    b) Make sure that the dongle is correctly plugged
       and recognized (VID:1bc0, PID:8100)
    c) sudo cp 92-SLKey-HID.rules /etc/udev/rules.d/
    d) Restart the computer
    e) Start CoppeliaSim with ./coppeliaSim 
       (or alternatively with ./coppeliaSim.sh)
    
3.  You are receiving errors with a cloud license, typically
    on Ubuntu 24. 
    a) Execute following:
       wget https://www.openssl.org/source/openssl-1.1.1w.tar.gz
       tar -xzf openssl-1.1.1w.tar.gz
       cd openssl-1.1.1w
       ./config --prefix=/usr/local/openssl-1.1.1 --openssldir=/usr/local/openssl-1.1.1
       make -j$(nproc)
       sudo make install
    b) Start CoppeliaSim with ./coppeliaSim.sh

