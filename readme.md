

# haddCondor Overview

 This tool takes an input root file list, then submits a dagman job that iteratively hadds the rootfiles until there is only 1 large root file

# Build instructions
```
make 
make hadd
```

# haddCondor.x input arguments
 - 1= output diretory name
 - 2= path (where .x and source code is located)
 - 3= inputlist.list
 - 4= nCombine (number of input files to hadd per job)

# haddCondor workflow

 - build the two executables
 - run haddCondor.x with proper arguments (like in example.sh)
   - this will create a workspace to store the job output, and house the submit/dag scripts
   - the jobs are separated with nCombine, each job uses N inputfiles <= nCombine
 - go to the generated /dag/ directory for the dag script
 - submit dag with:
```
condor_submit_dag mydag.dg
```
 - the dag will then run the first layer of jobs, produce output rootfiles, then run jobs that hadd the previous layers output as input. This continues until there is only 1 final root file


# Notes/caveats

make sure the list is formatted correct : (no line break at the end of the file)

when inputting path,  make sure there is **not** a / at the end of the path
Wrong: pathto/mypath/		Right: pathto/mypath

nCombine must be >= 2

Currently intermediate hadd rootfiles are not deleted, this can take up alot of space. just delete them manually when the job finishes. Ill probably update this later. exploiting the naming convention, you can delete these like
```rm f_*.root
```

It might be a good idea to add request for more memory in the submission scripts (for large hadds)

