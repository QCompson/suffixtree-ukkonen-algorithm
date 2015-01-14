#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;


class Node;

//An edge between two nodes is described by the index of the first character and the index of the last character
//and represents a substring of the initial input
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

//The suffix link which is created between a node 'v' and node 's(v)' if the edge for 'v' is labeled xA, and
// the edge for 's(v)' is labeled A.
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
		int fs=0;
		if(e->startNode!=NULL){
			fs=e->startNode->number;
		}
		int fe=0;
		if(e->endNode!=NULL){
			fe=e->endNode->number;
		}
		for(vector<NormalEdge>::iterator it = edges.begin(); it != edges.end(); ++it) {
			NormalEdge *edgeTmp=&(*it);
			int ss=0;
			if(edgeTmp->startNode!=NULL){
				ss=edgeTmp->startNode->number;
			}
			int se=0;
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

//The algorithm is based on the triple: Active Node, Active Edge and Active Lenght which keeps a better track
//of the current step and helps out with characters already inserted in the tree, as well as with suffix links
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

//The Tree instance which iscreated from the initial input.
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
	Node *previousInsertedNode;

public:
    SuffixTree(string _inputString){
        this->inputString = _inputString;
		this->nodeNumber=1;
		Node *rootNode=new Node(this->nodeNumber);
		this->root=rootNode;
		this->nodeNumber++;
		this->currentPosition=-1;
		this->remainder=0;
		this->startCharacterIndexForInsertInString=0;
		this->graphvizOutput.clear();
		Triple *triple=new Triple(root, 0);
		this->activePoint=triple;
		this->previousInsertedNode=NULL;
    }


    void CreateSuffixTree(){
		int inputStringLen=inputString.length();
		for(int i=0;i<inputStringLen; i++){
			AddCharacter(inputString[i]);
		}
	}


    //Handling a new character while following all the rules
	void AddCharacter(char c){
		this->previousInsertedNode=NULL;
		this->currentPosition++;
		this->remainder++;
		AddSuffixToExistingEdges(root);

		while(this->remainder>0){
			if(this->activePoint->length==0){
				this->startCharacterIndexForInsertInString=this->currentPosition;
			}
			if(NeedToInsertNewEdge(false)){
				AddNewEdge(activePoint->activeNode, startCharacterIndexForInsertInString, startCharacterIndexForInsertInString);
				if(previousInsertedNode!=NULL)
				{
					SuffixEdge *suffixEdge=new SuffixEdge(previousInsertedNode, activePoint->activeNode);
					previousInsertedNode->suffixEdge=suffixEdge;
				}
				previousInsertedNode=activePoint->activeNode;
			}
			else{
				activePoint->activeEdge=ChooseActiveEdge();
				int edgeLen=activePoint->activeEdge->endCharacterIndex-activePoint->activeEdge->startCharacterIndex+1;
				if(activePoint->length>=edgeLen)
				{
					startCharacterIndexForInsertInString+=edgeLen;
					activePoint->length-=edgeLen;
					activePoint->activeNode=activePoint->activeEdge->endNode;
					activePoint->activeEdge=ChooseActiveEdge();
					continue;
				}
				if(!NeedToInsertNewEdge(true))
				{
					activePoint->length++;
					if(previousInsertedNode!=NULL)
					{
						SuffixEdge *suffixEdge=new SuffixEdge(previousInsertedNode, activePoint->activeNode);
						previousInsertedNode->suffixEdge=suffixEdge;
					}
					previousInsertedNode=activePoint->activeNode;
					break;
				}

			    int splitIndex=activePoint->length+activePoint->activeEdge->startCharacterIndex;
				AddNewInternalEdge(splitIndex, currentPosition);
				if(previousInsertedNode!=NULL)
				{
					SuffixEdge *suffixEdge=new SuffixEdge(previousInsertedNode, activePoint->activeEdge->endNode);
					previousInsertedNode->suffixEdge=suffixEdge;
				}
				previousInsertedNode=activePoint->activeEdge->endNode;
			}
			remainder--;
			if(activePoint->activeNode->number==root->number && activePoint->length>0){
				activePoint->length--;
				startCharacterIndexForInsertInString++;
				activePoint->activeEdge=ChooseActiveEdge();
			}
			else{
				if(activePoint->activeNode->suffixEdge==NULL){
					activePoint->activeNode=root;
				}
				else{
					activePoint->activeNode=activePoint->activeNode->suffixEdge->endNode;
				}
			}
		}

	}

    //Creating a new edge in the tree
	void AddNewEdge(Node *startNode, int startIndex, int endIndex){
			Node *leaf=new Node(nodeNumber);
			nodeNumber++;
			NormalEdge *edge=new NormalEdge(startIndex, endIndex, startNode, leaf);
			startNode->edges.push_back(*edge);
	}

	void AddNewInternalEdge(int startIndex, int endIndex)
	{
		Node *splittedEdgeNode =new Node(nodeNumber);
		nodeNumber++;

		NormalEdge *splittedEdge=new NormalEdge(activePoint->activeEdge->startCharacterIndex, startIndex-1, activePoint->activeNode, splittedEdgeNode);
		activePoint->activeNode->edges.push_back(*splittedEdge);
		activePoint->activeEdge=ChooseActiveEdge();

		Node *leafEdgeNode=new Node(nodeNumber);
		nodeNumber++;
		NormalEdge *leafEdge=new NormalEdge(endIndex, endIndex, splittedEdgeNode, leafEdgeNode);
		splittedEdgeNode->edges.push_back(*leafEdge);

		activePoint->activeEdge->startCharacterIndex+=activePoint->length;

		activePoint->activeEdge->startNode=splittedEdgeNode;
		splittedEdgeNode->edges.push_back(*(activePoint->activeEdge));

		activePoint->activeNode->RemoveEdge(activePoint->activeEdge);
		activePoint->activeEdge=splittedEdge;
	}

	void AddSuffixToExistingEdges(Node *_startNode){
		int edgesSize=_startNode->edges.size();
		for(int i=0;i<edgesSize;i++){
			NormalEdge *tmpEdge=&_startNode->edges.at(i);
			if(tmpEdge->endNode->edges.size()==0){
				tmpEdge->endCharacterIndex++;
			}
			else{
				AddSuffixToExistingEdges(tmpEdge->endNode);
			}
		}
	}

	bool NeedToInsertNewEdge(bool useActiveLen){
		int len=1;
		if(useActiveLen){
			len+=activePoint->length;
		}
		if(activePoint->activeEdge==NULL){
			int edgesSize=activePoint->activeNode->edges.size();
			for(int i=0;i<edgesSize;i++){
				NormalEdge *tmp=&activePoint->activeNode->edges.at(i);
				string s1=inputString.substr(tmp->startCharacterIndex, len);
				string s2=inputString.substr(startCharacterIndexForInsertInString, len);

				if(s1.compare(s2)==0){//equals
					return false;
				}
			}
			return true;
		}
		else{
			string s1=inputString.substr(activePoint->activeEdge->startCharacterIndex, len);
			string s2=inputString.substr(startCharacterIndexForInsertInString, len);
			if(s1.compare(s2)==0){//equals
				return false;
			}
		}
		return true;
	}

	NormalEdge* ChooseActiveEdge(){
		int edgeSize=activePoint->activeNode->edges.size();
		for(int i=0;i<edgeSize;i++){
			NormalEdge *tmpEdge=&activePoint->activeNode->edges.at(i);
			string s1=inputString.substr(tmpEdge->startCharacterIndex, 1);
			string s2=inputString.substr(startCharacterIndexForInsertInString, 1);

			if(s1.compare(s2)==0){//equals
				return tmpEdge;
			}
		}
	return NULL;
	}

	//Printing the edges in a standard .dot notation
	void PrintEdges(Node *n)
	{
		int edgesSize=n->edges.size();
		for(int i=0;i<edgesSize;i++)
		{
			NormalEdge *tmp=&n->edges.at(i);
			graphvizOutput<<"\t node"<<n->number;
			graphvizOutput<<" -> node"<<tmp->endNode->number;
			graphvizOutput<<" [label=\"";
			graphvizOutput<<tmp->text;
			graphvizOutput<<"\",weight=3] \n";
			PrintEdges(tmp->endNode);
		}
	}

	//Printing the tree in a .dot format which is a standard for graphviz visualization
	string PrintTree()
	{
		graphvizOutput.clear();
        graphvizOutput<<"digraph { \n";
        graphvizOutput<<"\t rankdir = LR; \n";
        graphvizOutput<<"\t edge [arrowsize=0.5,fontsize=12] \n";
        graphvizOutput<<"\t node1 [label=\"\",style=filled,fillcolor=lightgrey,shape=circle,width=.1,height=.1]; \n";
        graphvizOutput<<"//------leaves------ \n";
        PrintLeaves(root);
        graphvizOutput<<"//------nodes------ \n";
        PrintInternalNodes(root);
        graphvizOutput<<"//------edges------ \n";
        PrintEdges(root);
        graphvizOutput<<"//------suffix links------ \n";
	    PrintSuffixEdges(root);
        graphvizOutput<<"} \n";

        return graphvizOutput.str();
	}

};

//Methods handling input/output

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
