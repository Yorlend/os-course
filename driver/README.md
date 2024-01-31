https://www.kernel.org/doc/html/v4.15/driver-api/usb/usb.html#the-usb-devices

Information from /sys/kernel/debug/devices about fingerprint device:

    T:  Bus=03 Lev=01 Prnt=01 Port=03 Cnt=01 Dev#=  2 Spd=12   MxCh= 0
    D:  Ver= 2.00 Cls=02(comm.) Sub=01 Prot=01 MxPS=64 #Cfgs=  1
    P:  Vendor=27c6 ProdID=5125 Rev= 2.00
    S:  Manufacturer=Shenzhen Goodix Technology Co.,Ltd.
    S:  Product=Goodix Fingerprint Device 
    S:  SerialNumber=00000000001A
    C:* #Ifs= 2 Cfg#= 1 Atr=60 MxPwr=100mA
    I:* If#= 0 Alt= 0 #EPs= 1 Cls=02(comm.) Sub=01 Prot=01 Driver=(none)
    E:  Ad=82(I) Atr=03(Int.) MxPS=   8 Ivl=16ms
    I:* If#= 1 Alt= 0 #EPs= 2 Cls=0a(data ) Sub=00 Prot=00 Driver=(none)
    E:  Ad=01(O) Atr=02(Bulk) MxPS=  64 Ivl=0ms
    E:  Ad=81(I) Atr=02(Bulk) MxPS=  64 Ivl=0ms

Spd=12 - 12 Mbit/s for full speed USB.

Configuration consists of 2 interfaces (I:)

    I:* If#= 0 Alt= 0 #EPs= 1 Cls=02(comm.) Sub=01 Prot=01 Driver=(none)
    I:* If#= 1 Alt= 0 #EPs= 2 Cls=0a(data ) Sub=00 Prot=00 Driver=(none)
