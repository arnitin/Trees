#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <time.h>

/* AVL Data Structure */ 
using namespace std;
class node {
public :
  node * left;
  node * right;
  int    heightVal;
  int    key;
  int    val;
  
  node(){
    heightVal = 0;
    left  = NULL;
    right = NULL;
  }

};

int BTreeDeg = 3;
int HashCount = 3;

/* BTree Data Structure */ 
class BTreeNode{
public :
  
  int currKeyCount;
  int * keys;
  int * vals;
  BTreeNode ** ptrs;

  BTreeNode();

  BTreeNode(int val);
};

/*BTreeNode :: BTreeNode(){
  }
*/
/* Constructor */
BTreeNode :: BTreeNode(int val){
  keys = new int[val - 1 ];
  vals = new int[val - 1 ];
  ptrs = new BTreeNode*[val];
}

/* Return height of BTree node */
int height(node * In){ 
  if(In == NULL){
    return -1;
  } else {
    return In -> heightVal;
  }
}
/* Initialize global queue for BFS on BTree*/ 
queue<BTreeNode *> q;

/*AVL Right rotation */
void RightRot(node * &In) {
  node * temp; 
  temp = In -> right;
  In-> right = temp->left;
  temp->left = In;
  In->heightVal = max(height(temp->right),height(temp->left)) + 1;
  temp->heightVal = max(height(temp->right),In->heightVal) + 1;
  In=temp;
  //cout<<"\nRot right\n";

}

/* AVL Left Rotation*/
void LeftRot(node * &In) {
  node * temp; 
  temp=In->left;
  In->left=temp->right;
  temp->right=In;
  In->heightVal=max(height(In->left),height(In->right))+1;
  temp->heightVal=max(height(temp->left),In->heightVal)+1;
  In=temp;
  //cout<<"\nrot left\n";

}

int searchAVL(int key, node* Tree){
  if (Tree == NULL) {
    return 0;
  }
  if(Tree->key == key) {
    return 1;
  } else if(key> Tree->key){
    searchAVL(key,Tree->right);
  } else if (key < Tree -> key){
    searchAVL(key,Tree->left);
  }
}

void BalanceAVL(node *&In, int key, int type){
  if (type == 0) {//left    
    if(key< In->left->key) {
      LeftRot(In);
    } else {
      RightRot(In->left);
      LeftRot(In);
    }
  } else {//right
    if(key> In->right->key)
      RightRot(In);
    else{
      LeftRot(In->right);
      RightRot(In);
    }
  }
}


void insertAVL(node * &In,int key){

  if (In == NULL){
    In = new node;
    In->key = key;
    In->val = 2*key;
    In->left=NULL;
    In->right=NULL;

  } else if (key < In -> key){  
    if(key == 50){
      int yuuu = 0;
    }
    insertAVL(In->left,key);     
    if(height(In->left) - height(In->right) == 2) {
      BalanceAVL(In,key,0);
    }
  } else if(key > In -> key){    
    insertAVL(In->right,key);      
    if(height(In->right) - height(In->left) == 2) {
      BalanceAVL(In,key,1);
    }
  } 
  int r=height(In->right);
  int l= height(In->left);
  In->heightVal = max(l,r)+1; // because height return -1
 
}

void inorder(node *&In, ofstream &file){
  if(In == NULL){
    return;
  }
  inorder(In->left,file);
  file << " " <<In->val;
  inorder(In->right,file);
}


void postorder(node *&In,ofstream &file){
  if(In == NULL){
    return;
  }
  postorder(In->left,file);
  postorder(In->right,file);
  file << " " << In->val;
}


int location(int key, int * keys, int keyCount){
  int i = 0;
  while(i < keyCount && key > keys[i]) {
    i++;
  }
  return i;
}


int whatAction(BTreeNode ** insertNode, int key, int  * splitKey,BTreeNode * &curRoot){
  BTreeNode *temp1,*temp2;
  int currKeyCount,i,split,loc,flag;
  int temp1Key,temp2Key;
  
  if (curRoot == NULL) { // Null at that location insert there
    *insertNode = NULL;
    *splitKey = key;
    return 1;
  }
  currKeyCount = curRoot -> currKeyCount;
  loc = location(key,curRoot->keys,currKeyCount);
  if (loc < currKeyCount && key == curRoot->keys[loc]){
    return 0;
  }

  flag = whatAction(&temp1,key,&temp1Key,curRoot->ptrs[loc]);
 
  if (flag != 1) {
    return flag;
  }
  if (currKeyCount < (BTreeDeg - 1)){ // find loc and insert
    loc = location(temp1Key,curRoot->keys,currKeyCount);
    for(i=currKeyCount ; i> loc ; i--){
      curRoot->keys[i] = curRoot->keys[i-1];
      curRoot->vals[i] = curRoot->vals[i-1];
      curRoot->ptrs[i+1] = curRoot->ptrs[i];

    }
    curRoot->keys[loc]   = temp1Key;
    curRoot->vals[loc]   = temp1Key * 2;
    curRoot->ptrs[loc+1] = temp1;
    curRoot -> currKeyCount += 1;
    return 2; 
  }

  if (loc == (BTreeDeg - 1)){ // copy the data for split
    temp2Key = temp1Key;
    temp2 = temp1;
  } else {
    temp2Key = curRoot->keys[BTreeDeg - 2];
    temp2    = curRoot->ptrs[BTreeDeg - 1];
    for(i=(BTreeDeg - 2) ; i>loc ; i--){
      curRoot->keys[i]   = curRoot->keys[i-1];
      curRoot->vals[i]   = curRoot->vals[i-1];
      curRoot->ptrs[i+1] = curRoot->ptrs[i];
    }
    curRoot->keys[loc] = temp1Key;
    curRoot->vals[loc] = temp1Key * 2;
    curRoot->ptrs[loc + 1] = temp1;
  }

  split = (BTreeDeg - 1)/2;
  *splitKey = curRoot->keys[split];
  (*insertNode) = new BTreeNode(BTreeDeg);
  curRoot->currKeyCount = split;
  (*insertNode)->currKeyCount = BTreeDeg -1 - split;

  for(i=0 ; i < (*insertNode)->currKeyCount ; i++) {// push split data to proper location
    (*insertNode)->ptrs[i] = curRoot->ptrs[i + split + 1];
    if (i<(*insertNode)->currKeyCount -1 ){
      (*insertNode) -> keys[i] = curRoot->keys[i + split + 1];
      (*insertNode) -> vals[i] = curRoot->vals[i + split + 1];
    } else {
      (*insertNode) -> keys[i] = temp2Key;
      (*insertNode) -> vals[i] = temp2Key * 2;
    }
  }
  (*insertNode)->ptrs[(*insertNode)->currKeyCount] = temp2;
  return 1;

}

void BTreeInsert(int key, BTreeNode *&BTreeIn) {
  BTreeNode * insertNode;
  int flag = -1;
  int splitKey;
  flag = whatAction(&insertNode,key,&splitKey,BTreeIn);
  if (flag == 1) { //Nothing : Just insert
    BTreeNode * temp;
    temp = BTreeIn;
    BTreeIn = new BTreeNode(BTreeDeg);
    //    BTree -> ptrs = new BTreeNode[3]);
    BTreeIn->currKeyCount = 1;
    BTreeIn->keys[0] = splitKey;
    BTreeIn->vals[0] = splitKey *2;
    BTreeIn->ptrs[0] = temp;
    BTreeIn->ptrs[1] = insertNode;
  }
}



int BTreeSearch(int key,BTreeNode * Tree){
  int loc,currKeyCount;
  BTreeNode * temp = Tree;
  int i;
  while (temp)
    {
      currKeyCount = temp->currKeyCount;
      /*      for (i=0;i<temp->currKeyCount;i++){
	      }*/
      loc = location(key,temp -> keys, currKeyCount);
      if ((loc < currKeyCount) && key == temp->keys[loc]) {
		return 1; //success!!
      }
      temp = temp->ptrs[loc];
    }
  return 0; // failure
}


void BTreeLevel(BTreeNode * BTree, ofstream & file){
  BTreeNode * temp;
  int         i;

  q.push(BTree);
  while(!q.empty()){   
    temp = q.front(); // Get top of queue
    if(temp != NULL) {
      for (i=0;i<temp->currKeyCount;i++) {
		file << " " << temp->vals[i];
      }  
      for (i=0;i<=temp->currKeyCount;i++) {    
		q.push(temp->ptrs[i]); // insert to queue
      }
    }
    q.pop();
  }
}


void BTreeInorder(BTreeNode * In, ofstream & file)
//void binaryTree::aux_inorder(bTreeNode *&root)
{
  int pos;
    
  if (In) {
    BTreeInorder(In->ptrs[0],file);
        
    for (pos = 1; pos <= In->currKeyCount; pos++) { // do for all keys
      file<<In->vals[pos-1]<<" ";
      BTreeInorder(In->ptrs[pos],file);
    }
  }
}

int main(int argc, char * argv[]){
 
  int         noOfElements;
  int         i = 0;
  int         randVal;
  int         location = -1;
  int         ar[1000000];
  vector<int> Array;
  string      filemode = "-u";
  string      randommode = "-r";
  clock_t     Start, Time;
  ofstream    AVL_inorder;
  ofstream    AVL_postorder;
  ofstream    AVLHash_inorder;
  ofstream    BTree_sorted;
  ofstream    BTree_level;
  ofstream    BTreeHash_level;

  
  if (argc < 3) {
    cout << "Valid Option are : " <<  endl << "-r NumElements HashSize BTreeDegree" << endl << "-u input_file" << endl;
    exit(0);
  }
  
  if (filemode.compare(argv[1]) == 0){    
    /* File Reading */ 
    if (argc < 3) {   
      cout << "Please Specify All Parameters In the following format" << endl << "'-u' input_file" << endl;
      exit(0);
    }
    
    ifstream  infile(argv[2]);
	/* Initialize files trucate everything*/
    AVL_inorder.open("AVL_inorder.out",ios_base::in|ios_base::out|ios_base::trunc);    
    AVL_postorder.open("AVL_postorder.out",ios_base::in|ios_base::out|ios_base::trunc);
    AVLHash_inorder.open("AVLHash_inorder.out",ios_base::in|ios_base::out|ios_base::trunc);    
    BTree_sorted.open("BTree_sorted.out",ios_base::in|ios_base::out|ios_base::trunc);
    BTree_level.open("BTree_level.out",ios_base::in|ios_base::out|ios_base::trunc);    
    BTreeHash_level.open("BTreeHash_level.out",ios_base::in|ios_base::out|ios_base::trunc);
    
    int         key[600];
    int         a;
    int         b;    
    i = 0;
	/* Read from files*/
    infile >> noOfElements;
    while(infile >> a >> b){
      if (ar[a] != 63000){
	ar[a] = 63000;
	Array.push_back(a);
	i++;
      }
    }

    if (i!=noOfElements){
      noOfElements = i;
    }
    
  } else  if (randommode.compare(argv[1]) == 0){
    if (argc < 5) {
      cout << "Please Specify All Parameters In the following format" << endl << "'-r' NumElements HashSize BTreeDegree" << endl;
      return (0);
    }
    noOfElements = atoi(argv[2]);
    HashCount    = atoi(argv[3]);
    BTreeDeg     = atoi(argv[4]);

    for(i=0;i<noOfElements;i++){
      randVal = rand() % 1000000;
      if (ar[randVal] != 63000){ // just making sure that numbers are not repeated
		ar[randVal] = 63000;
		Array.push_back(randVal);
      } else {
		i--;
      }
    }
  }
  /* Initialize trees and hash */
  node *Tree = NULL,*AVLHash[HashCount];
  
  /* Insert to a AVL */
  Start = clock();
  
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    insertAVL(Tree,randVal);      
  }
  
  Time = clock() - Start; 
  cout << Time << " AVL" << endl;
  
  /* Search AVL */
  
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    if (!searchAVL(randVal,Tree)) {
      cout << " Did not find " << randVal;
    }
  }
  
  Time = clock() - Start; 
  cout << Time << " Search AVL" << endl<< endl;
  
  /* Write to files */
  AVL_inorder << "\n";
  inorder(Tree,AVL_inorder);
  AVL_postorder << "\n";
  postorder(Tree,AVL_postorder);

  AVL_inorder.close();
  AVL_postorder.close();

  /* Insert to a AVLHash */ 
  
  for(i=0;i<HashCount;i++){
    AVLHash[i] = NULL;  //Initializing everything to null just in case
  } 
  Start = clock();

  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    location = randVal % HashCount;
    Tree = AVLHash[location];
    insertAVL(AVLHash[location],randVal);
  }
  
  Time = clock() - Start; 
  cout << Time << " AVLHash" << endl; 
  
  /* Write to file */
  for(i=0;i<HashCount;i++){
    AVLHash_inorder << "\n";
    inorder(AVLHash[i],AVLHash_inorder);
  }
  
  AVLHash_inorder.close();
  
  /* Search AVLHash */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    location = randVal % HashCount;    
    if (!searchAVL(randVal,AVLHash[location])) {
      cout << " Did not find " << randVal;
    }      
  }
  
  Time = clock() - Start; 
  cout << Time << " Search AVLHash" << endl<< endl;

  /* Insert to a RB */ 
  map<int, int> RBTree;
  map<int,int>::iterator it;

  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    RBTree.insert(make_pair(randVal, randVal));
  }
  Time = clock() - Start; 
  cout << Time << " RB" << endl; 

  /* Search RB */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    it = RBTree.find(randVal); 
    if (it == RBTree.end( )) {
      cout << " Did not find " << randVal;
    }
  }
  Time = clock() - Start; 
  cout << Time << " RB Search" << endl<< endl; 
  
  /* Insert to a RBHash */ 

  map<int, int> RBTreeHash[HashCount];
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    location = randVal % HashCount;
    RBTreeHash[location].insert(make_pair(randVal, randVal));
  }
  
  Time = clock() - Start; 
  cout << Time << " RBHash" << endl; 

  /* Search RBHash */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    location = randVal % HashCount;
    it = RBTreeHash[location].find(randVal); 
    if (it == RBTreeHash[location].end( )) {
      cout << " Did not find " << randVal;
    }
  }
  Time = clock() - Start; 
  cout << Time << " RBHash Search" << endl<< endl; 

  /* Initialize Btree and corrosponding hash */
  BTreeNode *BTree = NULL,*BTreeHash[HashCount];

  /* Insert to a BTree */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    BTreeInsert(randVal,BTree);
  }
  
  Time = clock() - Start; 
  cout << Time << " BTree" << endl; 

  /* Search BTree */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    if (!BTreeSearch(randVal,BTree)) {
      cout << "Did not find " << randVal << " " << i << "\n";
      break;
    }
  }
  
  Time = clock() - Start; 
  cout << Time << " BTree Search" << endl << endl; 
 
 /* Write level wise */
  BTreeLevel(BTree,BTree_level);
  BTree_level.close();
  /* Write Inorder */
  
  BTreeInorder(BTree,BTree_sorted);
  BTree_sorted.close();
  for(i=0;i<HashCount;i++){
    BTreeHash[i] = NULL; 
  }
  
  /* Insert BTreeHash */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    location = randVal % HashCount;
    BTreeInsert(randVal,BTreeHash[location]);
  }
  
  Time = clock() - Start; 
  cout << Time << " BTreeHash" << endl; 


  /* Search BTreeHash */ 
  Start = clock();
  for(i=0;i<noOfElements;i++){
    randVal  = Array[i];
    location = randVal % HashCount;
    if (!BTreeSearch(randVal,BTreeHash[location])) {
      cout << "Did not find " << randVal << " " << i << "\n";
      // break;
    }
  }
  Time = clock() - Start; 
  cout << Time << " BTreeHash Search" << endl; 

  /* Write LevelWise */
  for(i=0;i<HashCount;i++){
    BTreeLevel(BTreeHash[i],BTreeHash_level);
    BTreeHash_level << "\n";
  }
  BTreeHash_level.close();
}
