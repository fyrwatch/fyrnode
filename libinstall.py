"""
===========================================================================
Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.

This file is part of the FyrNode library.
No part of the FyrNode library can not be copied and/or distributed 
without the express permission of Manish Meganathan and Mariyam A.Ghani
===========================================================================
A python script to install the fyrnode library at the provided target location.
===========================================================================
"""

import os
import sys
from distutils.dir_util import copy_tree

script_dir = os.path.dirname(os.path.realpath(__file__))
source_dir = os.path.join(script_dir, "..", "fyrnode")

if __name__ == "__main__":
    target_dir = input("Enter target directory to install the library:\n")

    if not os.path.isdir(target_dir):
        ("invalid target directory. try again!")
        sys.exit()

    if not target_dir.endswith("fyrnode"):
        target_dir = os.path.join(target_dir, "fyrnode")

    try:
        copy_tree(source_dir, target_dir)
    except:
        print("library install failed!")
        sys.exit()

    print(f"library install successful at {target_dir}")
