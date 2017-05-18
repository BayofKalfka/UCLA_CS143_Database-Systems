#include "BTreeNode.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{   // refer to PageFile::read(PageId pid, void* buffer)
	return pf.read(pid, buffer);
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{   // refer to PageFile::read(PageId pid, void* buffer)
	return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
	PageFile pf;

	// the size of each entry pair
	int keySize = sizeof(int);
	int pointerSize = sizeof(RecordId);
	int pairSize = keySize + pointerSize;

	int count = 0;
	char *temp = buffer+sizeof(RecordId);
	int i;

	for(i=0;i<=pf.PAGE_SIZE-pairSize-sizeof(PageId);i+=pairSize)
	{
		int currentKey;
		memcpy(&currentKey,temp,keySize);// read current key to 'currentKey'
		if (currentKey==0)// if current key is 0, break
		{
			break;
		}else{
			count++;
			temp+=pairSize;
		}
	}
	
	return count; 
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ 
	PageFile pf;
	// Check the pid of this node
	PageId pid = getNextNodePtr();
	// Check the size of each entry pair
	int pairSize = sizeof(int) + sizeof(RecordId);
	// Check the capability of entry pairs in this node
	int totalPairNum = (PageFile::PAGE_SIZE - sizeof(PageId))/pairSize;
	// Check if this node will overflow after insert
	if (getKeyCount()+1>totalPairNum)
	{
		return RC_NODE_FULL;
	}
	// Check where to insert the new entry, the insert address is i
	char* temp= buffer+sizeof(RecordId);
	int i;

	for (int i = 0; i <= pf.PAGE_SIZE-sizeof(RecordId); i+=pairSize)
	{
		int currentKey;
		memcpy(&currentKey,temp,sizeof(int));
		// If key to insert is smaller than current key or current key is 0, we break
		if (key<currentKey || currentKey==0)
		{
			break;
		}
		temp=temp+pairSize;

	}
	//Copy the fisrt part(first i+pairSize bytes) from buffer to newBuffer 
	char *newBuffer = (char*)malloc(sizeof(buffer));//allocate a memory block for newBuffer
	memset(newBuffer, 0, sizeof(buffer));// clear the memory if necessary
	memcpy(newBuffer,buffer,i);
	//Copy the second part-(rid,key) pair to newBuffer
	memcpy(newBuffer+i,&rid,sizeof(RecordId));
	memcpy(newBuffer+i+sizeof(RecordId),&key,sizeof(int));
	//Copy the third part-the rest part of the current node to newBuffer
	// Skip the last pid
	memcpy(newBuffer+i+pairSize,buffer+i,sizeof(buffer)-i-sizeof(PageId));
	//Copy current PageId to the end of the newBuffer
	memcpy(newBuffer+sizeof(buffer)-sizeof(PageId),&pid,sizeof(PageId));

	//Copy newBuffer back to buffer and free newBuffer
	memcpy(buffer,newBuffer,sizeof(buffer));
	free(newBuffer);

	//Successfully add new entry and return 0
	return 0; 
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	PageFile pf;
	// Check the size of each entry pair
	int pairSize = sizeof(int) + sizeof(RecordId);
	// Check the capability of entry pairs in this node
	int totalPairNum = (PageFile::PAGE_SIZE - sizeof(PageId))/pairSize;
	// Check the pid of this node
	PageId pid = getNextNodePtr();
	// Check the sibling buffer is empty, if not return error code
	if (!(sibling.getKeyCount()==0))
	{
		return RC_INVALID_FILE_MODE;
	}
	//Check overflow will happen after insert, if not return the error code
	if (!(getKeyCount()+1>totalPairNum))
	{
		return RC_INVALID_FILE_FORMAT;
	}
	// Clear the sibling buffer if necessary
	memset(sibling.buffer,0,sizeof(buffer));

	//Split the buffer half to sibling buffer 
	//Ckeck the number of keys to keep in buffer
	int numKeysKeep=(getKeyCount()+1)/2;
	//Check where to split : splitIndex
	int splitIndex=pairSize * numKeysKeep;
	//Copy the second half of buffer to sibling.buffer
	//Skip the last pid
	memcpy(sibling.buffer,buffer+splitIndex,sizeof(buffer)-sizeof(PageId)-splitIndex);
	// Clear the second half of buffer 
	// Skip the last pid
	memset(buffer+splitIndex,0,sizeof(buffer)-sizeof(PageId)-splitIndex);
	// Return siblingKey
	memcpy(&siblingKey,sibling.buffer,sizeof(int));
	// Check which node to insert
	if (key>siblingKey)
	{
		sibling.insert(key,rid);
	}else{
		insert(key,rid);
	}
	// Update siblingKey after insert
	memcpy(&siblingKey,sibling.buffer,sizeof(int));
	// Set pid to sibling node pid ptr

	return 0; 
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	PageFile pf;
	// the size of each entry pair
	int keySize = sizeof(int);
	int pointerSize = sizeof(RecordId);
	int pairSize = keySize + pointerSize;

	char *temp = buffer+sizeof(RecordId);
	int i;
	int count=0;

    //search if the searchKey exists in the node;
    //if exists, set the corresponding eid and return 0;
    //if not exists, set the corresponding eid and reutrn certain error
	for(i=sizeof(RecordId);i<=pf.PAGE_SIZE-sizeof(PageId)-pairSize;i+=pairSize)
	{
		int currentKey;
		memcpy(&currentKey,temp,keySize);

		//find the right key
		if (currentKey==searchKey) 
		{
			eid = count;
			return 0;
		}
		//since the key is sorted, if currentKey is larger than searchKey,
		//Set eid and return error
		if(currentKey>searchKey){ 
			eid = count;
			return RC_NO_SUCH_RECORD;
		}
		count++;
		temp+=pairSize;
	}

    //after finish the loop, the function still works,
    //then it means the 
	eid = count;
	return RC_NO_SUCH_RECORD;
}
 

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{  
	int keySize = sizeof(int);
	int pointerSize = sizeof(RecordId);
	int pairSize = keySize + pointerSize;

	char *temp = buffer;

    //if the given eid is out of range, return error
	if(eid+1>=getKeyCount())
		return RC_NO_SUCH_RECORD;

	memcpy(&rid, temp+eid*pairSize, sizeof(RecordId));
	memcpy(&key, temp+eid*pairSize+sizeof(RecordId), sizeof(int));

	return 0; 
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{  
	PageId pid=0;
    PageFile pf;
    char* temp=buffer;

    memcpy(&pid, temp+pf.PAGE_SIZE-sizeof(PageId), sizeof(PageId));
    
	return pid; 
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	PageFile pf;
    char* temp=buffer;

    if(pid<0)
    	return RC_INVALID_PID;

    memcpy(temp+pf.PAGE_SIZE-sizeof(PageId), &pid, sizeof(PageId));
    
	return 0; 
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ 
	return pf.read(pid, buffer);
 }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ 
	return pf.write(pid, buffer); 
}
/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ 
	PageFile pf;

	// the size of each entry pair
	int keySize = sizeof(int);
	int pointerSize = sizeof(PageId);
	int pairSize = keySize + pointerSize;

	// Count the number of keys in this node
	int count = 0;
	char *temp = buffer+sizeof(PageId);
	int i;

	for(i=0;i<=pf.PAGE_SIZE-pairSize-sizeof(PageId);i+=pairSize)
	{
		int currentKey;
		memcpy(&currentKey,temp,keySize);// read current key to 'currentKey'
		if (currentKey==0)// if current key is 0, break
		{
			break;
		}
			count++;
			temp+=pairSize;
		
	}

	return count;
	
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ 
	PageFile pf;
	// Check the size of each entry pair
	int pairSize = sizeof(int) + sizeof(PageId);
	// Check the capability of entry pairs in this node
	int totalPairNum = (PageFile::PAGE_SIZE - sizeof(PageId))/pairSize;
	// Check if this node will overflow after insert, if not return error code
	if (getKeyCount()+1>totalPairNum)
	{
		return RC_NODE_FULL;
	}
	// Check where to insert the new entry: i
	// Keep the keys sorted after insert
	char* temp= buffer+sizeof(PageId);
	int i;
	// Skip the first pid
	for (int i = sizeof(PageId); i <= pf.PAGE_SIZE-sizeof(PageId); i+=pairSize)
	{
		int currentKey;
		memcpy(&currentKey,temp,sizeof(int));
		// If key to insert is smaller than current key or current key is 0, we break
		if (key<currentKey || currentKey==0)
		{
			break;
		}
		temp=temp+pairSize;

	}
	//Copy the fisrt part(first i+pairSize bytes) from buffer to newBuffer 
	char *newBuffer = (char*)malloc(sizeof(buffer));//allocate a memory block for newBuffer
	memset(newBuffer, 0, sizeof(buffer));// clear the memory if necessary
	memcpy(newBuffer,buffer,i);
	//Copy the second part-(key,pid) pair to newBuffer
	memcpy(newBuffer+i,&key,sizeof(int));
	memcpy(newBuffer+i+sizeof(int),&pid,sizeof(PageId));
	//Copy the third part-the rest part of the current node to newBuffer
	memcpy(newBuffer+i+pairSize,buffer+i,sizeof(buffer)-i);
	//Copy current PageId to the end of the newBuffer
	//memcpy(newBuffer+sizeof(buffer)-sizeof(PageId),&getNextNodePtr(),sizeof(PageId));

	//Copy newBuffer back to buffer and free newBuffer
	memcpy(buffer,newBuffer,sizeof(buffer));
	free(newBuffer);

	//Successfully add new entry and return 0
	return 0; 
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{

	PageFile pf;
	//Check the size of each entry pair
	int pairSize = sizeof(int) + sizeof(PageId);
	// Check the capability of entry pairs in this node
	int totalPairNum = (PageFile::PAGE_SIZE - sizeof(PageId))/pairSize;
	// Check overflow will happen after insert, if not return error code
	if (!(getKeyCount()+1>totalPairNum))
	{
		return RC_INVALID_FILE_FORMAT;
	}
	// Check the sibling buffer is empty, if not return error code
	if (!(sibling.getKeyCount()==0))
	{
		return RC_INVALID_FILE_MODE;
	}
	// Clear the sibling buffer if necessary
	memset(sibling.buffer,0,sizeof(buffer));

	// Split the node half and half with sibling
	// Check number of keys to keep in buffer
	int numKeysKeep = (getKeyCount()+1)/2;
	// Check halfIndex
	int halfIndex = numKeysKeep*pairSize + sizeof(PageId);
	// Check the last key of first half and first key of second half
	int firstHalfKey;
	int secondHalfKey;
	memcpy(&firstHalfKey,buffer+halfIndex-pairSize,sizeof(int));
	memcpy(&secondHalfKey,buffer+halfIndex,sizeof(int));

	// Check midKey
	// Split the node
	// Remove midKey
	// If key is smaller than firstHalfKey, then firstHalfKey is the midKey
	if (key<firstHalfKey)
	{
		midKey=firstHalfKey;
		// Check where is the firstHalfKey in buffer: splitIndex
		// And split the node here 
		int splitIndex = halfIndex;
		// Copy the second half to sibling.buffer
		// From  splitIndex to the end of buffer
		// Skip the first sizeof(PageId) bytes of sibling.buffer and leave it for initial pid 
		memcpy(sibling.buffer+sizeof(PageId),buffer+splitIndex,sizeof(buffer)-splitIndex);
		// Set the last pid of the fisrt half of buffer as the initial pid of sibling.buffer
		// Clear the second half of buffer
		memset(buffer+splitIndex,0,sizeof(buffer)-splitIndex);
		// key is smaller than firstHalfKey
		// So insert the entry in buffer
		insert(key,pid);
		// Remove midKey
		// CLear the last key in first half
		memset(buffer+splitIndex-pairSize,0,sizeof(int));

	}
	// Check midKey
	// Split the node
	// Remove midKey
	// If key is bigger than secondHalfKey, then secondHalfKey is the midKey
	else if (key>secondHalfKey)
	{
		midKey=secondHalfKey;
		// Check where is the firstHalfKey in buffer: splitIndex
		// And split the node here 
		int splitIndex = halfIndex;
		// Copy the second half to sibling.buffer
		// From  splitIndex to the end of buffer
		// Skip the first sizeof(PageId) bytes of sibling.buffer and leave it for initial pid 
		memcpy(sibling.buffer+sizeof(PageId),buffer+splitIndex,sizeof(buffer)-splitIndex);
		// Set the last pid of the fisrt half of buffer as the initial pid of sibling.buffer
		// Clear the second half of buffer
		memset(buffer+splitIndex,0,sizeof(buffer)-splitIndex);
		// key is bigger than secondHalfKey
		// So insert the entry in sibling.buffer
		sibling.insert(key,pid);
		// Remove midKey
		// CLear the first key in sebling.buffer
		memset(sibling.buffer+sizeof(PageId),0,sizeof(int));

	}
	else{
		midKey=key;// if key is between fisrtHalfKey and secondHalfKey, key is the mideKey
		// Check where is the firstHalfKey in buffer: splitIndex
		// And split the node here 
		int splitIndex = halfIndex;
		// Copy the second half to sibling.buffer
		// From  splitIndex to the end of buffer
		// Skip the first sizeof(PageId) bytes of sibling.buffer and leave it for initial pid 
		memcpy(sibling.buffer+sizeof(PageId),buffer+splitIndex,sizeof(buffer)-splitIndex);
		// Clear the second half of buffer
		memset(buffer+splitIndex,0,sizeof(buffer)-splitIndex);
		// Since we have to remove the midKey=key, we just donot insert the key
		// And only copy the entry pid as the first pid of sibling.buffer 
		memcpy(sibling.buffer,&pid,sizeof(PageId));

	}

	return 0;


}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ 
	// the size of each key and pageid pair
	int pairSize = sizeof(int) + sizeof(PageId);
	PageFile pf;
	char *temp = buffer;
	int i;

    //search if the searchKey exists in the node;
    //if exists, set the corresponding eid and return 0;
    //if not exists, set the corresponding eid and reutrn certain error
	for(i=sizeof(PageId); i<=pf.PAGE_SIZE-pairSize; i+=pairSize)
	{
		
		int currentKey;
		memcpy(&currentKey, temp+i, sizeof(int));

        //when currentKey = searchKey, return the pid after currentkey
		if (currentKey==searchKey) 
		{
			memcpy(&pid, temp+i+sizeof(int), sizeof(PageId));
			return 0;
		}

        //when currentKey > searchKey, return the pid before currentkey
		if(currentKey > searchKey){
			memcpy(&pid, temp+i-sizeof(PageId), sizeof(PageId));
			return 0;
		}
	}

    //when searchKey is greater than all keys in the node, return last pid
    memcpy(&pid, temp+pf.PAGE_SIZE-sizeof(PageId), sizeof(PageId));
    return 0; 
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 
	char* temp = buffer;

	if(pid1 >= pid2)
		return RC_INVALID_PID;

	memcpy(temp, &pid1, sizeof(PageId));
	memcpy(temp+sizeof(PageId), &key, sizeof(int));
	memcpy(temp+sizeof(PageId)+sizeof(int), &pid2, sizeof(PageId));

	return 0; 
}
