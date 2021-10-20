# How to install TDD Dev environment (debian/ubuntu)

sudo apt install catch make 

# How to launch a test manually 
 
 
TDD_ROOT_FILE="\<your-directory>/QbGifBuilder/cpluscplus/TDD"  
  
(optional clean-up)  

    [ -d "\${TDD_ROOT_FILE}" ] && rm "\${TDD_ROOT_FILE}"/bin/*  

To build and run a test, type:  

    cd "\${TDD_ROOT_FILE}/makefiles"  
    make -f "tdd-\<my-test>.makefile" "../bin/tdd-&lt;my-test\>.x"  
    [ \$? -eq 0 ] && cd ../bin && ./tdd-&lt;my-test>.x  

### Example to run all tests:
  
    [ -d "\${TDD_ROOT_FILE}" ] && rm "\${TDD_ROOT_FILE}"/bin/*  
    cd "\${TDD_ROOT_FILE}/makefiles"  
    make -f "tdd-all.makefile" "../bin/tdd-all.x"  
    [ \$? -eq 0 ] && cd ../bin && ./tdd-all.x  
  

# How to launch a test from Visual Studio Code  

This section explains how to build and execute a tdd-\<test>.cpp file when pressing [CTRL] + [T]  

### Open the QbGifBuilder workspace  
  
To take advantage of this feature, you must open the workspace from the following file:  

    \<your-directory>/QbGifBuilder/_workspaces/QbGifBuilder_cplusplus.code-workspace  

### Change your visual studio code configuration

Enter the menu File / Preferences / Keyboard shortcuts ( or press [Ctrl}+[K] [Ctrl]+[S])  
Then click on the 'file' icon (upper-right corner)  

Place your key bindings in this file to override the default behavior

    [
        {
            "key": "ctrl+shift+t",
            "command": "workbench.action.tasks.runTask",
            "args": "Test",
        }
    ]

### build and execute a tdd-\<test>.cpp file
  
In visual studio code, open a file from \<your-directory>/QbGifBuilder/cpluscplus/TDD and type [CTRL] + [T]  
  