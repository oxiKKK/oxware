# A simple script to count lines of code inside source and header files.
#
# Copyright oxiKKK 2023 (c)
#
# Version: 1.0
#

import sys
import os
import datetime
from pathlib import Path

# script version
version = "1.0"

# initialize to current script directory
script_dir = os.path.dirname(__file__)

# directory to perform the search in
final_directory = ""

# list of extensions to search for
extensions = ('.cxx', '.cpp', '.c', '.h', '.hpp')

# add directory names here to not take place in counting
dir_filter = (
    'external', 
    'src\\assets'
)

# settings and their explanation
settings = {
    '-v': "enables verbose mode", 
    '-f': "dumps to a file elegantly",
    '-m': "Automatically modifies README.md and Menu.cpp"
}

def print_settings():
    print("\nSettings:")

    for s in settings.items():
        print(s)
    print("")

# settings
verbs           = False
dump_to_file    = False
modify_files   = False

# sum of all lines
total_lines = 0

# passed in arguments
arguments = sys.argv

# dump file creation
created_file = False
dump_filename = ""
dumped_file = None # the file handle we'll dump to

# file print
def fprint(s):
    if dump_to_file:
        dumped_file.write(s)
        dumped_file.write('\n')
        print(s)
    else:
        print(s)

# create file and write header
def create_dump_file() -> bool:
    print("creating dump file...")

    # create dump\ directory if not exists
    Path(r".\dumps").mkdir(parents=True, exist_ok=True)

    global dump_filename
    timestamp = str(datetime.datetime.now())[:10].replace(' ', '_')
    dump_filename = r"dumps\lines_{}.txt".format(timestamp)
    try:
        global dumped_file
        dumped_file = open(dump_filename, "w")
    except OSError as e:
        print(f"OSError: {e}")
        return False
    
    fprint(
f"""//
// File generated automatically by cntlines.py script
// Generated at: {timestamp}.
// Version: {version}
//
""")

    return True

# verbose print
def verbose(s):
    if verbs:
        print(s)

# check settings passed in
def parse_settings():
    # parse directory
    global final_directory
    final_directory = os.path.join(script_dir, arguments[1])
    print(f"applying to '{final_directory}'")

    if '-v' in arguments:
        global verbs
        verbs = True

    if '-f' in arguments:
        global dump_to_file
        dump_to_file = True

    if '-m' in arguments:
        global modify_files
        modify_files = True

# check parameters
def check_params() -> bool:
    if len(arguments) <= 1:
        print(f"error: invalid arguments: {arguments}")
        print_settings()
        return False
    else:
        parse_settings()
        return True

# automatically modify README.md with linecount
def modify_readme_fn():
    try:
        file = open(r"..\..\README.md", "r", encoding="utf8")
    except OSError as e:
        print(f"OSError: {e}")
        sys.exit()
    
    contents = file.read()

    token = ", featuring **"
    ending_token = "\n\nThis cheat is still in"
    pos = contents.find(token)
    ending_pos = contents.find(ending_token, pos)

    to_replace_with = ", featuring **{:,} lines of C++ code in {:,} source files!**".format(count_data.total_lines, len(count_data.files))

    final_contents = contents[:pos] + to_replace_with + contents[ending_pos:]
    file.close()

    # reopen the file and write to it
    file = open(r"..\..\README.md", "w", encoding="utf8")
    file.write(final_contents)

    print("wrote new README.md!")

# automatically modify Menu.cpp linecount and filecount with up to date data
def modify_menu_cpp_fn():
    try:
        file = open(r"..\..\src\cheat\ui\rendering_contexts\menu\Menu.cpp", "r", encoding="utf8")
    except OSError as e:
        print(f"OSError: {e}")
        sys.exit()
    
    contents = file.read()

    token = r'/*LINES&FILESMARKER*/"{} lines in {} files", '
    ending_token = ");\n"
    pos = contents.find(token)
    ending_pos = contents.find(ending_token, pos)

    to_replace_with = '/*LINES&FILESMARKER*/"{{}} lines in {{}} files", "{:,}", "{:,}"'.format(count_data.total_lines, len(count_data.files))

    final_contents = contents[:pos] + to_replace_with + contents[ending_pos:]
    file.close()

    # reopen the file and write to it
    file = open(r"..\..\src\cheat\ui\rendering_contexts\menu\Menu.cpp", "w", encoding="utf8")
    file.write(final_contents)

    print("wrote new Menu.cpp!")

# check directory
def check_directory() -> bool:
    if os.path.exists(final_directory):
        return True
    else:
        print(f"error: directory doesn't exist ({final_directory})")
        return False

# object to hold counted data
class CountData:
    class File:
        def __init__(self, p, l):
            self.path = p
            self.lines = l

        path = ""
        lines = 0
    
    total_lines = 0
    files = []

count_data = CountData()

# count lines inside all files within a directory
def count_lines() -> None:
    for subdir, dirs, files in os.walk(final_directory):
        
        # see for blacklisted directories
        filter = False
        for blacklisted in dir_filter:
            if blacklisted in subdir:
                filter = True

        if filter:
            verbose(f"filtered '{subdir}'.")
            continue

        # now iterate over files
        for file in files:

            ext = os.path.splitext(file)[-1].lower()
            if ext in extensions:
                fulldir = os.path.join(subdir, file)

                num_lines = sum(1 for _ in open(fulldir, encoding='utf8'))

                file_data = CountData.File(
                    fulldir, num_lines
                )

                count_data.files.append(file_data)
                count_data.total_lines += num_lines

# print results etc
def process_result():
    # sort the list
    count_data.files.sort(key=lambda x: x.lines, reverse=True)

    for i, f in enumerate(count_data.files):
       verbose(f"{i}: {f.path}: {f.lines} lines")

    fprint("-----------------------------------------")
    fprint(f"total lines: {count_data.total_lines} in {len(count_data.files)} files.")
    fprint("")
    fprint("Top 10 files by line count:")

    for i in range(len(count_data.files) if len(count_data.files) < 10 else 10):
       f = count_data.files[i]
       fprint(f"{i}: {f.path}: {f.lines} lines")

    print("")

#---------------------------------------------------------

def script():
    print("starting...")

    print(f"script directory: {script_dir}")

    if not check_params():
        sys.exit()

    # create dump file
    if dump_to_file:
        if not create_dump_file():
            print(f"error: could not create dump file. ({dump_filename})")
            sys.exit()
        else:
            print(f"dumping to {dump_filename}.")

    if not check_directory():
        sys.exit()

    count_lines()

    process_result()

    if modify_files:
        modify_readme_fn()
        modify_menu_cpp_fn()

    # close the file if opened
    if dump_to_file:
        dumped_file.close()

    print("finish!")
    
script()