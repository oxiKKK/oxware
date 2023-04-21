# png2cpp.ps1 (c) oxiKKK 2023
#
# Executes a script which runs png2cpp executable. Converts input png file into cpp global variable.
#
# Usage: ./png2cpp.ps1 png_image.png
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

$exe = "png2cpp.exe"

# see if the executable exists
if (!(Test-Path $exe))
{
	Write-Error("The executable '" + $exe + "' isn't available.")
	break
}

Write-Output("Generating a cpp file...")

# execute the command
$output_file = $input_file + "_c.cpp"
$variable_name = "g_" + $input_file
./png2cpp.exe $input_file_full $variable_name $output_file

Write-Output("Successfully wrote the result to '" + $output_file +  "'!")