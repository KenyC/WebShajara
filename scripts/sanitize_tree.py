#!/usr/bin/python3
import sys
import os

file_path = sys.argv[1]

with open(file_path, "r") as f_in:
	with open(file_path + ".txt", "w") as f_out:
		for line in f_in:
			if line[-1] == '\n':
				line = line[:-1]
			f_out.write(
				'"' +\
				line.replace("\\", r"\\") +\
				'"\n'
			)

