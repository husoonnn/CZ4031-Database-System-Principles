#include "BPTree.h"
#include "types.h"

#include <vector>
#include <cstring>
#include <iostream>

int BPTlevel = 1;

using namespace std;

/*INSERTING A RECORD INTO A NODE:
1. Run search operation to find appropriate place to insert
2. Insert the new keys at the proper location, 
  but if the node has a maximum number of keyss already:
3. The node, along with a newly inserted keys, will split from the middle element.
4. The middle element will become the parent for the other two child nodes.
5. The nodes must re-arrange keyss in ascending order.
*/

// // Insert a record into the B+ Tree index. Key: Record's numVotes, Value: {blockAddress, offset}.
// void BPTree::insert(Address address, int keys) //Mtd of BPlusTree 
// {
//   //Check if root node exists
//   if (rootAddress == nullptr){

//     //Initialise node to store incoming new record
//     root = new Node(maxKeys); //Create new node 
//     root->keyss[0] = keys; //Insert new keys into new array of keyss in new node
//     root->isLeaf = true;
//     root->numKeys = 1; //New keys inserted into new node thus total = 1 keys
//   }
//   else{
//     if (root->numKeys < maxKeys) {
//       //insert into block
//     }
//     else{
//       //else split two blocks with half of each keyss
//     }
    
//     //check bool balancing();


//   }
// }

// Insert Operation
void BPTree::insert(Address address,int key) {

  //If empty tree, create new node
  if (root == NULL) {
    root = new Node(maxKeys);
    numNodes += 1;
    root->storagepointer[0] = address; //link index key to storage
    root->keys[0] = key; //Insert new key into new array of keys in new node
    root->isLeaf = true;
    root->numKeys = 1; //New key inserted into new node thus total = 1 key
  //If not empty - cursor pt to root block - conduct search top down
  } else {
    Node *cursor = root;
    Node *parent;
    while (cursor->isLeaf == false) { //cursor is at non leaf node
      parent = cursor; //original starting position of parent node

      //iterate through records in the block
      for (int i = 0; i < cursor->numKeys; i++) { 
        if (key < cursor->keys[i]) { //comparison of keys
          cursor = cursor->pointers[i]; //cursor traverse to left block
          break;
        }
        if (i == cursor->numKeys - 1) {
          cursor = cursor->pointers[i+1]; //cursor traverse to right block
          break;
        }
      }
    }
    if (cursor->numKeys < maxKeys) { //if block still have space 
      int i = 0;

      while (key > cursor->keys[i] && i < cursor->numKeys){ //on same block, compare new key with key in the block & i < total numKeys
        i++; //pt of insertion
      }

      for (int j = cursor->numKeys; j > i; j--) {
        cursor->keys[j] = cursor->keys[j - 1]; //shifting keys, larger than new key, right by 1 position
        cursor->storagepointer[j] = cursor->storagepointer[j-1];
      }

      cursor->keys[i] = key; //insert key i th position
      cursor->storagepointer[i] = address;
      cursor->numKeys++; //update numKeys
      cursor->pointers[cursor->numKeys] = cursor->pointers[cursor->numKeys-1]; //shifting pointers
      cursor->pointers[cursor->numKeys-1]=NULL;
    
    }else{ //if block no space
      Node *newLeaf = new Node(maxKeys); //create new node
      numNodes += 1;
      if (cursor->isLeaf){
        if (cursor->leafLinkPointer == NULL){
          cursor->leafLinkPointer = newLeaf;
        }
        else {
          void* temp = cursor->leafLinkPointer;
          newLeaf->leafLinkPointer = temp;
          cursor->leafLinkPointer = newLeaf;
        }
      }
      int virtualNode[maxKeys+1]; //KIVKIVvivivivii
      Address virtualStorage[maxKeys+1];

      for (int i = 0; i < maxKeys; i++) { //take in (maxKeys keys + 1) keys and store in vir node
        virtualNode[i] = cursor->keys[i];
        virtualStorage[i] = cursor->storagepointer[i];
      }

      int i = 0;
      int j = 0;

      while (key > virtualNode[i] && i < maxKeys){ //finding position to slot in new key
        i++;
      }
      for (int j = maxKeys; j > i; j--) { //moving keys that are larger 
        virtualNode[j] = virtualNode[j - 1];
        virtualStorage[j] = virtualStorage[j-1];
      }


      virtualNode[i] = key;  //insert new key into position i
      virtualStorage[i] = address;
      newLeaf->isLeaf = true;
      cursor->numKeys = (maxKeys + 1) / 2; //splitting keys into 2 nodes 
      newLeaf->numKeys = maxKeys + 1 - (maxKeys + 1) / 2; //newleaf should contain behind half nodes 

      cursor->pointers[cursor->numKeys] = newLeaf; //pointing to first pointer of new leaf node 
      newLeaf->pointers[newLeaf->numKeys] = cursor->pointers[maxKeys]; //
      cursor->pointers[maxKeys] = NULL; 

      //rearranging the 2 leaf nodes 
      for (i = 0; i < cursor->numKeys; i++) {
        cursor->keys[i] = virtualNode[i];
        cursor->storagepointer[i] = virtualStorage[i];
      }
      for (i = 0, j = cursor->numKeys; i < newLeaf->numKeys; i++, j++) {
        newLeaf->keys[i] = virtualNode[j];
        newLeaf->storagepointer[i] = virtualStorage[i];
      }

      //KIVIVIVIVIV
      if (cursor == root) {
        BPTlevel += 1;
        Node *newRoot = new Node(maxKeys);
        numNodes += 1;
        newRoot->keys[0] = newLeaf->keys[0];
        newRoot->pointers[0] = cursor;
        newRoot->pointers[1] = newLeaf;
        newRoot->storagepointer = NULL;
        newRoot->isLeaf = false;
        newRoot->numKeys = 1;
        root = newRoot;
        // std::cout<<"root "<<root<<endl; 
        
      } else {
        insertInternal(newLeaf->keys[0], parent, newLeaf, address);
      }
      height = BPTlevel;
      
    }
  }
  
  // std::cout<<"key "<<key<<endl; 
}

//inserting a key into the non-leaf node of the tree
void BPTree::insertInternal(int key, Node *cursor, Node *child, Address address) {
  //if got space for insertion
  if (cursor->numKeys < maxKeys) {
    int i = 0;
    while (key > cursor->keys[i] && i < cursor->numKeys)//traverse block
      i++;
    for (int j = cursor->numKeys; j > i; j--) { //shifting keys larger than key to the right
      cursor->keys[j] = cursor->keys[j - 1];
    }
    for (int j = cursor->numKeys + 1; j > i + 1; j--) { //shifting pointers to the right 
      cursor->pointers[j] = cursor->pointers[j - 1];
    }
    cursor->keys[i] = key; //key inserted 
    cursor->numKeys++;
    //assign empty pointer to new child node 
    cursor->pointers[i + 1] = child; //KIV
  }
   else { //if parent node is already full, go through split using virtual array block
    Node *newInternal = new Node(maxKeys);
    numNodes += 1;
    int virtualKey[maxKeys + 1];
    Node* virtualPtr[maxKeys + 2];
    for (int i = 0; i < maxKeys; i++) { //adding current keys to virtualkey array
      virtualKey[i] = cursor->keys[i];
    }
    for (int i = 0; i < maxKeys + 1; i++) {
      virtualPtr[i] = cursor->pointers[i]; //adding current pointers to virtual array
    }
    int i = 0, j;
    while (key > virtualKey[i] && i < maxKeys) //traversing virtual array to insert key
      i++; 
    for (int j = maxKeys + 1; j > i; j--) { //shifting keys greater than key to the right
      virtualKey[j] = virtualKey[j - 1];
    }
    virtualKey[i] = key; //insert key
    for (int j = maxKeys + 2; j > i + 1; j--) {  //shifting pointers to the right
      virtualPtr[j] = virtualPtr[j - 1];
    }
    virtualPtr[i + 1] = child; //assign pointer to new child node
    newInternal->isLeaf = false; //non-leaf
    cursor->numKeys = (maxKeys + 1) / 2; //splitting parent node into two child nodes
    newInternal->numKeys = maxKeys - (maxKeys + 1) / 2; 

    // inserting keys into new node from virtual node
    for (i = 0, j = cursor->numKeys + 1; i < newInternal->numKeys; i++, j++) {
      newInternal->keys[i] = virtualKey[j];
    }
    
    // inserting pointers into new node from virtual node
    for (i = 0, j = cursor->numKeys + 1; i < newInternal->numKeys + 1; i++, j++) {
      newInternal->pointers[i] = virtualPtr[j];
    }
    if (cursor == root) {
      BPTlevel += 1;
      Node *newRoot = new Node(maxKeys);
      numNodes += 1;
      newRoot->keys[0] = cursor->keys[cursor->numKeys];
      newRoot->pointers[0] = cursor; //point to left child
      newRoot->pointers[1] = newInternal; //point to right child
      newRoot->isLeaf = false;
      newRoot->numKeys = 1;
      root = newRoot;
    } else {
      insertInternal(cursor->keys[cursor->numKeys], findParent(root, cursor), newInternal, address);
    }
  }
}

// Find the parent
Node *BPTree::findParent(Node *cursor, Node *child) {
  Node *parent;
  //if parent or child is leaf
  if (cursor->isLeaf || (cursor->pointers[0])->isLeaf) { //KIVIVIVIVIIVIV
    return NULL;
  }
  //traverse node to find the parent using recursive function
  for (int i = 0; i < cursor->numKeys + 1; i++) {
    if (cursor->pointers[i] == child) { //comparing addresses of parent and child
      parent = cursor;
      return parent;
    } else {
      parent = findParent(cursor->pointers[i], child);
      if (parent != NULL)
        return parent;
    }
  }
  return parent;
}

// Print the tree
void BPTree::display(Node *cursor) {
  if (cursor != NULL) {
    for (int i = 0; i < cursor->numKeys; i++) {
      cout << cursor->keys[i] << " | ";
    }
    if (cursor->numKeys == 1){
      cout << "X" << " | " << "X" << " | ";
    }
    else if (cursor->numKeys == 2){
      cout << "X" << " | ";
    }
    cout << "\n";
    if (cursor->isLeaf != true) {
      for (int i = 0; i < cursor->numKeys + 1; i++) {
        display(cursor->pointers[i]);
      }
    }
    //cout << "Tree level: " << BPTlevel <<endl;
  }
}

// Print the tree
void BPTree::LLdisplay(Node *cursor) {
  if (cursor->isLeaf == true){
    while (cursor->leafLinkPointer != NULL){
      for (int i = 0; i < cursor->numKeys; i++) {
        std::cout<<cursor->storagepointer[i].blockAddress<<endl;
      }
      cursor = (Node *)cursor->leafLinkPointer;
    }
    for (int j=0; j<cursor->numKeys; j++){
      std::cout<<cursor->storagepointer[j].blockAddress<<endl;
    }
  }
  else{
    LLdisplay(cursor->pointers[0]);
  }
}

// Get the root
Node *BPTree::getRoot(){
  return root;
}

int BPTree::getnumNodes(){
  return numNodes;
}

int BPTree::getHeight(){
  return height;
}

// bool BPTree::search(Node* cursor, int lowerboundkey, int upperboundkey){

//   bool search_result;
//   bool search_check=false;

//   // if leaf nodes not reach, recursively traverse the b+ tree
//   if (cursor->isLeaf == false){
//     for(int i=0; i < cursor->numKeys + 1; i++){
//       search_result = search(cursor->pointers[i],lowerboundkey,upperboundkey);
//       if (search_result == true){
//         search_check = true;
//       }
//     }
//     if (search_check == true){
//       for (int l=0; l < cursor->numKeys; l++){
//         std::cout<<cursor->keys[l]<<" ";
//       }
//       std::cout<<endl;
//       return true;
//     }
//     else {
//       return false;
//     }
//   }
//   //leaf nodes reached, compare key values to lower and upper bounds
//   //if leaf node in range, return the path taken from the root node
//   else{
//     bool check_found = false;

//     for(int j=0; j < cursor->numKeys; j++){
//       if (cursor->keys[j] >= lowerboundkey && cursor->keys[j] <= upperboundkey){
//         check_found = true;
//         std::cout<<"Key found within range: "<<'\t'<<cursor->keys[j]<<endl;
//         std::cout<<"Tconst of the key found: "<<'\t'<<(char *)(cursor->storagepointer[j].blockAddress)<<endl;
//         //displayBlock(cursor->storagepointer[j].blockAddress, cursor->keys[j]);
//       }
//     }
//     if (check_found == true){
//       return true;
//     }
//     else{
//       return false;
//     }
//   }
// }

void BPTree::search(Node *node, int lowerboundkey, int upperboundkey)
{

  int i = 0;
  static bool printcheck = false;
  static int recursiondepth = 0;
  static int newrecursiondepth = 0;

  recursiondepth += 1;
  newrecursiondepth = recursiondepth;
  if (node->isLeaf != true) { //if not leaf node
      for (int i = 0; i < node->numKeys + 1; i++) {
        search(node->pointers[i], lowerboundkey, upperboundkey);
        recursiondepth -= 1;
      }
      if (printcheck == true && recursiondepth < newrecursiondepth){
        printKeys(node);
        printcheck = false;
      }
      if (recursiondepth == 1){
        printKeys(node);
      }
  }
  for (int i = 0; i < node->numKeys; i++)
  { 
    if (node->keys[i] >= lowerboundkey && node->keys[i] <= upperboundkey){
      if (node->isLeaf == true){
        displayBlock(node->storagepointer[i].blockAddress, node->keys[i]);
      }
      printKeys(node);
      printcheck = true;
    }
  }

}

void BPTree::printKeys(Node* cursor){
  std::cout<<"Keys accessed in node: "<<endl;
  for (int i=0; i < cursor->numKeys; i++){
    std::cout<<cursor->keys[i]<<" ";
  }
  std::cout<<'\n';
}

void BPTree::displayKeys(Node *node){
  for(int i=0; i<node->numKeys; i++){
    std::cout<<node->keys[i]<<'\t';
  }
  std::cout<<endl;
}

void BPTree::displayNode(Node *node)
{

  int i = 0;
  std::cout << "|";
  for (int i = 0; i < node->numKeys; i++)
  {
    std::cout << node->keys[i] << " | ";
  }

  
  for (int i = node->numKeys; i < maxKeys; i++)
  {
    std::cout << " x |";      // Remaining empty keys
  }
  cout << "\n";
  if (node->isLeaf != true) { //if not leaf node
      for (int i = 0; i < node->numKeys + 1; i++) {
        display(node->pointers[i]);
      }
  }

}

void BPTree::displayBlock(void *blockAddress,int targetvalue)
{
  // Load block into memory
  void *block = operator new(nodeSize);
  std::memcpy(block, blockAddress, nodeSize);

  unsigned char testBlock[nodeSize];
  memset(testBlock, '\0', nodeSize);

  //Block is empty.
  if (memcmp(testBlock, block, nodeSize) == 0)
  {
    std::cout << "Empty block!" << '\n';
    return;
  }

  unsigned char *blockChar = (unsigned char *)block;

  int i = 0;
  int numrecords = 0;
  float totalratings = 0;
  while (i < nodeSize)
  {
    // Load each record
    void *recordAddress = operator new(sizeof(Record));
    std::memcpy(recordAddress, blockChar, sizeof(Record));

    Record *record = (Record *)recordAddress;

    if (record->numVotes == targetvalue){
      numrecords += 1;
      std::cout <<"Record: "<<"[" << record->tconst << "|" << record->averageRating << "|" << record->numVotes << "]";
      totalratings += record->averageRating;
    }
    
    blockChar += sizeof(Record);
    i += sizeof(Record);
  }

  cout << "\n";
  std::cout<<"Total number of records: "<<numrecords<<endl;
  std::cout<<"Total average ratings: "<<totalratings<<endl;
  std::cout<<"Average of average ratings: "<<totalratings/numrecords<<endl;
  std::cout<<endl;
}