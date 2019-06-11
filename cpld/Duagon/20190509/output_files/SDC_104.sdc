## Generated SDC file "D:/Project/PC104/ETree_C02_Board/code/CPLD_Only/isa/isa_bus/output_files/top_cpld.sdc"

## Copyright (C) 1991-2013 Altera Corporation
## Your use of Altera Corporation's design tools, logic functions 
## and other software and tools, and its AMPP partner logic 
## functions, and any output files from any of the foregoing 
## (including device programming or simulation files), and any 
## associated documentation or information are expressly subject 
## to the terms and conditions of the Altera Program License 
## Subscription Agreement, Altera MegaCore Function License 
## Agreement, or other applicable license agreement, including, 
## without limitation, that your use is for the sole purpose of 
## programming logic devices manufactured by Altera and sold by 
## Altera or its authorized distributors.  Please refer to the 
## applicable agreement for further details.


## VENDOR  "Altera"
## PROGRAM "Quartus II"
## VERSION "Version 13.1.0 Build 162 10/23/2013 SJ Full Version"

## DATE    "Sat Nov 10 20:49:42 2018"

##
## DEVICE  "EPM240T100C5"
##


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************
create_clock -name {clk} -period 20.000 -waveform { 0.000 10.000 } [get_ports {clk}]


#**************************************************************
# Create Generated Clock
#**************************************************************

create_generated_clock -name CLK_DIV_2 -source [get_ports {clk}] -divide_by 2 [get_registers {clk_div_2}]
create_generated_clock -name CLK_DIV_4 -source [get_registers {clk_div_2}] -divide_by 2 [get_registers {clk_div_4}]
#create_generated_clock -name CLK_DIV_8 -source [get_registers {clk_div_4}] -divide_by 2 [get_registers {clk_div_8}]
#create_generated_clock -name CLK_DIV_16 -source [get_registers {clk_div_8}] -divide_by 2 [get_registers {clk_div_16}]
#create_generated_clock -name CLK_DIV_32 -source [get_registers {clk_div_16}] -divide_by 2 [get_registers {clk_div_32}]
#create_generated_clock -name CLK_DIV_64 -source [get_registers {clk_div_32}] -divide_by 2 [get_registers {clk_div_64}]

#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************



#**************************************************************
# Set Input Delay
#**************************************************************



#**************************************************************
# Set Output Delay
#**************************************************************



#**************************************************************
# Set Clock Groups
#**************************************************************



#**************************************************************
# Set False Path
#**************************************************************



#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

