How to add a new texture
========================
1) Preconditions:
 - Sorry that this is not very configurable yet (i did not have time for that)
 - Install Netpbm Tools under the folder C:\bin\NetPBM-10.68\ (or fix all the references in the batch files)

2) Modify run.cmd:
 - Add a new parameter to genpalette command ("test.bmp") 
 - Add a line for generating the file ("call ./genhdr.cmd test")
 
3) Generate graphics
 - Start an admininstrator command line and give a command: "run_out.cmd"
 - Check that there are no errors in the out.txt file 

