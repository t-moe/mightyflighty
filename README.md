# MightyFlighty

A simple flight tracking app written im qml/qt for the beagle bone black (embedded linux). This is a study project for the lecture "Embedded Linux".  
We recommend reading the online version of this file on: https://github.com/t-moe/mightyflighty

## Prequisites

* An up2date version of qt (at least 5.5) with the modules location, quick and qml.
* An internet connection (otherwise some data providers won't work).
* A synchronized clock on your linux (otherwise ssl connections will fail). Install ntp!
* An API Key from flightstats.com (otherwise you cannot use the flighstats.com data provider). Read below.

## Installation & Start

If you have been provided with an executable, just start it.
* `MightyFlightyBBB` will only work on the BeagleBone. 
* `MightyFlightyX86` will only work on PC.

If you want to build it yourself, you need to execute:
* `qmake && make` to build it for PC
* `/<ROOTFS_PATH>/usr/local/Qt5.5.1/bin/qmake MightyFlighty.pro -r -spec linux-arm-gnueabihf-g++ && make` to build it for the bbb

## Usage

* Hold and drag the map around to control the visible area
* Pinch to zoom
* Click on a plane, to see details about it.
* Click on the gear icon, to open the settings dialog, where you can change several app settings
* Click on the arrow icon, to terminate the application
* Click on the center icon, to center the map on europe.

The following features are only available on the bbb:

* Use the poti to control the zoom level
* After tapping on a plane (dialog opens), press any of the 4 buttons for some seconds in order to bind the plane to that button.
* If you press (short) on a button where a plane is bound to, the plane will be fixed in the center of the map, until you close the dialog.
* If you press (short) on a button where no plane is bound to, the current opened dialog will be closed.
* If you want to unbind a button, press it for serveral seconds, while no dialog is open.


## Common problems

### The application is not showing up on my embedded bbb, but the process has started

Make sure your profile sets the correct environment variables for qt.
Example of a correct qt configuration:

```
# Qt environment
export QT_PLUGIN_PATH=/usr/local/Qt5.5.1/plugins
export QT_QPA_FONTDIR=/usr/local/Qt5.5.1/lib/fonts
export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/Qt5.5.1/lib/plugins/platforms
export QT_QPA_PLATFORM=linuxfb
export QT_QPA_EVDEV_MOUSE_PARAMETERS=rotate=0:dejitter=10
export QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS=rotate=0
export QT_QPA_EGLFS_PHYSICAL_WIDTH=140
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=200
export QT_QPA_EGLFS_DEPTH=16
export QT_QPA_GENERIC_PLUGINS=tslib:/dev/input/event0
```


### How to get an API-Key for flightstats.com, and how to enter it on a device without keyboard?

If you're lucky you've been provided with a file `BFH.conf`, which contains a valid evaulation API Key. Copy the file to `~/.config/MightyFlighty/BFH.conf`  
If you're a stranger from the internet, reading this page, then you have to request your own API Key. You can get a free evaluation account for 30 days here: https://developer.flightstats.com/getting-started. You may enter the API-Key in the settings dialog of the application.

