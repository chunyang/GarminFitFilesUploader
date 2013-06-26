syncruns
========

Copy .FIT files from a Garmin watch to the PC and upload them to Garmin
Connect.

Requirements
------------

 * 'mechanize' is used to automate interactions with the Garmin Connect site
    $ gem install mechanize

Configuration
-------------

Modify the constants at the top of the `syncruns` script to reflect where the
watch should be mounted and where .FIT files should be copied to on the PC.
Example configuration options are below.

    WATCH_MOUNT = '/mnt/fr10'
    FIT_DIR = '/home/cyang/etc/runs'

The entry in `/etc/fstab` corresponding to the watch is:

    # Forerunner 10
    LABEL=GARMIN /mnt/fr10 vfat ro,user 0 2

Usage
-----

After configuring the script, just run the `syncruns` script. Provide Garmin
Connect credentials for sign-in when prompted.
