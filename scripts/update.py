#!/usr/bin/env python3

from glob import glob
from subprocess import run

files = glob("src/*")


run(["git", "checkout", "core"])
run(["git", "checkout", "master", "--"] + files)
run(["git", "commit", "-m", "\"Update files from master\""])
run(["git", "push"])
