#include "bits/stdc++.h"
using namespace std;
  
int originalGraph[40][40],minimumSpanningTree[40][40];
int adjacencyMatrixForMinimumSpanningTree[40][40];
bool visited[40];
int parentsMinimumSpanningTree[40];
int verticesMinimumSpanningTree[40];
unordered_map<string,int> mstMap;//path, cost for that particular mst
unordered_map<string,int> closedList; // this is our closed List which we have kept to keep track of nodes that have been extended , and can be used to prune away the paths. full state and cost for that 
//Each time we update the vertices , we update their parents too-> Not that for the 

class Node
{
    /*This class defines the State of the problem. It also defines how the Cities will
    be stored in the priority_queue*/
    public:
        int citynum;
        string pathSoFar;
        int citiesNotVisited;
        char name;
        int hCost;
        int actualCost;
        int totalCost;
        string state;// the state contains the sorted version of pathsofar+ the last city visited
        vector<int> citiesLeft;
        
        bool operator <(Node  other) const
        {
            return  this->totalCost > other.totalCost;
        }
        
        Node(){};
        Node(int citynum,string pathSoFar,int citiesNotVisited,char name,int hCost,int actualCost,int totalCost,string state,vector<int> citiesLeft):citynum(citynum),pathSoFar(pathSoFar),citiesNotVisited(citiesNotVisited),name(name),hCost(hCost),actualCost(actualCost),totalCost(totalCost),state(state),citiesLeft(citiesLeft){}     
    
}City[50000];
int nodeTrack=0;

priority_queue<Node> astar;
class TSP 
{
    /*This Class defines the Basic functions for the TSP Problem, like taking the Input and calculating the distance matrix for the cities*/
    public:
    
     char node[3];
     int points[40][2],numCities;
     //TSP(int i,int j) : x(i),y(j){}
     
     void inputData()
     {
        
        scanf("%d",&numCities);
        int i=0;
        getc_unlocked(stdin);
        //double distance;
        while(i<numCities)
        {
            scanf("%s%d%d ",node,&points[i][0],&points[i][1]);
            i++;
        }
        
     }
     
     int CalculateDistance(int x1,int y1,int x2,int y2)
     {
        return int(sqrt(pow(x1-x2,2)+pow(y1-y2,2)) + 0.5 );
     }
     void createOriginalDistanceGraph()
     {
        int i,j;
        for(i=0;i<numCities;i++)
        {
            originalGraph[i][i]=0;
            adjacencyMatrixForMinimumSpanningTree[i][i]=0;
            for(j=i+1;j<numCities;j++)
            {
                originalGraph[i][j]=CalculateDistance(points[i][0],points[i][1],points[j][0],points[j][1]);
                originalGraph[j][i]=originalGraph[i][j];
                // This is a Symmetric TSP
                adjacencyMatrixForMinimumSpanningTree[i][j]=0;
                adjacencyMatrixForMinimumSpanningTree[j][i]=0;
                minimumSpanningTree[i][j]=INT_MAX;
                minimumSpanningTree[j][i]=INT_MAX;
            }
        }
     } 
};

int distance(int city1,int city2)
{
    return originalGraph[city1][city2];
}

int buildMinimumSpanningTree2(vector<int> vertices,vector<char> nodesLeft)
{
    /*This function returns the length of the minimumSpanningTree for the
    remaining unvisited Cities.
    */
    int size=vertices.size();
    if(size==1)
    {
        return 0;
    }
    
    
    int pcity[40],pdist[40],minDistance=INT_MAX;
    vector<int>::iterator it1;
    vector<char>::iterator it2;
    int i=0;
    string cities;
    sort(nodesLeft.begin(),nodesLeft.end());
    for(it1=vertices.begin(),it2=nodesLeft.begin();it1!=vertices.end(),it2!=nodesLeft.end();it1++,it2++)
    {
        pcity[i]=*it1; //parent city 
        pdist[i]=INT_MAX; //parent distance;
        i++;
        cities+=*it2;
    }
    
    unordered_map<string,int>:: iterator mit;//iterator for the hash map for the MST
    //So that we don't have to calculate the MST length again and again for each path.
    mit=mstMap.find(cities);
    if(mit!=mstMap.end())
    {
        return mit->second;
    }
    
    int newCity=pcity[size-1];//i.e we are making the last city as the newCity for finding the MST
    int thisDistance;
    int length=0,minIndex;
    for(int m=size-1;m>0;m--)
    {
        minDistance=INT_MAX;
        for(int j=0;j<m;j++)
        {
            thisDistance=distance(pcity[j],newCity);
            
            if(thisDistance < pdist[j]) pdist[j]=thisDistance;
            if(pdist[j]<minDistance) minDistance=pdist[j],minIndex=j;
        }
        newCity=pcity[minIndex];
        length+=minDistance;
        pcity[minIndex]=pcity[m-1];
        pdist[minIndex]=pdist[m-1];   
    }
    mstMap[cities]=length;
    return length;
    
}

int calculateHeuristic(vector<int> vertices,vector<char> nodesLeft,int currentCityForExpansion)
{

    /*This function calculates the Heuristic value for the remaining path from the current city to the remaining unvisited cities to the source city.*/
    int size=vertices.size();

    if(size==1)
    {
        vector<int>:: iterator itt=vertices.begin();
    
        return distance(currentCityForExpansion,*itt)+ distance(*itt,0);
    }
    if(size==0)
    {
        
        return distance(currentCityForExpansion,0);
    }
    
    int pcity[40],pdist[40],minDistance=INT_MAX;
    vector<int>::iterator it1;
    //vector<char>::iterator it2;
    int i=0;
    string cities;
    
    for(it1=vertices.begin();it1!=vertices.end();it1++)
    {
        pcity[i]=*it1; //parent city 
        pdist[i]=INT_MAX; //parent distance;
        i++;

    }
    int mst;
  
        mst=buildMinimumSpanningTree2(vertices,nodesLeft);
   
    int nearestUnvisitedCityDistance=INT_MAX,nearestToSource=INT_MAX,thisDistance1,thisDistance2;

    for(i=0;i<size;i++)
    {
       
        thisDistance1=distance(pcity[i],currentCityForExpansion);// this is the distancefrom the unvisited city to the currentCityForExpansion
        thisDistance2=distance(pcity[i],0); //this is the distance from the source
        if(thisDistance1<nearestUnvisitedCityDistance) nearestUnvisitedCityDistance=thisDistance1;
        
        if(thisDistance2<nearestToSource) nearestToSource=thisDistance2;
        
    }
    int hn=mst+nearestToSource+nearestUnvisitedCityDistance;
    return hn;
}



int optimumCost=INT_MAX;

Node createNode(int citynum,string pathSoFar,int citiesNotVisited,char name,int hCost,int aCost,int totalCost,string state,vector<int> citiesLeft)
{
    /*This function Creates a new City to be explored*/
    Node temp( citynum, pathSoFar, citiesNotVisited, name, hCost,aCost,totalCost,state, citiesLeft);
    return temp;
}

int numExpanded=1,totalNumofNodes=1;
string startSearch()
{
    /*This is the main ASTAR Search Function where the nodes are generated and put on the Priority Queue*/
    Node current;
    int hn,tCost,aCost;
    char name;
    vector<int> vertices;
    vector<int>::iterator it,it3;
    vector<int> nextUnvisitedCities;
    vector<char> namesOfCitiesYettoVisit,namesOfCitiesYettoVisit2;
    vector<char>::iterator it2;
    string pathSoFar,state,npathSoFar,nstate;
    string res;
    while(!astar.empty() and astar.top().totalCost < optimumCost)
    {
        current=astar.top();
         astar.pop();

        pathSoFar=current.pathSoFar;
        state=current.state;
        
        
        vertices=current.citiesLeft;
        for(it=vertices.begin();it!=vertices.end();it++)
        {
            int next=*it;

         }
       
        if(current.citiesNotVisited==0)
        {
          
            int tcost=current.actualCost + distance(current.citynum,0);
            
            if(tcost< optimumCost )
            {
                optimumCost=tcost;
                res=current.pathSoFar;

            }
           
            continue;
        }
        if(closedList.find(state)!=closedList.end() and closedList[state] < current.totalCost)
        {
            
            continue;
        }
        if(closedList.find(state)!=closedList.end() and closedList[state] > current.totalCost)
        {
            
            closedList[state]=current.totalCost;
        }
        else if(closedList.find(state)==closedList.end())
        {
            closedList[state]=current.totalCost;
        }
        numExpanded++;
        for(it=vertices.begin();it!=vertices.end();it++)
        {
            int next=*it;
            nextUnvisitedCities.clear();
            namesOfCitiesYettoVisit.clear();
            vector<int>:: iterator it4;

            for(it3=vertices.begin();it3!=vertices.end();it3++)
            {
                int x=*it3;
                
                if(x==next)
                {
                    continue;
                } 

                nextUnvisitedCities.push_back(x);
            }
           
            for(it4=vertices.begin();it4!=vertices.end();it4++)
            {
                int city3=*it4;
                if(city3==next) continue;
                if( city3>25)
                {
                    namesOfCitiesYettoVisit.push_back('a'+ city3-26);
                }
                else
                {
                    namesOfCitiesYettoVisit.push_back('A'+city3);
                }
            }
            hn=calculateHeuristic(nextUnvisitedCities,namesOfCitiesYettoVisit,next);

            aCost=current.actualCost + distance(current.citynum,next);
            tCost=hn+ aCost;

            if( next>25)
            {
                name='a'+ next-26;
            }
            else
            {
                name='A'+next;
            }
            npathSoFar=pathSoFar+name;
            
            string temps=npathSoFar;
            sort(temps.begin(),temps.end());
            nstate=temps+name;
            
           
            totalNumofNodes++;
            astar.push(createNode(next,npathSoFar,current.citiesNotVisited-1,name,hn,aCost,tCost,nstate,nextUnvisitedCities));
            
            
        }

    }
    return res;
}



int main()
{
    TSP prob1;
    int i;
    prob1.inputData();
    prob1.createOriginalDistanceGraph();// Till now we have the orginal Distance Graph
    int numCities=prob1.numCities;

    vector<int> v1;
    vector<char> v2;

    for( i=1;i<numCities;i++)
    {
        v1.push_back(i);
        if(i>25)
        {
            v2.push_back('a'+i-26);
        }
        else
        {
            v2.push_back('A'+i);
        }
    }
    vector<char>:: iterator it=v2.begin();
    int initialHeuristic=calculateHeuristic(v1,v2,0);

    
    
    City[nodeTrack++]=createNode(0,"A",numCities-1,'A',initialHeuristic,0,initialHeuristic,"AA",v1);
    
  

    astar.push(City[0]);
    string res=startSearch();
    
    int lengthMst = buildMinimumSpanningTree2(v1,v2);

cout<<"\n the optimumCost is "<<optimumCost<<" with path "<<res+'A'<<"\n";


    cout<<"the number of nodes expanded are "<<numExpanded<<" total nodes are "<<totalNumofNodes<<"\n";
    return 0;   
}
     
     
     

