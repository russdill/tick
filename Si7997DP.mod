*Aug 02, 2010
*Doc. ID: 66927, ECN S10-1637, Rev. A
*File Name: Si7997DP_PS.txt and Si7997DP_PS.lib
*This document is intended as a SPICE modeling guideline and does not
*constitute a commercial product data sheet. Designers should refer to the
*appropriate datasheet of the same number for guaranteed specification
*limits.
.SUBCKT Si7997DP D G S 
M1 3 GX S S PMOS W= 10697500u  L= 0.25u 
M2 S GX S D NMOS W= 10697500u  L= 2.248e-07 
R1 D 3 2.921e-03 TC=2.708e-03 5.640e-06 
CGS GX S 4.093e-09 
CGD GX D 2.911e-10 
RG G GY 4.6 
RTCV 100 S 1e6 TC=-1.832e-04 1.242e-06 
ETCV GY GX 100 200 1 
ITCV S 100 1u 
VTCV 200 S 1 
DBD D S DBD 
**************************************************************** 
.MODEL PMOS PMOS ( LEVEL = 3 TOX = 5e-8 
+ RS = 5.000e-04 KP = 3.469e-06 NSUB = 2.41e+16 
+ KAPPA = 9.190e-03 ETA = 7.107e-06 NFS = 7.889e+11 
+ LD = 0 IS = 0 TPG = -1) 
*************************************************************** 
.MODEL NMOS NMOS ( LEVEL = 3 TOX = 5e-8 
+NSUB = 2.101e+16 IS = 0 TPG = -1 ) 
**************************************************************** 
.MODEL DBD D ( 
+FC = 0.1 TT = 1.772e-08 T_MEASURED = 25 BV = 31 
+RS = 1.930e-03 N = 1.299e+00 IS = 1.206e-09 
+EG = 1.173e+00 XTI = 5.590e-01 TRS1 = 1.597e-03 
+CJO = 6.258e-10 VJ = 8.996e-01 M = 2.000e-01 ) 
.ENDS 
