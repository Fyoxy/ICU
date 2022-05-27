"""
* | File        :   connect.py
* | Author      :   Fyoxy
* | Function    :   Automatic launch of program
* | Info        :   Waits for the controller to connect to the 
*                   system and identifies it by the MAC address.
*                   After that executes the specified file.
*                
*----------------
* |	This version:   V1.0
* | Date        :   27.05.2022
* | Info        :   Working state
*
"""

import pydbus, os, time

bus = pydbus.SystemBus()

controller_mac = "xx:xx:xx:xx"
adapter = bus.get('org.bluez', '/org/bluez/hci0')
mngr = bus.get('org.bluez', '/')

def list_connected_devices():
    mngd_objs = mngr.GetManagedObjects()
    for path in mngd_objs:
        con_state = mngd_objs[path].get('org.bluez.Device1', {}).get('Connected', False)
        if con_state:
            addr = mngd_objs[path].get('org.bluez.Device1', {}).get('Address')
            if addr == controller_mac:
                print(f'Device [{addr}] is connected')
                os.system("sudo /home/pi/icu_v2/icu/main.out")
                quit()
    print("No device found, sleeping for 5s")
    time.sleep(5)
    list_connected_devices()


if __name__ == '__main__':

    list_connected_devices()
