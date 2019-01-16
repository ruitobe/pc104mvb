onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /top_cpld_vlg_tst/arm_addr
add wave -noupdate /top_cpld_vlg_tst/arm_cs
add wave -noupdate /top_cpld_vlg_tst/treg_arm_data
add wave -noupdate /top_cpld_vlg_tst/arm_rd
add wave -noupdate /top_cpld_vlg_tst/arm_wr
add wave -noupdate /top_cpld_vlg_tst/clk
add wave -noupdate /top_cpld_vlg_tst/io_chrdy
add wave -noupdate -divider {New Divider}
add wave -noupdate /top_cpld_vlg_tst/arm_clk
add wave -noupdate /top_cpld_vlg_tst/treg_isa_data
add wave -noupdate /top_cpld_vlg_tst/rst_n
add wave -noupdate /top_cpld_vlg_tst/bale_edge
add wave -noupdate /top_cpld_vlg_tst/bale_flag
add wave -noupdate /top_cpld_vlg_tst/test_status
add wave -noupdate /top_cpld_vlg_tst/arm_data
add wave -noupdate /top_cpld_vlg_tst/arm_wait
add wave -noupdate /top_cpld_vlg_tst/bale
add wave -noupdate /top_cpld_vlg_tst/isa_addr
add wave -noupdate /top_cpld_vlg_tst/isa_clk
add wave -noupdate /top_cpld_vlg_tst/isa_data
add wave -noupdate /top_cpld_vlg_tst/mem_r
add wave -noupdate /top_cpld_vlg_tst/mem_w
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {3165535 ps} 0}
configure wave -namecolwidth 231
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {64 ps}
