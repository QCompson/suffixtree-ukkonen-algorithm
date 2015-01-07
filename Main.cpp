#include <iostream>

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
};



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

int main (int argc, char* argv[]) {
    return 0;
}
