#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
using namespace std;

/*
 * change buckets to 256 instead of 10
 * invert to ascii and use as hashindex
 * change printing format
 * print from hashtable to out file
 */

class ListNode {
	friend class LLStack;
	friend class LLQ;
	friend class RadixSort;

public:
	string data;
	ListNode *next;

	ListNode(string newData){
		data = newData;
		next = NULL;
	}
};

class LLStack {
// top to return top, push to insert to top, pop to delete from top, isEmpty to check if stack is empty and print
	friend class RadixSort;
public:
	ListNode *top;

	LLStack(){
		top = NULL;
	}

	ListNode* getTop() { //returns the top of the stack
		if(isEmpty()){
			cout << "top is NULL\n";

			return NULL;
		}

		return top;
	}

	void push(ListNode *nNode){ // insert newNode to top of stack S
		nNode->next = top;
		top = nNode;
	}

	ListNode* pop(){ // delete from top of stack
		if(isEmpty()){
			cout << "can't pop, top is NULL" << endl;
			return NULL;
		}

		ListNode *temp = top;
		top = top->next;
		temp->next = NULL;

		return temp;
	}

	bool isEmpty(){
		return top == NULL;
	}

	void print(ofstream &out){
		out << "*** Below is the output of the stack***" << endl;
		if(isEmpty()){
			out << "top is NULL, nothing to print\n";
			return;
		}
		ListNode *curr = top;

		out << "Top->";
		while(curr->next != NULL){
			out << "(" << curr->data << ", " << curr->next->data << ")->";
			curr = curr->next;
		}
		out << "(" << curr->data << ", NULL)->NULL\n\n";
	}

};

class LLQ {
// addQ(nN) insert after tail, ListNode deleteQ() deletes and returns deleted node, bool isEmpty(), print()
	friend class RadixSort;
public:
	ListNode *head;
	ListNode *tail;
	ListNode *dummy;

	LLQ(){
		dummy = new ListNode("9999");
		head  = tail = dummy;
	}

	void addQ(ListNode *nNode){
		tail->next = nNode;
		tail = nNode;

		// don't touch head cause it's already pointing to dummy
	}

	ListNode* deleteQ(){
		ListNode *temp = head->next; // node to be returned and deleted

		if(isEmpty()){
			cout << "empty queue" << endl;
			return NULL;
		}

		head->next = head->next->next; // make head->next point to next in queue
		temp->next = NULL; // detach from queue reference

		if(head->next == NULL){
			tail = dummy;
		} // to update tail if we are deleting the last node in queue

		return temp;
	}

	bool isEmpty(){
		return tail->next == head->next && head->next == NULL; // dummy->next should always = NULL if empty queue
	}

	void print(int currTable, int index, ofstream &out){ // print from dummy head to tail
//		cout << "*** Below is the output of the queue at index " << index << " ***\n";
		if(isEmpty() || head->next == NULL){
			out << "Table[" << currTable << "][" << index << "]: " << head->data << ",NULL)->NULL\n";
			return;
		}
		ListNode *curr = head;

		out << "Table[" << currTable << "][" << index << "]: ";
		while(curr->next != NULL){
			out << "(" << curr->data << ", " << curr->next->data << ")->";
			curr = curr->next;
		}
		out << "(" << curr->data << ", NULL)->NULL\n";
//		cout << "Tail(" << index << ")->(" << tail->data << ", NULL)->NULL\n";
	}

};

class RadixSort {
// fread(done), getMax(done),
public:
	const static int tableSize = 256;
	string data;
	int currTable;
	int prevTable;
	int lngstStrLen; // LENGTH of the max digit
	int currPos; // index of current place
	LLQ hashTable[2][tableSize];

	RadixSort(){
		data = "";
		currTable = 0;
		prevTable = 0;
		lngstStrLen = 0;
		currPos = 0;
	}

/* 1st read:
 * read input from file
 * compare if lower than minNum
 * if input is lower than minNum update minNum
 * if input is greater than maxNum update maxNum
 * repeat step 1 to step 4 until eof and set radix sort's offset to lowest number from file and set radix sort's maxNum
 *
 * (close open file)
 *
 * loadStack:
 * read from input
 * put in new node and push to stack the input + abs value of offset
 * repeat step 1 to step 2 until eof
 * print the stack
 *
 * dumpStack:
 * pop from stack
 *
 */


	void firstReading(ifstream &in, ofstream &out){
		string newStr = "";
		string lngStr = "";
		// compare string length for current and next string until eof to find longest..

//		out << "unsorted:\n";
		while(in >> newStr){
			if(newStr.length() > lngStr.length()){
				lngStr = newStr;
			}
//			out << newStr << ' ';
		}
		lngstStrLen = lngStr.length();

//		out << endl;
//		maxDigitLength = getMaxDigitLength(posNum+offSet); // cout << "offSet: " << offSet << endl << "maxDigitLength: " << maxDigitLength << endl;
	} // firstReading()

	string paddedData(string data){
		int padLen = lngstStrLen - data.length();

		for(int i=0; i<padLen; i++){
			data += (char)0;
		}
//		data.substr(padLen, how long the string is originally); // to undo padded

		return data;
	}

	void loadStack(ifstream &in, ofstream &out1, ofstream &out2){
		string data = "";
		LLStack *newStack = new LLStack();

		while(in >> data){
			newStack->push(new ListNode(paddedData(data)));
		}

		newStack->print(out2);
		sort(newStack, out1, out2);

	} // loadStack()

	void dumpStack(LLStack *stack, ofstream &out){ // uses currDigit, currTable
		// pop from stack to get data
		// pass data to getCurrDigit() to get nth place
		// use the returned value to hash to index
		// add to the tail of that index (or enqueue to hashTable[currTable][hashIndex])
		// repeat till stack empty
		// print table (hashTable[currTable]) // don't print nulls

		while(!stack->isEmpty()){
			ListNode *nNode = stack->pop();
			int hashIndex = getCurrStr(nNode->data); // pass popped data to getCurrDigit()
			hashTable[currTable][hashIndex].addQ(nNode);
		} // end in 1

		printTable(out);

//		cout << getCurrDigit(1996);
	}

	void sort(LLStack *stack, ofstream &out1, ofstream &out2){ // like the main of RadixSort
		currPos = 0; // controls the nth place of which hashing
		currTable = 0; // currTable initialize

		dumpStack(stack, out2); // transfering from stack to queue
		// currDig++, set currTab = 1, set prevTab = 0, (currQ is an iterator)set currQ = 0
		// move data from prevTab based on 'nth (2nd) place' to currTab sequentially
		// outer outer iterator
			// outer iterator
				// inner iterator
				// dequeue starting index 0
				// hash index <- getCurrDigit(node->data)
				// enqueue to currTable based on hashed index
//		for(int i=1; i<4; i++){
//			currTable = i%2;
//			prevTable = (currTable+1) % 2;
//			cout << currTable << ' ' << prevTable << endl;
//		}

		for(currPos = 1; currPos < lngstStrLen; currPos++){
			currTable = currPos%2; // INITIALIZE CURRTABLE
			prevTable = (currTable+1) % 2; // INITIALIZE PREVTABLE
			for(int currQ=0; currQ<tableSize; currQ++){
				while(!hashTable[prevTable][currQ].isEmpty()){
					ListNode *nNode = hashTable[prevTable][currQ].deleteQ(); // dequeue
					int hashIndex = getCurrStr(nNode->data); // get data from returned node and getCurrDigit() for hashIndex

					hashTable[currTable][hashIndex].addQ(nNode); // add to next queue based on hashing
				} // transitions from prevQ to currQ based in nth place
			} // updates the hashTable index
			printTable(out2);
		} // changes to the current nth position from right to left

		out1 << "sorted:\n";
		for(int i=0; i<tableSize; i++){
			while(!hashTable[currTable][i].isEmpty()){
				out1 << hashTable[currTable][i].deleteQ()->data << endl;
//				out1 << hashTable[currTable][i].deleteQ()->data-offSet << ' ';
			}
		} // dequeue from the currTable in index order, add the offSet and print to text file

	} // sort()

	int getCurrStr(string str){ // gets the number in the nth position from right to left
		return str.at((str.length()-1) - currPos); // returning a char.. but since it is an int function it converts to ASCII
//		return to_string(number).at(to_string(number).length() - 1) - '0';
//		return to_string(number).at(currDigit) - '0'; // since it converts to ASCII we look at the table and sub the offSet
	} // getCurrDigit(you could take the maxNum.len() and subtracting until it reaches 0?)

	int getMaxDigitLength(int maxNum){
		return to_string(maxNum).length();
	} // getMaxDigitLength()

	void printTable(ofstream &out){
		out << "***Print Table***" << endl;

		for(int i=0; i<tableSize; i++){
			if(!hashTable[currTable][i].isEmpty()){
				hashTable[currTable][i].print(currTable, i, out);
			}
		}

		out << endl;
	} // printTable()

//	Methods: firstRead(if, int, int), loadStack(if, top), dumpStack(top), tableIndex(index of given table),
//	getMaxDigitLength(int), getCurrDigit(return current digit in node), hashIndex(digit)???, constructor(create two hashTables),
//	printTable(print none empty queue (in table?))
};

int main(int argc, char** argv){
	ifstream in1(argv[1]); // non negs
	ofstream out1(argv[2]); // sorted
	ofstream out2(argv[3]); // debug
//	RadixSort *rad = new RadixSort();
	//	rad->firstReading(in1, out1);
	//	in1.close(); // reset file to...
	//	in1.open(argv[1]); // read from start
	//	rad->loadStack(in1, out1, out2); // LOAD STACK GOES STRAIGHT INTO SORTING

	RadixSort *rad = new RadixSort();

	rad->firstReading(in1, out2);
	in1.close();
	in1.open(argv[1]);
	rad->loadStack(in1, out1, out2);

	in1.close();
	out1.close();
	out2.close();

	return 0;
}
/*
 * add padding sort then remove padding
 */
