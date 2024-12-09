#include "strmsrv.h"
#include <iostream>
#include <fstream>
using namespace std;

StreamService::StreamService()
{
  // Update as needed
  cUser_ = nullptr; 
  // declare as null, this is the only thing necessary as the rest are vectors 
}

StreamService::~StreamService()
{
  // need to deallocate for users and content since they were created with new 
  for (size_t i = 0; i < users_.size(); i++){
    delete users_[i];
    // loop through and delete 
  }
  for (size_t i = 0; i < content_.size(); i++){
    delete content_[i]; 
  }
}

void StreamService::readAndParseData(std::istream& is, Parser& p)
{
    p.parse(is, content_, users_);
    cout << "Read " << content_.size() << " content items." << endl;
    cout << "Read " << users_.size() << " users." << endl;
}

void StreamService::userLogin(const std::string& uname)
{
  if (cUser_ != nullptr){
    throw std::runtime_error("Need to logout first"); 
    // will not be nullptr if someone is alr logged in 
  }
  // need to get the user index by name, check if it is -1 bc that means the user doesn't exist 
  int UserIndex = getUserIndexByName(uname);
  if (UserIndex == -1){
    throw std::invalid_argument("Username not valid");
  }
  cUser_ = users_[UserIndex];
  // set the cUser to user if it has not thrown out error 
}

void StreamService::userLogout()
{
  cUser_ = nullptr; 
  // set to nullptr as that is default and what the login checks for 
}

std::vector<CID_T> StreamService::searchContent(const std::string& partial) const
{
    std::vector<CID_T> results;
    for(size_t i = 0; i < content_.size(); i++){
        // Push back when the string 'partial'
        //  is contained in the name of the current content. Lookup the string
        //  class documentation to find an appropriate function that can be used
        //  to do this simply.
        if((partial == "*") || (content_[i]->name().find(partial) != std::string::npos)){
          // added an or to the if statement that checks if parrial is contained 
            results.push_back(i);
        }        
    }
    return results;
}

std::vector<CID_T> StreamService::getUserHistory() const
{
    throwIfNoCurrentUser();
    return cUser_->history;
}

void StreamService::watch(CID_T contentID)
{
  throwIfNoCurrentUser(); 
  // throws if no user is currently logged in 
  if (!isValidContentID(contentID)){
    throw std::range_error("contentID is not valid");
    // error if the content id is not valid 
  }
  Content* cur = content_[contentID];
  // create content object 
  if (cur->rating() > cUser_->ratingLimit){
    throw RatingLimitError("content rating is above the User's rating limit");
    // check if user's rating is appropriate
  }
  cUser_->history.push_back(contentID);
  // add contentID to the history 
  cur->addViewer(cUser_->uname); 
  // add a viewer to the list 
}

void StreamService::reviewShow(CID_T contentID, int numStars)
{
  throwIfNoCurrentUser(); 
  // throws if there is no user logged in 
  if(!isValidContentID(contentID)){
    throw ReviewRangeError("contentID is not valid");
    // check for valid content ID 
  }
  if(numStars > 5 || numStars < 0){
    throw ReviewRangeError("number of stars out of range");
    // check if stars are in range 
  }
  Content* cur = content_[contentID];
  // create object 
  cur->review(numStars); 
  // add the stars to the review 
}

CID_T StreamService::suggestBestSimilarContent(CID_T contentID) const
{
  throwIfNoCurrentUser();
  // first checkpoint 
  if (!isValidContentID(contentID)){
    throw invalid_argument("Content ID not valid");
    // check if calid content id 
  }
  const vector<string>& viewers = content_[contentID]->getViewers();
  // create vector 
  if (viewers.empty()){
    return -1; 
    // check if empty 
  }
  vector<int> count(content_.size(), 0);
  // create variable to keep track 
  for (size_t i = 0; i < viewers.size(); i++){
    // loop through viewers 
    int ind = getUserIndexByName(viewers[i]);
    // get name and get index from that 
    if (ind == -1){
      continue; 
      // check if -1 
    }
    const vector<int>& history = users_[ind]->history;
    // history update 
    for (size_t j = 0; j < history.size(); j++){
      int watched = history[j];
      bool didwatch = false; 
      for (size_t k = 0; k < cUser_->history.size(); k++){
        if (cUser_->history[k] == watched){
          didwatch = true; 
          break; 
        }
      }
      if (!didwatch && watched != contentID){
        count[watched]++;
        // loop through and see if watched 
      }
    }
  }
  CID_T suggested = -1; 
  int max = 0; 
  // variables to determine the suggested 
  for (size_t i = 0; i < count.size(); i++){
    if (count[i] > max){
      max = count[i]; 
      suggested = static_cast<CID_T>(i);
      // loop through and see if there is a better option based on views 
    }
  }
  return suggested; 
  // return result 
}

void StreamService::displayContentInfo(CID_T contentID) const
{
    if(! isValidContentID(contentID)){
        throw std::invalid_argument("Watch: invalid contentID");
    }

}


bool StreamService::isValidContentID(CID_T contentID) const
{
    return (contentID >= 0) && (contentID < (int) content_.size());
}


void StreamService::throwIfNoCurrentUser() const
{
    if(cUser_ == NULL){
        throw UserNotLoggedInError("No user is logged in");
    }
}

int StreamService::getUserIndexByName(const std::string& uname) const
{
    for(size_t i = 0; i < users_.size(); i++){
        if(uname == users_[i]->uname) {
            return (int)i;
        }
    }
    return -1;
}
