"""
Object-oriented programming project - INFO0004-2
University of Liège
Academic year 2018-2019

This program is used to automatically test the 'painter' program
based on the reference files provided.

@author		Maxime Meurisse
@version	2019.05.13
"""

# Imports

import os
import subprocess
import platform

from os import listdir
from os.path import isfile, join

# Data

SYSTEM_OS = platform.system()

if SYSTEM_OS == "Windows":
	SHELL = "../bin/painter.exe"
else:
	SHELL = "./../bin/painter"

PAINT_PATH = "ref_paint/"
MY_PATH = "my_ppm/"
REF_PATH = "ref_ppm/"

# Functions

def get_filename(file):
	filename, file_extension = os.path.splitext(file)

	return filename

def get_extension(file):
	filename, file_extension = os.path.splitext(file)

	return file_extension

def filter_extension(file, ext):
	if get_extension(file) == ext:
		return True
	else:
		return False

# Main

if __name__ == "__main__":
	separator = "-------------------------"

	## Paint file

	print(separator)
	print("Searching for PAINT files")

	paint_files = [f for f in listdir(PAINT_PATH) if isfile(join(PAINT_PATH, f))]
	paint_files.sort()
	paint_files_filtered = [f for f in paint_files if filter_extension(f, ".paint")]

	if len(paint_files_filtered) == 0:
		print("No PAINT file found")
		print(separator)
	else:
		print(str(len(paint_files_filtered)) + " PAINT file(s) found")
		print("All PAINT files will be converted to PPM files with your 'painter' program (OS : " + SYSTEM_OS + ")")
		print(separator)

		for paint_file in paint_files_filtered:
			print("Creating PPM file from " + paint_file + "... ", end = '')
			subprocess.run(SHELL + " " + PAINT_PATH + paint_file, shell = True, stdout = subprocess.PIPE, stdin = subprocess.PIPE)
			os.rename(PAINT_PATH + get_filename(paint_file) + ".ppm", MY_PATH + get_filename(paint_file) + ".ppm")
			print("DONE")

	## PPM files

	print(separator)
	print("Searching for created PPM files")

	my_ppm = [f for f in listdir(MY_PATH) if isfile(join(MY_PATH, f))]
	ref_ppm = [f for f in listdir(REF_PATH) if isfile(join(REF_PATH, f))]

	my_ppm.sort()
	my_ppm_filtered = [f for f in my_ppm if filter_extension(f, ".ppm")]

	nb_diff = 0

	if len(my_ppm_filtered) == 0:
		print("No PPM files found")
		print(separator)
	else:
		print(str(len(my_ppm_filtered)) + " PPM file(s) found")
		print("PPM file(s) will be compared with reference PPMs")
		print(separator)

		for ppm_file in my_ppm:
			log = ppm_file + "... "

			if ppm_file in ref_ppm:
				f = open(MY_PATH + ppm_file, "rb")
				my_content = f.read()
				f.close()

				f = open(REF_PATH + ppm_file, "rb")
				ref_content = f.read()
				f.close()

				my_content = my_content.replace(b'\r\n', b'\n')

				if my_content == ref_content:
					print(log + "OK")
				else:
					nb_diff += 1
					print(log + "NOT OK")
			else:
				print(log + "NOT FOUND IN " + REF_PATH + " FOLDER")

		print(separator)

		log = "Comparison finished : "

		if nb_diff == 0:
			log += "all files found in " + REF_PATH + " folder are identical, nice job !"
		else:
			log += "you have " + str(nb_diff) + " different file(s).\n"
			log += "Don't panic ! Some differences are simply due to inaccuracies because of operations on double.\n"
			log += "These differences are not penalizing."

		print(log)
		print(separator)
