# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Dropbox\Projects\Pioneer_Kit_Shield\Firmware\XBee_WiFi_Example\CE216795_DualCoreSharedMemory01.cydsn\CE216795_DualCoreSharedMemory01.cyprj
# Date: Fri, 01 Dec 2017 20:55:21 GMT
#set_units -time ns
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyLFClk} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyFLL} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/fll}]]
create_clock -name {CyHFClk0} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/hfclk_0}]]
create_clock -name {CyFastClk} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/fastclk}]]
create_clock -name {CyPeriClk} -period 20 -waveform {0 10} [list [get_pins {ClockBlock/periclk}]]
create_generated_clock -name {CySlowClk} -source [get_pins {ClockBlock/periclk}] -edges {1 2 3} [list [get_pins {ClockBlock/slowclk}]]
create_clock -name {CyPeriClk_App} -period 20 -waveform {0 10} [list [get_pins {ClockBlock/periclk_App}]]
create_clock -name {CyHFClk1} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/hfclk_1}]]
create_clock -name {CyHFClk2} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/hfclk_2}]]
create_clock -name {CyHFClk3} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/hfclk_3}]]
create_clock -name {CyHFClk4} -period 10 -waveform {0 5} [list [get_pins {ClockBlock/hfclk_4}]]
create_clock -name {CyIMO} -period 125 -waveform {0 62.5} [list [get_pins {ClockBlock/imo}]]


# Component constraints for C:\Dropbox\Projects\Pioneer_Kit_Shield\Firmware\XBee_WiFi_Example\CE216795_DualCoreSharedMemory01.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Dropbox\Projects\Pioneer_Kit_Shield\Firmware\XBee_WiFi_Example\CE216795_DualCoreSharedMemory01.cydsn\CE216795_DualCoreSharedMemory01.cyprj
# Date: Fri, 01 Dec 2017 20:55:20 GMT
