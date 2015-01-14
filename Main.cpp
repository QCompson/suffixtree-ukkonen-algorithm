#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;


int currentPosition=-1;
class Node;
//Class for modeling edges. Edge has start and end Node, and start and end index.
//Indexes are used to determine which string from input is on that edge
class NormalEdge
{
public:
	int startCharacterIndex;
	int endCharacterIndex;
	Node *startNode; 
	Node *endNode;

	NormalEdge(int _startCharacterIndex, int _endCharacterIndex, Node *_startNode, Node *_endNode){
		this->startCharacterIndex=_startCharacterIndex;
		this->endCharacterIndex=_endCharacterIndex;
		this->startNode=_startNode;
		this->endNode=_endNode;
	}
	NormalEdge(){}

	//Function returns index of last character that is on this edge.
	int returnEndCharacterIndex();
};


//Class for modeling nodes. Node has collection of edges that go from him and suffix link to other node, 
//number which is used when visualizing tree and bool which determines if node is leaf or not
class Node
{
public:
	Node *suffixEdge;
	vector<NormalEdge> edges;
	bool isLeaf;
	int number;
	Node(int _number, bool _isLeaf){
		this->isLeaf=_isLeaf;
		this->number=_number;
		suffixEdge=NULL;
	}
	Node(){}


	//Function remove given edge from node
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
		for(vector<NormalEdge>::iterator it = edges.begin(); it != edges.end(); ++it){
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

int NormalEdge::returnEndCharacterIndex() {
		if(endNode->isLeaf==1){
			return currentPosition;
		}
		else{
			return endCharacterIndex;
		}
	}

//Class is modeling active point. Active point has active node, active edge and active length.
class Triple
{
public:
	Node *activeNode;
    NormalEdge *activeEdge;
    int length;

	Triple(Node *_activeNode, int _length){
		this->activeNode = _activeNode;
		this->length = _length;
		activeEdge=NULL;
	}
	Triple(){}
};

//Class is modeling suffix tree. It has functions for creating and printing suffix tree.
class SuffixTree
{
public:
	SuffixTree(string _inputString){
		this->nodeNumber=1;
		Node *rootNode=new Node(this->nodeNumber,false);
		root=rootNode;
		this->nodeNumber++;

		this->inputString=_inputString;
		this->remainder=0;
		this->startCharacterIndexForInsertInString=0; //ovo možda ne treba? povezano s tekstom u nodeu
		graphvizIspis.clear();
		Triple *triple=new Triple(root, 0);
		this->activePoint=triple;
		previousInsertedNode=NULL;
	}

	//Function takes each character from input string and call method for adding character to tree
	void CreateSuffixTree(){
		int inputStringLen=inputString.length();
		for(int i=0;i<inputStringLen; i++){
			AddCharacter(inputString[i]);
		}
	}

	//Function returns whole tree as string
	string PrintTree(){
		graphvizIspis.clear();
        graphvizIspis<<"digraph { \n";
        graphvizIspis<<"\t rankdir = LR; \n";
        graphvizIspis<<"\t edge [arrowsize=0.4,fontsize=10] \n";
        graphvizIspis<<"\t node1 [label=\"\",style=filled,fillcolor=lightgrey,shape=circle,width=.1,height=.1]; \n";
        graphvizIspis<<"//------leaves------ \n";
        PrintLeaves(root);
        graphvizIspis<<"//------internal nodes------ \n";
        PrintInternalNodes(root);
        graphvizIspis<<"//------edges------ \n";
        PrintEdges(root);
        graphvizIspis<<"//------suffix links------ \n";
	    PrintSuffixEdges(root);
        graphvizIspis<<"} \n";

        return graphvizIspis.str();
	}

private:
	Node *root;
	int nodeNumber;
	string inputString;
    Triple *activePoint;
    int remainder;
    int startCharacterIndexForInsertInString; //index of character in string from which new edge starts
    stringstream graphvizIspis;
	Node *previousInsertedNode; //node previously inserted during same step

	//Function has whole logic for adding character in suffix tree
	void AddCharacter(char c){
		previousInsertedNode=NULL;
		currentPosition++;
		remainder++;

		while(remainder>0){

			if(activePoint->length==0){
				startCharacterIndexForInsertInString=currentPosition;
			}
			if(NeedToInsertNewEdge(false)){

				AddNewEdge(activePoint->activeNode, startCharacterIndexForInsertInString, startCharacterIndexForInsertInString);
				if(previousInsertedNode!=NULL){
					previousInsertedNode->suffixEdge=activePoint->activeNode;
				}
				previousInsertedNode=activePoint->activeNode;
			}
			else{
				activePoint->activeEdge=ChooseActiveEdge();
				int edgeLen=activePoint->activeEdge->returnEndCharacterIndex()-activePoint->activeEdge->startCharacterIndex+1;
				if(activePoint->length>=edgeLen){
					startCharacterIndexForInsertInString+=edgeLen;
					activePoint->length-=edgeLen;
					activePoint->activeNode=activePoint->activeEdge->endNode;
					activePoint->activeEdge=ChooseActiveEdge();
					continue;
				}
				if(!NeedToInsertNewEdge(true)){
					activePoint->length++;
					if(previousInsertedNode!=NULL){
						previousInsertedNode->suffixEdge=activePoint->activeNode;
					}
					previousInsertedNode=activePoint->activeNode;
					break;
				}

				int splitIndex=activePoint->length+activePoint->activeEdge->startCharacterIndex;
				AddNewInternalEdge(splitIndex, currentPosition);
				if(previousInsertedNode!=NULL){
					previousInsertedNode->suffixEdge=activePoint->activeEdge->endNode;
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
					activePoint->activeNode=activePoint->activeNode->suffixEdge;
				}
			}
		}
	}

	//Function creates and adds new leaf node to tree
	void AddNewEdge(Node *startNode, int startIndex, int endIndex){
			Node *leaf=new Node(nodeNumber, true);
			nodeNumber++;
			NormalEdge *edge=new NormalEdge(startIndex, endIndex, startNode, leaf);
			startNode->edges.push_back(*edge);
	}

	//Function splits edge and creates two other ethes from it along with it's corresponding nodes and adds it to tree
	void AddNewInternalEdge(int startIndex, int endIndex){
		Node *splittedEdgeNode =new Node(nodeNumber, false);
		nodeNumber++;

		NormalEdge *splittedEdge=new NormalEdge(activePoint->activeEdge->startCharacterIndex, startIndex-1, activePoint->activeNode, splittedEdgeNode);

		activePoint->activeNode->edges.push_back(*splittedEdge);
		activePoint->activeEdge=ChooseActiveEdge();

		Node *leafEdgeNode=new Node(nodeNumber, true);
		nodeNumber++;
		NormalEdge *leafEdge=new NormalEdge(endIndex, endIndex, splittedEdgeNode, leafEdgeNode);

		splittedEdgeNode->edges.push_back(*leafEdge);

		activePoint->activeEdge->startCharacterIndex+=activePoint->length;

		activePoint->activeEdge->startNode=splittedEdgeNode;
		splittedEdgeNode->edges.push_back(*(activePoint->activeEdge));

		activePoint->activeNode->RemoveEdge(activePoint->activeEdge);
		activePoint->activeEdge=splittedEdge;
	}


	//Function decides if there is need for inserting new edge into tree.
	bool NeedToInsertNewEdge(bool useActiveLen){
		int len=1;
		if(useActiveLen){
			len+=activePoint->length;
		}
		if(activePoint->activeEdge==NULL){
			int edgesSize=activePoint->activeNode->edges.size();
			for(int i=0;i<edgesSize;i++){
				if(len==1){
					NormalEdge *tmp=&activePoint->activeNode->edges.at(i);
					char s1=inputString.at(tmp->startCharacterIndex);
					char s2=inputString.at(startCharacterIndexForInsertInString);
					if(s1==s2){//equals
						return false;
					}
				}
				else{
					NormalEdge *tmp=&activePoint->activeNode->edges.at(i);
					string s1=inputString.substr(tmp->startCharacterIndex, len);
					string s2=inputString.substr(startCharacterIndexForInsertInString, len);
					if(s1.compare(s2)==0){//equals
						return false;
					}
				}
			}
			return true;
		}
		else{
			if(len==1){
					char s1=inputString.at(activePoint->activeEdge->startCharacterIndex);
					char s2=inputString.at(startCharacterIndexForInsertInString);
					if(s1==s2){//equals
						return false;
					}
			}
			else{
				string s1=inputString.substr(activePoint->activeEdge->startCharacterIndex, len);
				string s2=inputString.substr(startCharacterIndexForInsertInString, len);
				if(s1.compare(s2)==0){//equals
					return false;
				}
			}
		}
		return true;
	}

	//Function chooses active node which will be used in active point
	NormalEdge* ChooseActiveEdge(){
		int edgeSize=activePoint->activeNode->edges.size();
		for(int i=0;i<edgeSize;i++){
			NormalEdge *tmpEdge=&activePoint->activeNode->edges.at(i);
			char s1=inputString.at(tmpEdge->startCharacterIndex);
			char s2=inputString.at(startCharacterIndexForInsertInString);
			if(s1==s2){//equals
				return tmpEdge;
			}
		}
		return NULL;
	}

	//Function prints edges in tree
	void PrintEdges(Node *n){
		int edgesSize=n->edges.size();
		for(int i=0;i<edgesSize;i++){
			NormalEdge *tmp=&n->edges.at(i);
			graphvizIspis<<"\t node"<<n->number;
			graphvizIspis<<" -> node"<<tmp->endNode->number;
			graphvizIspis<<" [label=\"";
			graphvizIspis<<inputString.substr(tmp->startCharacterIndex, tmp->returnEndCharacterIndex()-tmp->startCharacterIndex+1);
			graphvizIspis<<"\",weight=3] \n";
			PrintEdges(tmp->endNode);
		}
	}

	//Function prints leaf nodes in tree
	void PrintLeaves(Node *n){
		int edgesSize=n->edges.size();
		if(edgesSize==0){
			graphvizIspis<<"\t node"<<n->number;
			graphvizIspis<<" [label=\"\",shape=point] \n";
		}
		else{
			for(int i=0;i<edgesSize;i++){
				NormalEdge *tmp=&n->edges.at(i);
				if(tmp->endNode!=NULL){
					PrintLeaves(tmp->endNode);
				}
			}
		}
	}

	//Function prints internal nodes in tree
	void PrintInternalNodes(Node *n){
		int edgesSize=n->edges.size();
		if(n->number!=root->number && edgesSize>0){
			graphvizIspis<<"\t node";
			graphvizIspis<<n->number;
			graphvizIspis<<" [label=\"\",style=filled,fillcolor=lightgrey,shape=circle,width=.07,height=.07] \n";
		}
		if(edgesSize>0){
			for(int i=0;i<edgesSize;i++){
				NormalEdge *e=&n->edges.at(i);
				if(e->endNode!=NULL){
					PrintInternalNodes(e->endNode);
				}
			}
		}
	}

	//Function prints suffix links in tree
	void PrintSuffixEdges(Node *n){
		if(n->suffixEdge!=NULL){
			graphvizIspis<<"\t node" ;
			graphvizIspis<<n->number<<" -> node";
			graphvizIspis<<n->suffixEdge->number<<" [label=\"\",weight=1,style=dotted] \n";
		}
		int edgesSize=n->edges.size();
		for(int i=0;i<edgesSize;i++){
			NormalEdge *e=&n->edges.at(i);
			if(e->endNode!=NULL){
			PrintSuffixEdges(e->endNode);
			}
		}
	}

};

//Function writes tree (which ih generated as string) in file
void WriteToFile(string str){
	ofstream myfile;
    myfile.open ("tree.dot");
    myfile << str;
    myfile.close();
}

//Function read input string from file
string ReadFromFile(const char *filename){
  ifstream inFile(filename);
  if (inFile){
    std::string input;
    inFile.seekg(0, std::ios::end);
    input.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&input[0], input.size());
    inFile.close();
    return(input);
  }
}

int main(int argc, char* argv[]) {

	SuffixTree *st=new SuffixTree(ReadFromFile("inputString.txt"));
	//SuffixTree *st=new SuffixTree("mississippi#");
	st->CreateSuffixTree();
	string result=st->PrintTree();
	WriteToFile(result);

	//If user wants to see the graphviz output
	//cout <<result<<endl;


	cout<<"The sufix tree creation is done"<<endl;

    return 0;
}
