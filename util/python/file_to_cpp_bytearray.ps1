# file_to_cpp_bytearray.ps1 (c) oxiKKK 2023
#
# Simple script to convert file data into a C++ byte array.
#

function display_help
{
	$filename = Split-Path $PSCommandPath -leaf
	Write-Output("How to use:")
	Write-Output($filename + " <input file>")
	Write-Output("")
}

# check for number of parameters passed
if ($args.Count -eq 0 -or $args.Count -gt 1)
{
	Write-Error("Invalid arguments passed in.")
	display_help
	break
}

$input = $args[0]

Write-Output("Input file: '" + $input + "'")

# see if input is a string
if ($input.GetType() -ne [string])
{
	Write-Error("Input has to be a string however, it is a '" + $input.GetType().FullName + "'.")
	break
}

# see if the input file exists
if (!(Test-Path $input))
{
	Write-Error("The input file doesn't exist.")
	break
}

# full filename
$input_file_full = $input
$input_filename = (Get-Item $input ).Basename  
$global_var_name = "g_" + $input_filename

Write-Output("Running the script on '" + $input_file_full + "'...")

# execute the command
python f2b.py $input_file_full -l "C++" -s "uint8" -r 32 -n $global_var_name