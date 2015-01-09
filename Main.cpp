#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Node {
public:
    int number;

    Node() {}

    Node(int _number){
        this->number = _number;
    }
};

class SuffixTree {
private:
    Node *root;
	int nodeNumber;
	int currentPosition;
	string inputString;

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

	}
};


//A link between two nodes represented by [index_of_first_char, index_of_last_char]
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

void WriteToFile(string str, char* fileName)
{
	ofstream outFile;
    outFile.open(fileName);
    outFile<<str;
    outFile.close();
}


int main (int argc, char* argv[]) {
    return 0;
}
