/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
    std::fill(buffer, buffer + PageFile::PAGE_SIZE, 0);
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
	RC error;
	char* temp = buffer;

	error = pf.open(indexname, mode);
	if(error!=0)
		return error;

	if(pf.endPid()==0){
		rootPid=-1;
		treeHeight=0;

		RC writeerror = pf.write(0, temp);
		if(writeerror!=0)
			return writeerror;

	}else{
		RC readerror = pf.read(0, temp);
		if(readerror!=0)
			return readerror;
		memcpy(&rootPid, temp, sizeof(int));
		memcpy(&treeHeight, temp+sizeof(int), sizeof(int));
	}

    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	RC error;
	char* temp = buffer;

	memcpy(buffer, &rootPid, sizeof(int));
	memcpy(buffer+sizeof(int), &treeHeight, sizeof(int));

	error=pf.write(0, buffer);
	if(error!=0)
		return error;

	error=pf.close();
	if(error!=0)
		return error;

    return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    RC error;
    int currentheight = 1;
    PageId pid = rootPid;

    if(key < 0)
    	return RC_INVALID_ATTRIBUTE;

    if(treeHeight==0){
    	BTLeafNode root;
    	treeHeight++;
    	error = root.insert(key, rid);
    	if(error!=0)
    		return error;
    	error = root.write(1, pf);
    	if(error!=0)
    		return error;
    }

    int newkey = -1;
    int newpageid = -1;

    error = insertRec(key, rid, currentheight, pid, newkey, newpageid);
    if(error!=0)
    	return error;

    return 0;
}

RC BTreeIndex::insertRec(int key, const RecordId& rid, int currentheight, PageId pid, int& newkey, PageId& newpageid){

	RC error;
	BTNonLeafNode nonleaf;
	BTLeafNode leaf;

	newkey=-1;
	newpageid=-1;

	//reach leaf node
	if(currentheight == treeHeight){
		error=leaf.read(pid, pf);
		if(error!=0)
			return error;
		if(leaf.getKeyCount()>=85){  //if the leafnode is full, call insertAndSplite
			BTLeafNode newleaf;
			error = leaf.insertAndSplit(key, rid, newleaf, newkey);
			if(error!=0)
				return error;

			newleaf.setNextNodePtr(leaf.getNextNodePtr());
			leaf.setNextNodePtr(pf.endPid());
			newpageid = pf.endPid();

			error = newleaf.write(pf.endPid(), pf);
			if(error!=0)
				return error;

			error = leaf.write(pid, pf);
			if(error!=0)
				return error;

			if(treeHeight==1){
				BTNonLeafNode root;
				treeHeight++;
				root.initializeRoot(pid, newkey, newpageid);
				rootPid = pf.endPid();
				error = root.write(rootPid, pf);
				if(error!=0)
					return error;
			}

			return 0;
		}else{
			error = leaf.insert(key, rid);
			if(error!=0)
				return error;
			error = leaf.write(pid, pf);
			if(error!=0)
				return error;
			return 0;
		} 


	}else{
		error = nonleaf.read(pid, pf);
		if(error!=0)
			return error;
		PageId nextpid;
		nonleaf.locateChildPtr(key, nextpid);

		int nextnewkey =-1;
		PageId nextnewpid = -1;

		insertRec(key, rid, currentheight++, nextpid, nextnewkey, nextnewpid);

		//if the childnode overload, we should deal with the current parent node
		if(nextnewkey>=0 && nextnewpid >=0){
			if(nonleaf.getKeyCount()>=85){  //if the nonleafnode is full, call insertAndSplite
			BTNonLeafNode newnonleaf;
			error = nonleaf.insertAndSplit(nextnewkey, nextnewpid, newnonleaf, newkey);
			if(error!=0)
				return error;

			newpageid = pf.endPid();
			error = newnonleaf.write(newpageid, pf);
			if(error!=0)
				return error;

			error = nonleaf.write(pid, pf);
			if(error!=0)
				return error;

			if(currentheight==1){
				BTNonLeafNode root;
				treeHeight++;
				root.initializeRoot(pid, newkey, newpageid);
				rootPid = pf.endPid();
				error = root.write(rootPid, pf);
				if(error!=0)
					return error;
			}

			return 0;
		}else{
			error = nonleaf.insert(nextnewkey, nextnewpid);
			if(error!=0)
				return error;
			error = nonleaf.write(pid, pf);
			if(error!=0)
				return error;
			return 0;
		}

		}

	}


}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
	RC error;
	int searchheight=1;
	PageId pid = rootPid;
	BTNonLeafNode nonleaf;
	BTLeafNode leaf;
	int eid;

	while(searchheight<treeHeight){
		error=nonleaf.read(pid, pf);
		if(error!=0)
			return error;

		nonleaf.locateChildPtr(searchKey, pid);
		searchheight++;

	}

	cursor.pid = pid;

	error = leaf.read(pid, pf);
	if(error!=0)
		return error;
	leaf.locate(searchKey, eid);
	cursor.eid=eid;

    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	RC error;

	PageId indexpid = cursor.pid;
	int indexeid = cursor.eid;

	BTLeafNode leafnode;

	if(indexpid<=0)
		return RC_INVALID_CURSOR;

	error=leafnode.read(indexpid, pf);
	if(error!=0)
		return error;
	

	error=leafnode.readEntry(indexeid, key, rid);
	if(error!=0)
		return error;

    //eid begins from zero, so ">=" is used, to judge if next cursor is in next page
	if(indexeid+1>=leafnode.getKeyCount()){
		indexeid=0;		
		indexpid=leafnode.getNextNodePtr();
	}else{
		indexeid++;
	}

	cursor.pid = indexpid;
	cursor.eid = indexeid;
    return 0;
}
