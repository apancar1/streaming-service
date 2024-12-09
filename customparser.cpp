#include "customparser.h"
#include <iostream>
#include <sstream>

using namespace std;

const char* error_msg_1 = "Cannot read integer n";
const char* error_msg_2 = "Error in content parsing";
const char* error_msg_3 = "Error in user parsing";

void trim(string& str){
  // function that trims whitespace 
  size_t first = str.find_first_not_of(" \n\t\r");
  // find letters 
  if (first == string::npos){
    str = "";
    return; 
  }
  // repeat 
  size_t last = str.find_last_not_of(" \n\t\r");
  if (last == string::npos){
    str = "";
    return; 
  }
  str = str.substr(first, last - first + 1);
  // create new substring 
}

void CustomFormatParser::parse(std::istream& is, std::vector<Content*>& content, std::vector<User*>& users)
{
    // Erase any old contents
    content.clear();
    users.clear();

    string line; 
    int count =0; 
    // declare variables 
    if (!(is >> count)){
      throw ParserError(error_msg_1);
      // check if can read 
    }
    for (int i = 0; i < count; i++){
      // loop through items 
      int id = -1; 
      int type = -1; 
      is >> id >> type; 
      // read in id and type 
      if (id == -1 || type == -1){
        throw ParserError(error_msg_2);
        // check if did 
      }
      string contentName;
      is >> contentName; 
      getline(is, line);
      trim(line);
      contentName = contentName + " " + line; 
      // get the name, trim it, create it without 
      int reviews = -1; 
      int stars = -1; 
      int rating = -1; 
      // declare next set of variables 
      if (!(is >> reviews)){
        throw ParserError(error_msg_2);
        // try to read 
      }
      if (!(is >> stars)){
        throw ParserError(error_msg_2);
        // try to read 
      }
      if (!(is >> rating)){
        throw ParserError(error_msg_2);
        // try to read 
      }
      if (type == 1){
        // if it is a series, then need episodes 
        int episodes = -1; 
        if (!(is >> ws >> episodes)){
          throw ParserError(error_msg_2);
          // try to read 
        }
        // same as above, but just for episodes 
        content.push_back(new Series(id, contentName, reviews, stars, rating, episodes));
        // add to content 
      }
      else{
        // if it is movie object, nothing extra is necessary 
        content.push_back(new Movie(id, contentName, reviews, stars, rating));
        // just add to content 
      }
        getline(is,line);
        getline(is,line);
        // get next lines 
        stringstream viewerline(line);
        string viewer; 
        // line and individual variables 
        while (viewerline >> viewer){
          content.back()->addViewer(viewer);
          // add to content for each viewer in line 
          // go through the viewers now 
        }
      // add the content to the vector 
    }
    is.clear(); 
    // clear to start user parsing now 
    while (!is.eof()){
      // continue while the file has not reached to end 
      if (is.fail()){
        throw ParserError(error_msg_3);
        // check 
      }
      string curuser; 
      // variable for user 
      if (!(is >> curuser)){
        break; 
        // if can read 
      }
      int ratinglim; 
      // limit variable 
      if (!(is >> ratinglim)){
        throw ParserError(error_msg_3);
        // if red 
      }
      User* newuser = new User(curuser, ratinglim);
      // create new object 
      // will be same as above but for users now! 
      getline(is, line);
      getline(is,line);
      // read in 
      stringstream history(line);
      // get line 
      if (is.fail()){
        throw ParserError(error_msg_3);
        // check 
      }
      int viewed; 
      // variable for in line 
      while (history >> viewed){
        newuser->addToHistory(viewed);
        // add content in line 
      }
      users.push_back(newuser);
      // add to users 
    }
}
