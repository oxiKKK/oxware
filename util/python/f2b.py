#
# f2b.py or "File to Bytes" is a tool used to dump bytes stored inside of a file to a byte buffer.
#
# Copyright oxiKKK (c) 2023
#
# Version 1.1
#
# 1.1 Changes:
#   - removed variable declaration options.
#

import argparse
import os
import math


parser = argparse.ArgumentParser(
    description="File2Bytes -- manages file character encoding."
)

# input file argument
parser.add_argument("input", type=str, help="file to be processed")

# language argument
parser.add_argument(
    "-l",
    "--lang",
    required=True,
    type=str,
    choices={
        "C++": "outputs bytes to a C++ array",
        "C": "outputs bytes to a C array",
    },
    metavar="LANG",
    help="output lang",
)

# create extension depending on the language used
lang_to_extension = {"C++": ".cpp", "C": ".c"}

# style argument
# style is reliant on lang argument.
parser.add_argument(
    "-s",
    "--style",
    required=True,
    type=str,
    choices={
        "uint8": "resulting array will be a type of unsigned char",
        "uint16": "resulting array will be a type of unsigned short",
        "uint32": "resulting array will be a type of unsigned long",
        "uint64": "resulting array will be a type of unsigned long long",
    },
    metavar="STYLE",
    help="style of the resulting array",
)

# custom extension argument
parser.add_argument(
    "-e", "--extension", type=str, help="custom extension for the output file"
)

# variable name that will contain the byte data
parser.add_argument(
    "-n", "--name", type=str, help="variable name that will contain the byte data"
)

# number of rows or data that will be inside the array
parser.add_argument(
    "-r", "--rows", type=int, default=10, help="number of rows or data that will be inside the array"
)

# parse arguments
args = parser.parse_args()

# global data
input_file = ""
input_file_size = 0
output_filename = ""
input_file_bytes = bytes()


def convert_input_buffer_to_result_array_data():
    """
    converts input buffer to such format depending on the datatype
    returns array of strings of raw data by datatype
    """

    result = []
    hex_bytes = input_file_bytes.hex()

    if args.style == "uint8":  # unsigned char
        result = [
            f"0x{hex_bytes[i : i + 2]:>02}" for i in range(0, len(hex_bytes), 2)]
    elif args.style == "uint16":  # unsigned short
        result = [
            f"0x{hex_bytes[i : i + 4]:>04}" for i in range(0, len(hex_bytes), 4)]
    elif args.style == "uint32":  # unsigned long
        result = [
            f"0x{hex_bytes[i : i + 8]:>08}" for i in range(0, len(hex_bytes), 8)]
    elif args.style == "uint64":  # unsigned long long
        result = [
            f"0x{hex_bytes[i : i + 16]:>016}" for i in range(0, len(hex_bytes), 16)]

    return result


def get_total_bytes_size_by_datatype():
    """returns total size of the array depending on the data type selected by 'style' option"""

    size_by_datatype = int(input_file_size)

    # for style
    if args.style == "uint8":  # unsigned char
        size_by_datatype /= 1
        size_by_datatype = math.floor(size_by_datatype)
    elif args.style == "uint16":  # unsigned short
        size_by_datatype /= 2
        size_by_datatype = math.floor(size_by_datatype) + (
            1 if int(input_file_size) % 2 != 0 else 0
        )
    elif args.style == "uint32":  # unsigned long
        size_by_datatype /= 4
        size_by_datatype = math.floor(size_by_datatype) + (
            1 if int(input_file_size) % 4 != 0 else 0
        )
    elif args.style == "uint64":  # unsigned long long
        size_by_datatype /= 8
        size_by_datatype = math.floor(size_by_datatype) + (
            1 if int(input_file_size) % 8 != 0 else 0
        )
    return int(size_by_datatype)


def get_variable_names():
    """returns names of the variables that will contain the byte data and other information"""

    names = {
        "size_raw": "",  # raw size of the buffer
        "size": "",  # size depending on the datatype
        "data": "",  # raw data
    }

    if args.name != None:
        names["size_raw"] = args.name + "_size_raw"
        names["size"] = args.name + "_size"
        names["data"] = args.name
    else:
        names["size_raw"] = "g_output_bytes" + "_size_raw"
        names["size"] = "g_output_bytes" + "_size"
        names["data"] = "g_output_bytes"

    return names


def get_type_by_lang_and_style():
    """returns language-specific datatype based on style and lang settings"""

    datatype = ""

    # for style
    if args.style == "uint8":  # unsigned char
        datatype = "unsigned char"
    elif args.style == "uint16":  # unsigned short
        datatype = "unsigned short"
    elif args.style == "uint32":  # unsigned long
        datatype = "unsigned long"
    elif args.style == "uint64":  # unsigned long long
        datatype = "unsigned long long"

    return datatype


def get_output_file(a):
    """
    appends '_out' string to the input file name, such as 'img_out.extension'.
    the extension depends on the lang. (e.g. cpp, c, etc.)
    """

    path, ext = os.path.splitext(input_file)
    path += "_out"

    # see if user has explicitly selected some extension. if not, create automatically.
    # if the custom extension doesn't contain '.' in the beginning, append automatically
    # if the extension consists only from '.' and nothing else, forward to automatic method.
    if args.extension != None and len(args.extension) != 0 and args.extension != ".":
        path += "." + \
            args.extension if args.extension[0] != "." else args.extension
    else:
        path += lang_to_extension[args.lang]

    return path


def read_input_file():
    try:
        with open(input_file, "rb") as file:
            global input_file_bytes
            input_file_bytes = file.read()

        global input_file_size
        input_file_size = len(input_file_bytes)

        print(f"read {input_file_size} bytes of data.")
        return True

    except OSError as e:
        print(f"error while opening the input file: {input_file}\n  {e}")

        return False


def process_input():
    """processes desired input file"""

    # get output filename
    global output_filename
    output_filename = get_output_file(input_file)

    print(f"output file is: {output_filename}")

    # read and store contents of input file
    if not read_input_file():
        return False

    return True


def create_output_file():
    """creates output file for write"""

    return open(output_filename, "w")


def write_raw_byte_array_data(output_file_handle):
    """writes the raw byte array data depending on the datatype for current style"""

    n_data = 0

    result_array_data = convert_input_buffer_to_result_array_data()

    for data in result_array_data:
        is_last = n_data == len(result_array_data) - 1
        output_file_handle.write(data + (", " if not is_last else ""))

        if (n_data != 0 or args.rows == 1) and (n_data + 1) % args.rows == 0:
            output_file_handle.write("\n\t")

        n_data += 1

    output_file_handle.write("\n")


def write_bytearray(output_file_handle):
    """write resulting C++ or other language array to the file"""

    datatype = get_type_by_lang_and_style()
    variable_names = get_variable_names()
    size_by_datatype = get_total_bytes_size_by_datatype()

    """
    size_raw = X;
    size = Y;
    data[size] = 
    {
        Z
    };
    """

    output_file_handle.write("\n")
    output_file_handle.write(
        f"unsigned int {variable_names['size_raw']} = {input_file_size};\n"
    )
    output_file_handle.write(
        f"unsigned int {variable_names['size']} = {size_by_datatype};\n"
    )
    output_file_handle.write(
        f"{datatype} {variable_names['data']}[{size_by_datatype}] = \n"
    )
    output_file_handle.write("{\n\t")

    write_raw_byte_array_data(output_file_handle)

    output_file_handle.write("};\n")


def process_output():
    """creates and writes to the output file"""

    # create output file
    output_file_handle = None
    try:
        output_file_handle = create_output_file()

    except OSError as e:
        print(f"error while opening the output file: {input_file}\n  {e}")

        return

    print("created output file.")

    output_file_handle.write(
        f"""//
// File '{output_filename}' automatically generated by 'f2b.py' script
// Processed from file '{input_file}' of size {input_file_size:,} bytes
//
"""
    )

    write_bytearray(output_file_handle)

    output_file_handle.close()

    pass


def main():
    """program entry point"""

    global input_file
    input_file = args.input

    if not process_input():
        return

    process_output()

    print("done!")


if __name__ == "__main__":
    main()
