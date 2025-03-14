DATAPOINTS BDRVR CDSDefaultInput_1p8v_Typ_27degC
MODEL_VERSION=2.0
PowerClampReference=1.8
GroundClampReference=0

DATAPOINTS AC_PARAM
dVGateOpen=1
dVGateClose=0
ENABLE_ON_VOLT=1
ENABLE_OFF_VOLT=0
RISE_TIME=-1
FALL_TIME=-1
dVdtr=-1
dVdtf=-1
END AC_PARAM

DATAPOINTS LOGIC_PARAM
VHI_MIN=1.25
VLO_MAX=0.5
END LOGIC_PARAM

DATAPOINTS VI POWER_CLAMP
0 0
0.1 0
0.4 0.0001
0.5 0.0006
0.6 0.0012
0.7 0.0024
0.8 0.006
0.9 0.013
1 0.025
1.8 0.079
END VI POWER_CLAMP

DATAPOINTS VI ground_clamp
0 0
-0.1 0
-0.4 -0.0001
-0.5 -0.0005
-0.6 -0.0012
-0.7 -0.0024
-0.8 -0.006
-0.9 -0.013
-1 -0.025
-1.8 -0.079
END VI ground_clamp

DATAPOINTS PARASITICS
C_COMP=2e-12
END PARASITICS

END BDRVR CDSDefaultInput_1p8v_Typ_27degC
DATAPOINTS BDRVR CDSDefaultOutput_1p8v_Typ_27degC
MODEL_VERSION=2.0
PullUpReference=1.8
PullDownReference=0
PowerClampReference=1.8
GroundClampReference=0

DATAPOINTS AC_PARAM
dVGateOpen=1
dVGateClose=0
ENABLE_ON_VOLT=1
ENABLE_OFF_VOLT=0
RISE_TIME=3.6e-10
FALL_TIME=3.6e-10
dVdtr=5e+09
dVdtf=5e+09
END AC_PARAM

DATAPOINTS VI POWER_CLAMP
0 0
0.1 0
0.4 0.0001
0.5 0.0006
0.6 0.0012
0.7 0.0024
0.8 0.006
0.9 0.013
1 0.025
1.8 0.079
END VI POWER_CLAMP

DATAPOINTS VI ground_clamp
0 0
-0.1 0
-0.4 -0.0001
-0.5 -0.0005
-0.6 -0.0012
-0.7 -0.0024
-0.8 -0.006
-0.9 -0.013
-1 -0.025
-1.8 -0.079
END VI ground_clamp

DATAPOINTS VI PULL_UP
-3.6 -0.124
-3.5 -0.123
-3 -0.118
-2.5 -0.11
-2 -0.098
-1.5 -0.083
-1 -0.064
-0.5 -0.038
0 0
1 0.064
1.8 0.09
END VI PULL_UP

DATAPOINTS VI pull_down
-2 -0.188
-1 -0.07
0 0
0.5 0.07
1 0.127
1.5 0.164
2 0.188
2.5 0.203
3 0.207
3.6 0.21
END VI pull_down

DATAPOINTS PARASITICS
C_COMP=2e-12
END PARASITICS

END BDRVR CDSDefaultOutput_1p8v_Typ_27degC
