"""
This script rewrites the filelist.rst file autogenerated by breathe to organize the files in the documentation's API into the same directory structure as their counterparts in the ../Source directory.

Note that we assume here that there is only one level of subdirectories in the ../Source directory and that all the files we want to document are in those subdirectories (i.e. not in subdirectories of those). If this is not the case, then you may want to use os.walk(..) rather than os.listdir(..).

We are also not going to include cpp files - doxygen assumes that functions are documented in the header files, so that's what we'll assume here as well.
"""

import os
import re

# directory of the source files
rootdir = "../../Src"

outfile_path = "source/filelist.rst"

with open(outfile_path, 'w') as outfile:

    output_data = """File list
=========

.. toctree::
   :maxdepth: 2

   """
    for subdir in sorted(os.listdir(rootdir)):
        if not os.path.isdir(os.path.join(rootdir, subdir)):
            continue

        output_data += """{}_files
   """.format(subdir)
        subdir_file_name = "source/{}_files.rst".format(subdir)

        with open(subdir_file_name, 'w') as subdir_file:

            subdir_output_data = "{}\n".format(subdir.capitalize())
            subdir_output_data += "=" * len(subdir)
            subdir_output_data += """

.. toctree::
   :maxdepth: 2

   """

            for f in sorted(os.listdir(os.path.join(rootdir, subdir))):
                # do nothing if the file is not a fortran/header file
                if (f[-4:] != ".F90" and f[-4:] != ".f90" and f[-2:] != ".H" or f[-3:] == "F.H"):
                    continue

                rst_name = re.sub("_", "__", f)
                rst_name = re.sub("\.", "_8", rst_name)

                subdir_output_data += """file/{}
   """.format(rst_name)

            subdir_file.write(subdir_output_data)

    outfile.write(output_data)
