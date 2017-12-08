# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Dropbox\Projects\Pioneer_Kit_Shield\Firmware\Environmental_Sensor\Environmental_Sensor.cydsn\Environmental_Sensor.cyprj
# Date: Tue, 05 Dec 2017 22:33:32 GMT
#set_units -time ns
create_clock -name {UART_SCBCLK(FFB)} -period 708.33333333333326 -waveform {0 354.166666666667} [list [get_pins {ClockBlock/ff_div_1}]]
create_clock -name {I2C_SCBCLK(FFB)} -period 125 -waveform {0 62.5} [list [get_pins {ClockBlock/ff_div_2}]]
create_clock -name {CyRouted1} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/dsi_in_0}]]
create_clock -name {CyWCO} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/wco}]]
create_clock -name {CyLFCLK} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyECO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/eco}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyHFCLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/hfclk}]]
create_clock -name {CySYSCLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/sysclk}]]
create_generated_clock -name {UART_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 17 35} -nominal_period 708.33333333333326 [list]
create_generated_clock -name {I2C_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 3 7} [list]


# Component constraints for C:\Dropbox\Projects\Pioneer_Kit_Shield\Firmware\Environmental_Sensor\Environmental_Sensor.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Dropbox\Projects\Pioneer_Kit_Shield\Firmware\Environmental_Sensor\Environmental_Sensor.cydsn\Environmental_Sensor.cyprj
# Date: Tue, 05 Dec 2017 22:33:31 GMT
