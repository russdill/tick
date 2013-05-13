v 20121123 2
C 33700 31600 1 0 0 usb2412-1.sym
{
T 35600 38300 5 10 1 1 0 3 1
refdes=U1
T 35600 37900 5 10 1 1 0 3 1
device=USB2412
T 35600 37500 5 10 1 1 0 3 1
footprint=QFN-28
T 36300 31600 5 10 1 1 0 0 1
partno=638-1114-ND
}
C 32300 32800 1 0 0 gnd-1.sym
N 32400 33300 33700 33300 4
N 30500 34300 30500 34500 4
N 30100 34500 33700 34500 4
N 30500 33600 30500 33300 4
N 30100 33300 31700 33300 4
C 30100 34700 1 180 0 capacitor-1.sym
{
T 29900 34000 5 10 0 0 180 0 1
device=CAPACITOR
T 29200 34600 5 10 1 1 0 0 1
refdes=C1
T 29800 34200 5 10 1 1 0 0 1
footprint=0402
T 30100 34700 5 10 0 0 0 0 1
Title=0.1
T 29800 34600 5 10 1 1 0 0 1
value=27pF
}
N 29200 34500 28900 34500 4
N 28900 32900 28900 34500 4
N 28900 33300 29200 33300 4
C 28800 32600 1 0 0 gnd-1.sym
C 30100 33500 1 180 0 capacitor-1.sym
{
T 29900 32800 5 10 0 0 180 0 1
device=CAPACITOR
T 29200 33400 5 10 1 1 0 0 1
refdes=C2
T 29800 33000 5 10 1 1 0 0 1
footprint=0402
T 30100 33500 5 10 0 0 0 0 1
Title=0.1
T 29800 33400 5 10 1 1 0 0 1
value=27pF
}
N 31700 33300 31700 34100 4
N 31700 34100 33700 34100 4
C 26200 36800 1 0 0 usb-minib.sym
{
T 26300 39100 5 10 1 1 0 0 1
refdes=P1
T 26200 37100 5 10 1 1 0 0 1
footprint=usbmini-b
T 26200 36800 5 10 0 1 0 0 1
partno=H2960CT-ND
T 27200 37100 5 10 1 1 0 0 1
partno=H2960CT-ND
}
N 28600 38100 28600 38200 4
N 28600 38200 28400 38200 4
C 27000 36500 1 0 0 gnd-1.sym
C 28500 37200 1 0 0 gnd-1.sym
N 28600 37600 28400 37600 4
N 28600 37600 28600 37500 4
N 28400 38800 29500 38800 4
N 29500 37900 29500 38900 4
C 28500 37800 1 0 1 nc.sym
{
T 28400 39300 5 8 0 0 0 6 1
value=NoConnection
T 28400 40100 5 8 0 0 0 6 1
device=DRC_Directive
T 28400 38900 5 8 0 0 0 6 1
symversion=1.0
T 28400 39100 5 8 0 0 0 6 1
footprint=none
}
C 35700 31100 1 0 0 gnd-1.sym
C 35400 40600 1 0 0 generic-power.sym
{
T 35600 40850 5 10 1 1 0 3 1
net=3V3:1
}
N 35600 40000 35600 40600 4
N 34800 40000 34800 40300 4
N 34800 40300 35600 40300 4
N 35600 40300 36400 40300 4
N 36400 40300 36400 40000 4
N 36000 40000 36000 40300 4
N 35200 40000 35200 40300 4
C 32500 38800 1 90 0 resistor-1.sym
{
T 32100 39100 5 10 0 0 90 0 1
device=RESISTOR
T 32600 39400 5 10 1 1 0 0 1
refdes=R1
T 32600 39200 5 10 1 1 0 0 1
value=10k
T 32600 39000 5 10 1 1 0 0 1
footprint=0402
}
N 32400 39800 32400 39700 4
N 33700 37300 32000 37300 4
N 32000 37100 32000 37300 4
C 32100 36200 1 90 0 resistor-1.sym
{
T 31700 36500 5 10 0 0 90 0 1
device=RESISTOR
T 31500 36800 5 10 1 1 0 0 1
refdes=R2
T 31500 36600 5 10 1 1 0 0 1
value=10k
T 31500 36400 5 10 1 1 0 0 1
footprint=0402
}
C 31900 35800 1 0 0 gnd-1.sym
N 32000 36100 32000 36200 4
C 33100 39800 1 0 0 generic-power.sym
{
T 33300 40050 5 10 1 1 0 3 1
net=3V3:1
}
C 37800 32600 1 90 0 resistor-1.sym
{
T 37400 32900 5 10 0 0 90 0 1
device=RESISTOR
T 37900 33200 5 10 1 1 0 0 1
refdes=R3
T 37900 33000 5 10 1 1 0 0 1
value=12k
T 37900 32800 5 10 1 1 0 0 1
footprint=0402
}
N 37500 33700 37700 33700 4
N 37700 33700 37700 33500 4
C 37600 32200 1 0 0 gnd-1.sym
N 37700 32500 37700 32600 4
C 37600 34400 1 0 1 nc.sym
{
T 37500 35900 5 8 0 0 0 6 1
value=NoConnection
T 37500 36700 5 8 0 0 0 6 1
device=DRC_Directive
T 37500 35500 5 8 0 0 0 6 1
symversion=1.0
T 37500 35700 5 8 0 0 0 6 1
footprint=none
}
C 37600 34800 1 0 1 nc.sym
{
T 37500 36300 5 8 0 0 0 6 1
value=NoConnection
T 37500 37100 5 8 0 0 0 6 1
device=DRC_Directive
T 37500 35900 5 8 0 0 0 6 1
symversion=1.0
T 37500 36100 5 8 0 0 0 6 1
footprint=none
}
N 35800 31400 35800 31600 4
N 35400 31600 35400 31500 4
N 35400 31500 35800 31500 4
C 33400 38800 1 90 0 resistor-1.sym
{
T 33000 39100 5 10 0 0 90 0 1
device=RESISTOR
T 33500 39400 5 10 1 1 0 0 1
refdes=R6
T 33500 39200 5 10 1 1 0 0 1
value=10k
T 33500 39000 5 10 1 1 0 0 1
footprint=0402
}
N 33300 39800 33300 39700 4
N 32400 33100 32400 36500 4
C 34500 39900 1 0 1 nc.sym
{
T 34400 41400 5 8 0 0 0 6 1
value=NoConnection
T 34400 42200 5 8 0 0 0 6 1
device=DRC_Directive
T 34400 41000 5 8 0 0 0 6 1
symversion=1.0
T 34400 41200 5 8 0 0 0 6 1
footprint=none
}
C 36900 39900 1 0 1 nc.sym
{
T 36800 41400 5 8 0 0 0 6 1
value=NoConnection
T 36800 42200 5 8 0 0 0 6 1
device=DRC_Directive
T 36800 41000 5 8 0 0 0 6 1
symversion=1.0
T 36800 41200 5 8 0 0 0 6 1
footprint=none
}
C 32600 35200 1 0 0 resistor-1.sym
{
T 32900 35600 5 10 0 0 0 0 1
device=RESISTOR
T 32800 35500 5 10 1 1 0 0 1
refdes=R5
T 33200 35500 5 10 1 1 0 0 1
value=10k
T 32800 35000 5 10 1 1 0 0 1
footprint=0402
}
N 33500 35300 33700 35300 4
N 32400 35300 32600 35300 4
C 37600 35600 1 0 1 nc.sym
{
T 37500 37100 5 8 0 0 0 6 1
value=NoConnection
T 37500 37900 5 8 0 0 0 6 1
device=DRC_Directive
T 37500 36700 5 8 0 0 0 6 1
symversion=1.0
T 37500 36900 5 8 0 0 0 6 1
footprint=none
}
C 37600 36000 1 0 1 nc.sym
{
T 37500 37500 5 8 0 0 0 6 1
value=NoConnection
T 37500 38300 5 8 0 0 0 6 1
device=DRC_Directive
T 37500 37100 5 8 0 0 0 6 1
symversion=1.0
T 37500 37300 5 8 0 0 0 6 1
footprint=none
}
C 32500 32400 1 0 0 resistor-1.sym
{
T 32800 32800 5 10 0 0 0 0 1
device=RESISTOR
T 32700 32700 5 10 1 1 0 0 1
refdes=R7
T 33100 32700 5 10 1 1 0 0 1
value=100k
T 32700 32200 5 10 1 1 0 0 1
footprint=0402
}
C 33800 31300 1 90 0 capacitor-1.sym
{
T 33100 31500 5 10 0 0 90 0 1
device=CAPACITOR
T 32900 31800 5 10 1 1 0 0 1
refdes=C3
T 32900 31600 5 10 1 1 0 0 1
value=0.1uF
T 32900 31400 5 10 1 1 0 0 1
footprint=0402
T 33800 31300 5 10 0 0 0 0 1
Title=0.1
}
C 33500 30900 1 0 0 gnd-1.sym
N 33600 31200 33600 31300 4
N 33600 32200 33600 32500 4
N 33400 32500 33700 32500 4
C 31800 32800 1 0 0 generic-power.sym
{
T 32000 33050 5 10 1 1 0 3 1
net=3V3:1
}
N 32000 32800 32000 32500 4
N 32000 32500 32500 32500 4
N 37500 38500 38700 38500 4
{
T 37600 38500 5 10 1 1 0 0 1
netname=USBDM_DN1
}
N 37500 38100 38700 38100 4
{
T 37600 38100 5 10 1 1 0 0 1
netname=USBDP_DN1
}
C 43600 35900 1 0 1 connector_usb.sym
{
T 43500 37900 5 10 1 1 0 6 1
refdes=P2
T 43100 36200 5 10 1 1 0 0 1
footprint=usb-a
T 42800 36000 5 10 1 1 0 0 1
partno=1175-1001-ND
}
C 41300 36300 1 0 1 gnd-1.sym
N 41200 36700 41400 36700 4
N 41200 36700 41200 36600 4
C 42800 35600 1 0 1 gnd-1.sym
C 38700 39400 1 90 0 capacitor-1.sym
{
T 38000 39600 5 10 0 0 90 0 1
device=CAPACITOR
T 37800 39900 5 10 1 1 0 0 1
refdes=C4
T 37800 39700 5 10 1 1 0 0 1
value=0.1uF
T 37800 39500 5 10 1 1 0 0 1
footprint=0402
T 38700 39400 5 10 0 0 0 0 1
Title=0.1
}
C 38400 39100 1 0 0 gnd-1.sym
C 38300 40300 1 0 0 generic-power.sym
{
T 38500 40550 5 10 1 1 0 3 1
net=3V3:1
}
C 39700 39400 1 90 0 capacitor-1.sym
{
T 39000 39600 5 10 0 0 90 0 1
device=CAPACITOR
T 38800 39900 5 10 1 1 0 0 1
refdes=C5
T 38800 39700 5 10 1 1 0 0 1
value=0.1uF
T 38800 39500 5 10 1 1 0 0 1
footprint=0402
T 39700 39400 5 10 0 0 0 0 1
Title=0.1
}
C 40700 39400 1 90 0 capacitor-1.sym
{
T 40000 39600 5 10 0 0 90 0 1
device=CAPACITOR
T 39800 39900 5 10 1 1 0 0 1
refdes=C6
T 39800 39700 5 10 1 1 0 0 1
value=0.1uF
T 39800 39500 5 10 1 1 0 0 1
footprint=0402
T 40700 39400 5 10 0 0 0 0 1
Title=0.1
}
C 41700 39400 1 90 0 capacitor-1.sym
{
T 41000 39600 5 10 0 0 90 0 1
device=CAPACITOR
T 40800 39900 5 10 1 1 0 0 1
refdes=C7
T 40800 39700 5 10 1 1 0 0 1
value=0.1uF
T 40800 39500 5 10 1 1 0 0 1
footprint=0402
T 41700 39400 5 10 0 0 0 0 1
Title=0.1
}
C 42700 39400 1 90 0 capacitor-1.sym
{
T 42000 39600 5 10 0 0 90 0 1
device=CAPACITOR
T 41800 39900 5 10 1 1 0 0 1
refdes=C8
T 41800 39700 5 10 1 1 0 0 1
value=0.1uF
T 41800 39500 5 10 1 1 0 0 1
footprint=0402
T 42700 39400 5 10 0 0 0 0 1
Title=0.1
}
C 32200 39800 1 0 0 generic-power.sym
{
T 32400 40050 5 10 1 1 0 3 1
net=VUSB:1
}
C 29700 37000 1 90 0 capacitor-1.sym
{
T 29000 37200 5 10 0 0 90 0 1
device=CAPACITOR
T 28900 37400 5 10 1 1 0 0 1
refdes=C9
T 28900 37200 5 10 1 1 0 0 1
value=0.01uF
T 28900 37000 5 10 1 1 0 0 1
footprint=0402
T 29700 37000 5 10 0 0 0 0 1
Title=0.1
}
C 29400 36700 1 0 0 gnd-1.sym
C 29300 38900 1 0 0 generic-power.sym
{
T 29500 39150 5 10 1 1 0 3 1
net=VUSB:1
}
C 41000 37800 1 0 0 generic-power.sym
{
T 41200 38050 5 10 1 1 0 3 1
net=5VB:1
}
N 41200 37800 41200 37600 4
N 41200 37600 41400 37600 4
N 32400 38800 32400 37300 4
C 30000 37400 1 0 0 rclamp0502a-1.sym
{
T 30400 39300 5 10 1 1 0 3 1
refdes=U2
T 30900 38700 5 10 1 1 0 3 1
device=RClamp0502A
T 31400 39300 5 10 1 1 0 3 1
footprint=sot666
T 29800 40300 5 10 1 1 0 0 1
partno=RCLAMP0502ACT-ND
}
N 28400 38500 30000 38500 4
N 28600 38100 30000 38100 4
N 31700 38500 33700 38500 4
N 33700 38100 31700 38100 4
C 30800 37100 1 0 0 gnd-1.sym
C 30700 39600 1 0 0 generic-power.sym
{
T 30900 39850 5 10 1 1 0 3 1
net=VUSB:1
}
C 39100 36200 1 0 0 rclamp0502a-1.sym
{
T 39500 38100 5 10 1 1 0 3 1
refdes=U3
T 40000 37500 5 10 1 1 0 3 1
device=RClamp0502A
T 40500 38100 5 10 1 1 0 3 1
footprint=sot666
T 39000 35500 5 10 1 1 0 0 1
partno=RCLAMP0502ACT-ND
}
N 41400 37300 40800 36900 4
N 41400 37000 40800 37300 4
N 39100 36900 37500 37300 4
N 37500 36900 39100 37300 4
C 40100 35900 1 0 1 gnd-1.sym
C 39800 38400 1 0 0 generic-power.sym
{
T 40000 38650 5 10 1 1 0 3 1
net=5VB:1
}
N 38500 39400 43500 39400 4
N 38500 40300 43500 40300 4
C 43700 39400 1 90 0 capacitor-1.sym
{
T 43000 39600 5 10 0 0 90 0 1
device=CAPACITOR
T 42800 39900 5 10 1 1 0 0 1
refdes=C10
T 42800 39700 5 10 1 1 0 0 1
value=4.7uF
T 42800 39500 5 10 1 1 0 0 1
footprint=0402
T 43700 39400 5 10 0 0 0 0 1
Title=0.1
}
T 29300 33900 9 10 1 0 0 0 1
+/- 350ppm
C 30900 33600 1 90 0 crystal-2.sym
{
T 30400 33800 5 10 0 0 90 0 1
device=CRYSTAL
T 31000 34100 5 10 1 1 0 0 1
refdes=Y1
T 30200 33800 5 10 0 0 90 0 1
symversion=0.1
T 31000 33900 5 10 1 1 0 0 1
value=24MHz
T 31000 33700 5 10 1 1 0 0 1
footprint=7v
T 31000 34600 5 10 1 1 0 0 1
partno=887-1465-1-ND
}
N 30900 33600 30900 33000 4
C 30800 32700 1 0 0 gnd-1.sym
C 31000 35100 1 180 0 gnd-1.sym
N 30900 34800 30900 34300 4
C 32600 36400 1 0 0 resistor-1.sym
{
T 32900 36800 5 10 0 0 0 0 1
device=RESISTOR
T 32800 36700 5 10 1 1 0 0 1
refdes=R4
T 33200 36700 5 10 1 1 0 0 1
value=10k
T 32800 36200 5 10 1 1 0 0 1
footprint=0402
}
N 33500 36500 33700 36500 4
N 32600 36500 32400 36500 4
N 33700 36100 33600 36100 4
N 33600 36100 33600 38700 4
N 33600 38700 33300 38700 4
N 33300 38700 33300 38800 4
N 39100 37300 39100 38900 4
N 39100 38900 40800 38900 4
N 40800 38900 40800 37300 4
N 39100 36900 39100 35800 4
N 39100 35800 40800 35800 4
N 40800 35800 40800 36900 4
N 29900 38500 29900 40100 4
N 29900 40100 31800 40100 4
N 31800 40100 31800 38500 4
N 31800 38100 31800 37000 4
N 29900 37000 31800 37000 4
N 29900 37000 29900 38100 4