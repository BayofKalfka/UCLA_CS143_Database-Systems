/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
 
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  BTreeIndex index; //cursor for navigating through the tree
  IndexCursor cursor; //BTree for indexing

  RC     rc;
  int    key;     
  string value;
  int    count = 0;
  int    diff;

     // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
  fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
  return rc;
  }

    // temporary variable for WHERE condition
  SelCond temp;
  // check if the condition is existing
     // if false skip the B+ Tree index
  bool hasCond = false; 
 // check if value condition exists
    // if false skip the page read
  bool hasValueCond = false;
  // check if we need to use the index helper
    // if false we use the original scanning method
  bool useIndex = false;
  // check if any key conditions are conflicting
  bool validKeyCond = false;
    // max and min key value 
  int max = -1;
  int min = -1;
  int searchKey = -1;
  bool GEmin =false; // true if key >= min
  bool LEmax= false; // true if key <= max
  // check if any value conditions are conflicting
  bool valueConflict = false;
   std::string valEq = "";
  // the index of the condition
  int condIndex =-1; 

  // use loop to go through the conds and check conditions
  for (int i = 0; i < sizeof(cond); i++)
  {
    temp = cond[i]; // use a temp vector to store current conditon parameters
    int tempValue = atoi(temp.value); // store the value to compare in int form

    // we only use index helper when there is no NE condition
    // select statement with NE conditions will be processed by original scanning method
    if (temp.attr =1 && temp.comp!= SelCond::NE) // if attrbute is key and comparator is not NE, use index
    {
      hasCond=true;
      if (temp.comp==SelCond::EQ) // if there is an EQ conditon on key, then we can just use this condition to do selection; break
      {
        searchKey=tempValue;
        condIndex=i;
        break;
      }
      else if (temp.comp==SelCond::GE) // check key >= min
      {
        if (tempValue >min || min == -1)
        {
          GEmin = true;
          min= tempValue;
        }

      }
      else if (temp.comp==SelCond::GT)//key > min
      {
        if (tempValue >= min || min == -1)
        {
          GEmin=false;
          min = tempValue;
        }
      }
      else if (temp.comp==SelCond::LE)//key <= max
      {
        if (tempValue < max || max == -1)
        {
          LEmax=true;
          max = tempValue;
        }
      }
      else if (temp.comp==SelCond::LT)//key < max
      {
        if (tempValue < max || max == -1)
        {
          LEmax=false;
          max = tempValue;
        }
      }
    }
    else if(temp.attr ==2) // if the condition is value condition
    {  
      hasValueCond=true;
      if(temp.comp=SelCond::EQ)// check on value equality
      { 
        if (valEq=="" || strcmp(value.c_str(),cond[i].value)==0)// if the con[i].value is not empty
        
          valEq=tempValue;
        
        else
          valueConflict = true;
      }
    }

}

// Check if the conditions are impossible and end early
if (valueConflict || (max!=-1 && min!=-1&& max<min))

  goto end_early;

if (max!= -1 && min!=-1 && !GEmin && !LEmax && max==min)
  goto end_early;
///////////////////////////////////////
// if the index does not exist or empty condition we use orginal select without index
if (index.open(table+".idx",'r')!=0 || (!hasCond && attr!=4))
{
  // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    while (rid < rf.endRid()) 
    {   continue_while:
        // read the tuple
        if ((rc = rf.read(rid, key, value)) < 0) 
        {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
         }

        // check the conditions on the tuple
        for (unsigned i = 0; i < cond.size(); i++) 
        {
          // compute the difference between the tuple value and the condition value
            switch (cond[i].attr)
            {
                case 1:
              diff = key - atoi(cond[i].value);
              break;
                case 2:
              diff = strcmp(value.c_str(), cond[i].value);
              break;
            }

            // skip the tuple if any condition is not met
            switch (cond[i].comp) 
           {
                case SelCond::EQ:
              if (diff != 0) goto next_tuple;
              break;
                case SelCond::NE:
              if (diff == 0) goto next_tuple;
              break;
                case SelCond::GT:
              if (diff <= 0) goto next_tuple;
              break;
                case SelCond::LT:
              if (diff >= 0) goto next_tuple;
              break;
                case SelCond::GE:
              if (diff < 0) goto next_tuple;
              break;
                case SelCond::LE:
              if (diff > 0) goto next_tuple;
              break;
            }
        }

        // the condition is met for the tuple. 
        // increase matching tuple counter
        count++;

        // print the tuple 
        switch (attr) 
        {
          case 1:  // SELECT key
            fprintf(stdout, "%d\n", key);
            break;
          case 2:  // SELECT value
            fprintf(stdout, "%s\n", value.c_str());
            break;
          case 3:  // SELECT *
            fprintf(stdout, "%d '%s'\n", key, value.c_str());
            break;
        }

        // move to the next tuple
        next_tuple:
        ++rid;
    }
} // 162 if{}

/////////////////////////////////////
else // otherwise, use index
{
    // initialize variables
    count=0;
    rid.pid = rid.sid =0;
    useIndex=true;

    // Find start point for IndexCursor Cursor
    if (searchKey!=-1)
      index.locate(searchKey,cursor);
    else if (min!=-1&& !GEmin) // check if key is greater than min
      index.locate(min+1,cursor); 
    else if (min!=-1 && GEmin)
      index.locate(min,cursor);
    else
      index.locate(0,cursor);
  
    while(index.readForward(cursor, key, rid)==0)
    {
        if (!hasValueCond && attr==4)  // no need of page read
        {
            if (searchKey!=-1 && key!=searchKey) // if condition on key does not match, end
              goto end_early;
            if (max!=-1)
            {
                if (LEmax && key>max)
                  goto end_early;
                else if (!LEmax&& key>=max)
                  goto end_early;
            }
            if (min!=-1)
            {
                if (GEmin && key<min)
                  goto end_early;
                else if (!GEmin && key<=min)
                  goto end_early;
            }
            count++;
            continue;
        }
      
        // read the tuple
        if ((rc=rf.read(rid,key,value))<0)
        {
          fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
          goto exit_select;
        }
        // check the conditions on the tuple (modified from given code)
        for (int i = 0; i < cond.size(); i++)
        {
            // compute the difference between the tuple value and the condition value
            switch (cond[i].attr)
            {
                //if condition on key
                case 1:
                  diff = key - atoi(cond[i].value);
                  break;
                //if condition on value
                case 2:
                  diff = strcmp(value.c_str(), cond[i].value);
                  break;
            }
            //skip the tuple if any condition is not met
            //for less than (LT) or less than or equal (LE) conditions on keys, once one of them fails, we are done
            //since the tree index is sorted, every key afterwards is also going to fail the condition
            //the same applies with EQ, since once we're past the key that was equal, everything else will fail also
          switch (cond[i].comp)
          {
              case SelCond::EQ:
                if (diff != 0)
                {
                  if(cond[i].attr==1)
                    goto end_early;
                  goto continue_while;
                }
                break;
              case SelCond::NE:
                if (diff == 0) goto continue_while; //if keys ever match when they're not supposed to, break out of for-loop and wait for next cursor
                  break;
              case SelCond::GT:
                if (diff <= 0) goto continue_while; //if !(key > cond value), break out of for-loop and wait for next cursor
                  break;
              case SelCond::LT:
                if (diff >= 0)
                {
                  if(cond[i].attr==1) //if this ever fails on a key, everything else after will fail anyway, so we end
                    goto end_early;
                  goto continue_while;
                }
                break;
              case SelCond::GE:
                if (diff < 0) goto continue_while; //if !(key >= cond value), break out of for-loop and wait for next cursor
                break;
              case SelCond::LE:
                if (diff > 0)
                {
                  if(cond[i].attr==1) //if this ever fails on a key, everything else after will fail anyway, so we end
                    goto end_early;
                  goto continue_while;
                }
                break;
          }
        } // 242 for{}
        // the condition is met
        // increment the  tuple counter
        count++;
        //print the result
        switch (attr)
        {
            case 1:  // SELECT key
              fprintf(stdout, "%d\n", key);
              break;
            case 2:  // SELECT value
              fprintf(stdout, "%s\n", value.c_str());
              break;
            case 3:  // SELECT *
              fprintf(stdout, "%d '%s'\n", key, value.c_str());
              break;
        }
        continue_wile:
        cout << ""; //do nothing; we use this to jump to the next while cycle
    }
}  // 195 else {}
///////////////////////////////////////
end_early: //use to leave the while loop in case some tuple fails select conditions
  // print matching tuple count if "select count(*)"
  if (attr == 4) 
  {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

exit_select:
  if(useIndex)
    index.close(); //close the index file if applicable
  
rf.close();
return rc;

}



RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  /* your code here */
  

  RecordFile rf;
  fstream lf(loadfile.c_str());// split loadfile into tokens using null-character, then write into lf
  RecordId rid;
  BTreeIndex tree;


  RC rc;
  int addkey;
  string addvalue;
  string line;

  if(!lf){
    fprintf(stderr, "Error: cannot open loadfile %s\n", loadfile.c_str());
    exit(-1);
  };
  if (index)
  {
    // if the index is set to br true, create the B+ tree  file
    tree.open(table + ".idx", 'w');
    while(getline(lf, line)){
      SqlEngine::parseLoadLine(line, addkey, addvalue);
      //obtain the record id of the inserted tuple
      int rs_rid = rf.append(addkey, addvalue, rid);
      // if obtain the record id failed, return error code
      if (!rs_rid)
      {
        return RC_FILE_WRITE_FAILED;
      }
      //insert corresponding (key,RecordID) pair to B+ tree index file
      int rs_idx = tree.insert(addkey,rid);
      // if failed, return error code
      if (!rs_idx)
      {
        return RC_FILE_WRITE_FAILED;
      }
   }
    while(getline(lf, line)){
      SqlEngine::parseLoadLine(line, addkey, addvalue);
      rf.append(addkey, addvalue, rid);

    }

  }
  else{
    //if table exists, open it; if not exists, creat it
    rf.open(table + ".tbl", 'w');
    while(getline(lf, line)){
      SqlEngine::parseLoadLine(line, addkey, addvalue);
      rf.append(addkey, addvalue, rid);
    }
  }


  rf.close();
  lf.close();

  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
