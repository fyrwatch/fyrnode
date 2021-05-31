"""
===========================================================================
MIT License

Copyright (c) 2021 Manish Meganathan, Mariyam A.Ghani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===========================================================================
A python script to install the fyrnode library at the provided target location.
===========================================================================
"""
import os
import sys
from distutils.dir_util import copy_tree

arguments = sys.argv[1:]
script_dir = os.path.dirname(os.path.realpath(__file__))
source_dir = os.path.join(script_dir, "fyrnode")

print("[INFO] FyrNode installation is starting.")

if not arguments:
    print("[ERROR] target directory for installation was not entered.")
    sys.exit()

target_dir = arguments[0]

if not os.path.isdir(target_dir):
    ("[ERROR] target directory entered for installation is invalid. try again!")
    sys.exit()

if not target_dir.endswith("fyrnode"):
    target_dir = os.path.join(target_dir, "fyrnode")

try:
    copy_tree(source_dir, target_dir)

except:
    print("[FAIL] FyrNode library install failed!")
    sys.exit()

print(f"[SUCCESS] FyrNode library successfully installed at {target_dir}")
