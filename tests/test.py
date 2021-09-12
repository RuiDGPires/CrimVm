#!/usr/bin/env python3
# SCRIPT TO TEST THE PROJECT

from glob import glob
import subprocess
import tempfile 
import os
import argparse
import sys
import datetime
from pathlib import Path

HEADER = '\033[95m'
OKBLUE = '\033[94m'
OKCYAN = '\033[96m'
OKGREEN = '\033[92m'
WARNING = '\033[93m'
FAIL = '\033[91m'
ENDC = '\033[0m'
BOLD = '\033[1m'
UNDERLINE = '\033[4m'

dir_path = dir_path = os.path.dirname(os.path.realpath(__file__))

EXE = f"{dir_path}/../a.out"
INPUT_FILES = glob(f"{dir_path}/inputs/*")

def printError(string):
    print(FAIL + BOLD + string + ENDC)

def printSucess(string):
    print(OKGREEN + BOLD + string + ENDC)

print(BOLD, HEADER)
print("****************")
print("TESTING CRIMVM ")
print("****************")
print(ENDC)

ALL_TESTS_PASSED = True
for file in INPUT_FILES:
    # GET FILE NAME
    filename = os.path.basename(file)
    filename_no_ext = os.path.splitext(filename)[0] 
    out_file = f"{dir_path}/outputs/{filename_no_ext}.out"

    # EXECUTE PROGRAM
    result = subprocess.run([EXE, "-ar", file], stdout = subprocess.PIPE )
    ret_code = result.returncode 

    success = ret_code == 0 

    passed = False 

    if success:
        with open(out_file, "r") as f:
            file_text = f.read();
        passed  = file_text == result.stdout.decode('ascii')

    if not passed:
        print(FAIL, "fail\t", ENDC, ":\t", filename)
        ALL_TESTS_PASSED = False
    else:
        print(OKGREEN, "ok\t", ENDC, ":\t", filename) 

if ALL_TESTS_PASSED:
    printSucess("\nALL TESTS PASSED!")
else:
    printError("\nSOME TESTS FAILED")
