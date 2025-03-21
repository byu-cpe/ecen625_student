connect -url tcp:127.0.0.1:3121

# puts "Cable targets:\n [targets ]"

# Get a list of targets
set target_list [targets -nocase -filter {name =~ "xc7z020*" && jtag_cable_serial == "$::env(JTAG_SERIAL)"} -target-properties]

if {[llength $target_list] > 1} {
    puts "Error: Multiple matching targets: $target_list (Make sure only one board is plugged in)"
    exit 1
} elseif {[llength $target_list] == 0} {
    puts "Error: No targets.  Make sure board is plugged in and driver is installed."
    exit 1
}

targets -set -nocase -filter {name =~"APU*" && jtag_cable_serial == "$::env(JTAG_SERIAL)"}
rst -system
after 3000

targets -set -index 1 -filter {jtag_cable_serial == "$::env(JTAG_SERIAL)"}
fpga -file $::env(BIT_FILE)

targets -set -nocase -filter {name =~"APU*" && jtag_cable_serial == "$::env(JTAG_SERIAL)"}
loadhw -hw $::env(XSA_FILE) -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1

targets -set -nocase -filter {name =~ "*A9*#0" && jtag_cable_serial == "$::env(JTAG_SERIAL)"}
rst -processor
# dow $::env(FSBL_FILE)
# set bp_32_12_fsbl_bp [bpadd -addr &FsblHandoffJtagExit]
# con -block -timeout 60
# bpremove $bp_32_12_fsbl_bp

targets -set -nocase -filter {name =~"APU*" && jtag_cable_serial == "$::env(JTAG_SERIAL)"}
source $::env(PS7_INIT_FILE)
ps7_init
ps7_post_config

targets -set -nocase -filter {name =~ "*A9*#0" && jtag_cable_serial == "$::env(JTAG_SERIAL)"}
# jtagterminal -start
dow $::env(ELF_FILE)
configparams force-mem-access 0

targets -set -nocase -filter {name =~ "*A9*#0" && jtag_cable_serial == "$::env(JTAG_SERIAL)"}
con
# after 1000
# con

# puts "\033\[95m A 'JTAG-based Hyperterminal' window should now open.  This will display the output of the program. Press 'Ctrl+C' in this window to quit.\033\[0m"

# while { 1 } {

# }