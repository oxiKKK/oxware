# fompr.ps1 (c) oxiKKK 2023
#
# Simple script to convert font into compressed bytes inside a cpp file.
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

# filename without extension
$input_file = $input.Substring(0, $input.LastIndexOf('.'))

# filename extension
$input_file_ext = $input.Substring($input.LastIndexOf('.'))

# see if the extension is font format
if ($input_file_ext -ne ".ttf")
{
	Write-Error("The extension of the input file must be '.ttf' however, is '" + $input_file_ext + "'.")
	break
}

$compressor_exe = "binary_to_compressed_c.exe"

# see if the compressing executable exists
if (!(Test-Path $compressor_exe))
{
	Write-Error("The compressing executable '" + $compressor_exe + "' isn't available.")
	break
}

Write-Output("Compressing and generating a cpp file...")

# execute the command
$output_file = $input_file + "_c.cpp"
$variable_name = "g_" + $input_file
./binary_to_compressed_c $input_file_full $variable_name > $output_file

Write-Output("Successfully compressed the font to '" + $output_file +  "'!")