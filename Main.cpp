#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Node;
class NormalEdge {
public:
    int startCharacterIndex;
	int endCharacterIndex;
	Node *startNode;
	Node *endNode;

    NormalEdge(){}

    NormalEdge(int _startCharacterIndex, int _endCharacterIndex, Node *_startNode, Node *_endNode)
	{
		this->startCharacterIndex=_startCharacterIndex;
		this->endCharacterIndex=_endCharacterIndex;
		this->startNode=_startNode;
		this->endNode=_endNode;
	}
};

//The suffix link
class SuffixEdge {
public:
    Node *startNode;
	Node *endNode;

	SuffixEdge(){}

	SuffixEdge(Node *_startNode, Node *_endNode)
	{
		this->startNode=_startNode;
		this->endNode=_endNode;
	}


};

class Node {
public:
    int number;
	SuffixEdge *suffixEdge;
	vector<NormalEdge> edges;
    Node() {}

    Node(int _number){
        this->number = _number;
		this->suffixEdge=NULL;
    }

	void RemoveEdge(NormalEdge *e){
		int index=-1;
		int fs=NULL;
		if(e->startNode!=NULL){
			fs=e->startNode->number;
		}
		int fe=NULL;
		if(e->endNode!=NULL){
			fe=e->endNode->number;
		}
		for(vector<NormalEdge>::iterator it = edges.begin(); it != edges.end(); ++it) {
			NormalEdge *edgeTmp=&(*it);
			int ss=NULL;
			if(edgeTmp->startNode!=NULL){
				ss=edgeTmp->startNode->number;
			}
			int se=NULL;
			if(edgeTmp->endNode!=NULL){
				se=edgeTmp->endNode->number;
			}
			if(fs==ss && fe==se){
				edges.erase(it);
				break;
			}
		}
	}
};

class SuffixTree {
private:
    Node *root;
	int nodeNumber;
	int currentPosition;
	string inputString;
	Triple *activePoint;
	int remainder;
	stringstream graphvizOutput;   //Output is in dot notation (gaphviz)
	int startCharacterIndexForInsertInString;

public:
    SuffixTree(string _inputString){
        this->inputString = _inputString;
    }


    void CreateSuffixTree()
	{
		int inputStringLen=inputString.length();
		for(int i=0;i<inputStringLen; i++)
		{
			AddCharacter(inputString[i]);
		}
	}


    //Handling a new character while following all the rules
	void AddCharacter(char c){
		//TODO
	}

    //Creating a new edge in the tree
	void AddNewEdge(Node *startNode, int startIndex, int endIndex)
	{
			Node *leaf=new Node(nodeNumber);
			nodeNumber++;
			NormalEdge *edge=new NormalEdge(startIndex, endIndex, startNode, leaf);
			startNode->edges.push_back(*edge);

	}
};

class Triple{
public:
	Node *activeNode;
	NormalEdge *activeEdge;
	int length;

	Triple() {}
	
	Triple(Node * _activeNode, int _length){
		this->activeNode=_activeNode;
		this->length=_length;
		this->activeEdge=NULL;
	}
};
//A link between two nodes represented by [index_of_first_char, index_of_last_char]


void WriteToFile(string str, char* fileName)
{
	ofstream outFile;
    outFile.open(fileName);
    outFile<<str;
    outFile.close();
}

string ReadFromFile(const char *filename)
{
  ifstream inFile(filename);
  if (inFile)
  {
    std::string input;
    inFile.seekg(0, std::ios::end);
    input.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&input[0], input.size());
    inFile.close();
    return(input);
  }
}


int main (int argc, char* argv[]) {
    return 0;
}
