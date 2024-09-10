#!/usr/bin/env python3
#NAME: Roselyn Lee,Chris Baker
#EMAIL: roselyn.lee@gmail.com,cbaker24@g.ucla.edu
#ID:removed for partners privacy

import sys
import re


"""
Global Variables
Sets the dbl or trip string and offset depening on levels
"""
inconsistent = False

alocblk = "ALLOCATED BLOCK "
unrefblk = "UNREFERENCED BLOCK "

dblstr = "DOUBLE INDIRECT"
dbloff = "268"

tripstr = "TRIPLE INDIRECT"
tripoff = "65804"

inv_blk = "INVALID BLOCK "
inv_ind = "INVALID INDIRECT BLOCK "
inv_dind = "INVALID DOUBLE INDIRECT BLOCK "
inv_tind = "INVALID TRIPLE INDIRECT BLOCK "

res_blk = "RESERVED BLOCK "
res_ind = "RESERVED INDIRECT BLOCK "
res_dind = "RESERVED DOUBLE INDIRECT BLOCK "
res_tind = "RESERVED TRIPLE INDIRECT BLOCK "

dup_blk = "DUPLICATE BLOCK "
dup_ind = "DUPLICATE INDIRECT BLOCK "
dup_dind = "DUPLICATE DOUBLE INDIRECT BLOCK "
dup_tind = "DUPLICATE TRIPLE INDIRECT BLOCK "

nstr = " IN INODE "
ostr = " AT OFFSET "

"""
Exit function with value 1
"""
def exitE(var):
    print("Error:" + var, file=sys.stderr)
    exit(1)
    
    
"""
If the program is inconsistent, then we exit with 2
else we exit 0
"""
def verify_con():
    if inconsistent:
        return 2
    return 0
        
"""
Sets the inconsistent value and prints information to console
"""
def set_incon(var):
    print(var)
    global inconsistent
    inconsistent = True


"""
Checks for correct number of args
If Valid args, then opens the argv file with read permission
If exception it exits
"""
def arg_check():
    #len returns an integer value of string, array, or list
    #sys.argv is list of command line args, argv[0] is main script
    #argv[1] is first argument, File object set to report std error
    if len(sys.argv) < 2:
        exitE("Less than 2 args. Correct usage: lab3b [filename]")
    if len(sys.argv) > 2:
        exitE("more than 2 args. Correct usage: lab3b [filename]")
    try:
        fd = open(sys.argv[1], "r")
    except:
        exitE("Enter a valid file for 2nd arg")
    return(fd)


"""
Allocated I nodes have a valid type (file or dir)
Unallocated I nodes have type 0
Every unallocated Inode should be on a free I node List
For each inconsistency display messages below
"""
#Changed the statements to use set_incon
def inode_audit():
    global alloc_inodes
    alloc_inodes = []
    unalloc_inodes = []
    #For every item in inodes list we append for allocated node list
    for inode in inodes:
        alloc_inodes.append(int(inode.split(',')[1]))
    #For every item in inodes free list we append for unallocates node list
    for inode in inodes_free:
        unalloc_inodes.append(int(inode.split(',')[1]))
    #Determine inconsistencies for allocated and unallocated values
    for item in range(11, int(sb[0].split(',')[2]) + 1):
        if item in alloc_inodes and item in unalloc_inodes:
            set_incon("ALLOCATED INODE " + str(item) + " ON FREELIST")
        if item not in alloc_inodes and item not in unalloc_inodes:
            set_incon("UNALLOCATED INODE " + str(item) + " NOT ON FREELIST")
    
    #special case for 2
    if 2 in alloc_inodes and 2 in unalloc_inodes:
        set_incon("ALLOCATED INODE 2 ON FREELIST")
    if 2 not in alloc_inodes and 2 not in unalloc_inodes:
        set_incon("UNALLOCATED INODE 2 NOT ON FREELIST")
"""
Audits the blocks to perform indirected,
allocated/unallocated, and refereced/unreferenced checks
"""        
def block_audit():
    #Splits the superblock after the first comma and takes the integer
    #that follows immediately after the comma (2 item in list)
    max_block_value = int(sb[0].split(',')[1])
    checked_blocks = {}
    bmax = 4
    free_blocks_local = []

    for inode in inodes:
        isplit = inode.split(',')
        #Splits inode and holds second element in comma seperated list
        inodeNum = isplit[1]
        #Splits blocks and holds 12th thru 23rd elems in cs list (ends b4 24)
        blocks = isplit[12:24]
        #for each block so 12 times
        for block in blocks:            
            if int(block) <= -1 or int(block) > max_block_value:
                set_incon(inv_blk + block + nstr +
                          inodeNum + ostr + str(blocks.index(block)))
            elif int(block) < bmax and int(block) > 0:
                set_incon(res_blk + block + nstr +
                          inodeNum + ostr + str(blocks.index(block)))                

        #Splits indirect blocks from 24th to 26th
        indirect = isplit[24:27]
        
        
        #For 3 elements in list
        for elems in indirect:
            indone = indirect.index(elems) + 1
            indtwe = indone + 11
            level = str(indone)

            #Tuple did not return correctly            
            offset = str(indtwe)
            level_as_string = "INDIRECT"

            #Determine Depth, exit, or set indirection strings
            if int(level) < 0:
                exitE("Error: Level depth below zero") 
            elif level == "2":
                level_as_string = dblstr 
                offset = dbloff
            elif level == "3":
                level_as_string = tripstr
                offset = tripoff
            elif int(level) > 4:
                exitE("Incorrect level values, exiting 1")

            #Check if elements are less or equal -1 or above max possible block
            if int(elems) <= -1 or int(elems) > max_block_value:
                #Set indirection string appropriately
                if level_as_string == "INDIRECT":
                    full_string = inv_ind
                elif level_as_string == dblstr:
                    full_string = inv_dind
                elif level_as_string == tripstr:
                    full_string = inv_tind
                set_incon(full_string + elems + nstr + str(inodeNum) + ostr + offset)

            if int(elems) < 4:
                #If elements are inbetween
                if int(elems) > 0:
                    #Set indirection string appropriately
                    if level_as_string == "INDIRECT":
                        full_string = res_ind
                    elif level_as_string == dblstr:
                        full_string = res_dind
                    elif level_as_string == tripstr:
                        full_string = res_tind
                    set_incon(full_string + elems + nstr + str(inodeNum) + ostr + offset)
                    
    for f in blocks_free:
        free_blocks_local.append(int(f.split(',')[1]))

    for inode in inodes:
        isplit = inode.split(',')
        isplit_blocks = isplit[12:24]
        for bl in isplit_blocks:
            if int(bl) > 3 and int(bl) < max_block_value:
                if int(bl) in checked_blocks:
                    checked_blocks[int(bl)].append(0)
                    checked_blocks[int(bl)].append(isplit[1])
                    checked_blocks[int(bl)].append(int(isplit_blocks.index(bl)))
                else:
                    checked_blocks[int(bl)] = [0, isplit[
                        1], int(isplit_blocks.index(bl))]
        b = isplit[24:27]
        
        
        #Set block list elements depending upon position
        zero = int(b[0])
        if zero > 0x3 and zero < max_block_value:
            if zero in checked_blocks:
                checked_blocks[zero].append(1)
                checked_blocks[zero].append(isplit[1])
                checked_blocks[zero].append(int(12 + b.index(b[0])))
            else:
                checked_blocks[zero] = [1, isplit[
                    1], int(12 + b.index((b[0])))]
                
        one = int(b[1])
        if one > 3 and one < max_block_value:
            if one in checked_blocks:
                checked_blocks[one].append(2)
                checked_blocks[one].append(isplit[1])
                checked_blocks[one].append(int(dbloff))
            else:
                checked_blocks[one] = [2, isplit[
                    1], int(dbloff)]
         
        two = int(b[2])
        if two > 3 and two < max_block_value:
            if two in checked_blocks:
                checked_blocks[two].append(3)
                checked_blocks[two].append(isplit[1])
                checked_blocks[two].append(int(tripoff))
            else:
                checked_blocks[two] = [3, isplit[
                    1], int(tripoff)]
                
    for ind in indirect_blocks:

        indsplit = ind.split(',')
        count_blocks = int(indsplit[5])
        num_inodes = int(indsplit[1])
        level = int(indsplit[2]) - 1
        offset = int(indsplit[3])
        oflist = " ON FREELIST"
        #
        
        #Set the global variables depending on level
        if level < 0:
            exitE("Error: Level below expected values")
        elif level == 1:
            offset = int(dbloff)
        elif level == 2:
            offset = int(tripoff)
            
        if count_blocks > 3 and count_blocks < max_block_value:
            if count_blocks in checked_blocks:
                checked_blocks[count_blocks].append(level)
                checked_blocks[count_blocks].append(num_inodes)
                checked_blocks[count_blocks].append(offset)
            else:
                checked_blocks[count_blocks] = [
                    level, num_inodes, offset]
                
    front_block_in_list = int(int(groups[0].split(',')[8]) + int(groups[0].split(',')[3])
    * int(sb[0].split(',')[4]) / int(sb[0].split(',')[3]))

    #Handle Allocated/UnAllocated
    for it in range(front_block_in_list, max_block_value):
        if it not in checked_blocks and it not in free_blocks_local:
            set_incon(unrefblk + str(it))
        elif it in checked_blocks and it in free_blocks_local:
            set_incon(alocblk + str(it) + oflist)
        

    #Handle duplicates    
    for it in checked_blocks:
        temp = checked_blocks[it]
        if (len(temp) > 3):
            for i in range(0, len(temp), 3):
                comm = str(it) + nstr + str(temp[i+1]) + ostr + str(temp[i+2])
                if temp[i] == 0:
                    set_incon(dup_blk + comm)
                elif temp[i] == 1:
                    set_incon(dup_ind + comm)
                elif temp[i] == 2:
                    set_incon(dup_dind + comm)
                elif temp[i] == 3:
                    set_incon(dup_tind + comm)
"""
Performs the directory audits
checks if current or parent direc or other
Inodes should matcch the number of discovered links
Only refers to valid and allocated inodes
"""
def dir_audit():
    #Helper strings
    dnode = "DIRECTORY INODE "
    dir_parent = {}
    inv_i = " INVALID INODE "
    unaloc_i = " UNALLOCATED INODE "
    inodes_to_check = {}
    dpar = " NAME '..' LINK TO INODE "
    dcur = " NAME '.' LINK TO INODE "
    
    #Set all node elements to 0
    for item in range(11, int(sb[0].split(',')[2]) + 1):
        inodes_to_check[item] = 0
    inodes_to_check[2] = 0 #special case
    dir_parent[2] = 2
    linkstr = " LINKS BUT LINKCOUNT IS "
    for entries in direc_entries:
        temp = entries.split(',')
        #Check for current and parent directiories
        #We know that every directory should begin with two links
        #one to itself and one to is parent
        if temp[6] != "'.'" and temp[6] != "'..'":
            dir_parent[int(temp[3])] = int(temp[1])   

    #Helper strings
    should = " SHOULD BE "
    namestr = " NAME "
    #Handle unaloc/ivalid i nodes
    for entries in direc_entries:
        esplit = entries.split(',')
        num_inodes = int(esplit[3])
        if num_inodes < 1 or num_inodes > int(sb[0].split(',')[2]):
            set_incon(dnode + esplit[1] + namestr
            + esplit[6] + inv_i + str(num_inodes))
        elif num_inodes not in alloc_inodes:
            set_incon(dnode + esplit[1] + namestr
            + esplit[6] + unaloc_i + str(num_inodes))
        else:
            inodes_to_check[num_inodes] = inodes_to_check[num_inodes] + 1
        if esplit[6] == "'..'":
            if int(esplit[3]) != dir_parent[int(esplit[1])]:
                set_incon(dnode + esplit[1] + dpar +
                esplit[3] + should + str(dir_parent[int(esplit[1])]))
        elif esplit[6] == "'.'":
            if esplit[3] != esplit[1]:
                set_incon(dnode + esplit[1] + dcur +
                esplit[3] + should + esplit[1])
        
                
    #Handle Inconsistency check for links            
    for item in inodes:
        isplit = item.split(',')
        if int(isplit[6]) != inodes_to_check[int(isplit[1])]:
            set_incon("INODE " + isplit[1] + " HAS " + str(inodes_to_check[int(isplit[1])]) + linkstr + isplit[6])
    
    
if __name__ == '__main__':

    #Check valid arguments
    fdes = arg_check()
    #Read the valid file descriptor
    rex = fdes.read()
    
    #Instantiate globals
    global sb, groups, blocks_free, inodes_free, inodes, direc_entries, indirect_blocks
    #Uses reg expressions to find the "" and take all information on line
    sb = re.findall('SUPERBLOCK.*', rex)
    groups = re.findall('GROUP.*', rex)
    blocks_free = re.findall("BFREE.*", rex)
    inodes_free = re.findall("IFREE.*", rex)
    direc_entries = re.findall("DIRENT.*", rex)
    inodes = re.findall("INODE.*", rex)
    indirect_blocks = re.findall("INDIRECT.*", rex)

    #Handle consistent audit checks and exit successfully if verified
    block_audit()
    inode_audit()
    dir_audit()
    
    # Close just in case Python doesn't automatically close and exit with status 2 if inconsistency detected, otherwise exit successfuly with status 0
    fdes.close()
    exit(verify_con())
