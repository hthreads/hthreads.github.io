################################################################
# EECS 41104/51104 Embedded Systems
# Import base_soc design and add constraints
# Supports Vivado 2024.1 and 2025.1
# 
# Author: Tendayi Kamucheka (ftendayi@gmail.com)
# Date: 7 November 2025
#
################################################################

# Get the Vivado version
set vivado_version [version -short]
puts "Detected Vivado version: $vivado_version"

# Get script directory
set script_dir [file dirname [file normalize [info script]]]
puts "Script directory: $script_dir"

# Determine which TCL file to import based on version
if { [string match "2024.1*" $vivado_version] } {
    set base_soc_tcl "${script_dir}/base_soc_2024.1.tcl"
} elseif { [string match "2025.1*" $vivado_version] } {
    set base_soc_tcl "${script_dir}/base_soc_2025.1.tcl"
} else {
    puts "WARNING: Unsupported Vivado version $vivado_version"
    puts "Attempting to use base_soc.tcl if it exists..."
    set base_soc_tcl "${script_dir}/base_soc.tcl"
}

# Check if the TCL file exists
if { ![file exists $base_soc_tcl] } {
    puts "ERROR: File not found: $base_soc_tcl"
    puts "Please ensure the version-specific base_soc TCL file exists."
    return 1
}

# Import the base_soc design
puts "Importing base_soc design from: $base_soc_tcl"
source $base_soc_tcl

# Add constraint file
set constraint_file "${script_dir}/ES2025.srcs/constrs_1/new/base_soc.xdc"

if { [file exists $constraint_file] } {
    puts "Adding constraint file: $constraint_file"
    
    # Check if constraint file is already in project
    set existing_files [get_files -quiet $constraint_file]
    
    if { $existing_files eq "" } {
        # Add the constraint file to the project
        add_files -fileset constrs_1 -norecurse $constraint_file
        puts "Constraint file added successfully"
    } else {
        puts "Constraint file already exists in project"
    }
    
    # Enable the constraint file for synthesis and implementation
    set_property USED_IN_SYNTHESIS true [get_files $constraint_file]
    set_property USED_IN_IMPLEMENTATION true [get_files $constraint_file]
    puts "Constraint file enabled for synthesis and implementation"
} else {
    puts "WARNING: Constraint file not found: $constraint_file"
}

puts "Import completed successfully"
