#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <queue>
using namespace std;

// Declaring Node class
struct Node
{
    string key;
    vector<Node *>child;
    Node * parent;
    int shifter;
    int cost;

    bool operator==(const Node& p) const
    {
        return key == p.key;
    }
};

// Node constructor
Node *newNode(string key, Node * parent)
{
    Node *temp = new Node;
    temp->key = key;
    temp->parent = parent;
    return temp;
};

// Comparator for priority queue of Nodes
struct LessThanByCost
{
    bool operator()(const Node* lhs, const Node* rhs) const
    {
        return lhs->cost <= rhs->cost;
    }
};

// Global defaults
string goalState = "ANGELICA.";
string trivialState  = "ANGELICA.";
string easyState = "ANGELIC.A";
string mediumState = "AN.ELGCAI";
string hardState = ".ANELGCAI";
string difficultState = "LNGEAC.AI";


// Takes a string and prints in the shape of actual game in terminal
void printPuzzle(string puzzle) {
    for (int i = 0; i < puzzle.size(); ++i) {
        if (i % 3 == 0) {
            cout << "[" << puzzle[i] << ",";
        }
        else if (i % 3 == 1) {
            cout << puzzle[i] << ",";
        }
        else {
            cout << puzzle[i] << "]\n";
        }
    }
}

// Read choices from user, and prepare the code for it.
vector<int> printMenu(string &custom) {
    vector<int> choices;
    int input = 0;
    // Intro and mode selection
    cout << "This is an Angelica Puzzle solver, Enter the number for the mode would you like to use? \n";
    cout << "1) Uniform Cost Search \n";
    cout << "2) A* with the Misplaced Tile heuristic \n";
    cout << "3) A* with the Manhatten Distance heuristic \n";
    cin >> input;
    choices.push_back(input);

    // Default vs custom puzzle
    cout << "Enter 1 to use default puzzles or 2 to input your own.\n";
    cin >> input;
    choices.push_back(input);
    if (input == 2) {
        cout << "Please enter a string of 9 characters, with a valid angelica solution please.\n";
        cin >> input;
        custom = input;
    }
    else {
        cout << "Press the number next to your chosen puzzle:" << "\n";
        cout << "1) Trivial" << "\n";
        printPuzzle(trivialState);
        cout << "2) Easy" << "\n";
        printPuzzle(easyState);
        cout << "3) Medium" << "\n";
        printPuzzle(mediumState);
        cout << "4) Hard" << "\n";
        printPuzzle(hardState);
        cout << "5) Difficult" << "\n";
        printPuzzle(difficultState);
        cin >> input;
        choices.push_back(input);
    }
    return choices;
}

void printVector(vector<string> output) {
    cout << "[";
    for (int i = 0; i < output.size() - 1; ++i) {
        cout << output[i] << ",";
    }
    cout << output[output.size() - 1] << "]" << "\n";
}

// Position of "." in string
int findShifter(string puzzle) {
    for (int i = 0; i < puzzle.size(); ++i) {
        if (puzzle[i] == '.') {
            return i;
        }
    }
    return -1;
}

// Distance Formula
int distance(int index, int goal) {
    int num = 0;
    
    if ((index / 3) != (goal / 3)) {
        // If true, they are on the different y axis.
        if (goal >= 5) {
            while (index < 5) {
                index += 3;
                num++;
            }
        }
        else if(goal <= 2) {
            while (index > 2) {
                index -= 3;
                num++;
            }
        }

    }
    while (index < goal) {
        index++;
        num++;
    }
    while (index > goal) {
        index--;
        num++;
    }
    return num;
}

// Mode is what cost function, calclate cost and rank nodes accordingly
int getCost(string puzzle, int mode) {
    switch(mode) {
        case 1: {
            // Uniform Search Cost (BFS)
            // expand if possible, left up down right
            return 1;
            break;
        }
        case 2: {
            // A* with the Misplaced Tile heuristic
            // Calculate lowest cost, then expand in that order.
            int count = 0;
            for (int i = 0; i < goalState.size(); ++i) {
                if ( (puzzle[i] != goalState[i]) && (puzzle[i] != '.') ) {
                    count++;
                }
            }
            return count;
            break;
        }
        case 3: {
            // A* with the Manhatten Distance heuristic
            int total = 0;
            for (int i = 0; i < goalState.size(); ++i) {
                if ( (puzzle[i] != goalState[i]) && (puzzle[i] != '.') ) {
                    switch (puzzle[i]) {
                        case 'A': {
                            int dist1 = distance(i, 0);
                            int dist2 = distance(i, 7);
                            int min = dist1;
                            if (dist2 < dist1) {
                                min = dist2;
                            }
                            total += min;
                            break;
                        }
                        case 'N': {
                            total += distance(i, 1);
                            break;
                        }
                        case 'G': {
                            total += distance(i, 2);
                            break;
                        }
                        case 'E': {
                            total += distance(i, 3);
                            break;
                        }
                        case 'L': {
                            total += distance(i, 4);
                            break;
                        }
                        case 'I': {
                            total += distance(i, 5);
                            break;
                        }
                        case 'C': {
                            total += distance(i, 6);
                            break;
                        }
                    }
                }
            }
            return total;
            break;

        }
        
    }
    return -1;
}

Node* generalSearch(string problem, int mode) {
    // Logging the nodes I have found to prevent searching a puzzle already queued
    vector<string> catalog;

    // Initialize root node
    Node *root = newNode(problem, NULL);
    root->shifter = findShifter(problem);
    root->cost = getCost(root->key, mode);

    // Prepare queue
    priority_queue<Node* , vector<Node*>, LessThanByCost> checklist;
    checklist.push(root);
    catalog.push_back(root->key);
    while(!checklist.empty()) {
        if (checklist.top()->key == goalState) {
            return checklist.top();
        }
        else {
            int index = checklist.top()->shifter;

            // left, swap one left (if index % 3 = 0 then can't do it)
            if (index % 3 != 0) {
                string movement = checklist.top()->key;
                char temp = movement[index];
                movement[index] = movement[index - 1];
                movement[index - 1] = temp;
                bool found = false;
                for (int i = 0; i < catalog.size(); ++i) {
                    if (catalog[i] == movement) {
                        found = true;
                    }
                }
                if (!found) {
                    Node *left = newNode(movement, checklist.top());
                    checklist.top()->child.push_back(left);
                    left->cost = getCost(left->key, mode);
                    left->shifter = findShifter(left->key);
                    catalog.push_back(movement);
                    checklist.push(left);
                }
            }
            // right, swap one right (if index % 3 = 2 then can't do it)
            if (index % 3 != 2) {
                string movement = checklist.top()->key;
                char temp = movement[index];
                movement[index] = movement[index + 1];
                movement[index + 1] = temp;
                bool found = false;
                for (int i = 0; i < catalog.size(); ++i) {
                    if (catalog[i] == movement) {
                        found = true;
                    }
                }
                if (!found) {
                    Node *right = newNode(movement, checklist.top());
                    checklist.top()->child.push_back(right);
                    right->cost = getCost(right->key, mode);
                    right->shifter = findShifter(right->key);
                    catalog.push_back(movement);
                    checklist.push(right);
                }
            }
            // up, swap 3 over left (if index < 3 then can't)
            if (index >= 3) {
                string movement = checklist.top()->key;
                char temp = movement[index];
                movement[index] = movement[index - 3];
                movement[index - 3] = temp;
                bool found = false;
                for (int i = 0; i < catalog.size(); ++i) {
                    if (catalog[i] == movement) {
                        found = true;
                    }
                }
                if (!found) {
                    Node *up = newNode(movement, checklist.top());
                    checklist.top()->child.push_back(up);
                    up->cost = getCost(up->key, mode);
                    up->shifter = findShifter(up->key);
                    catalog.push_back(movement);
                    checklist.push(up);
                }
            }
            // down, swap 3 over right (if index > 5 then can't)
            if (index < 6) {
                string movement = checklist.top()->key;
                char temp = movement[index];
                movement[index] = movement[index + 3];
                movement[index + 3] = temp;
                bool found = false;
                for (int i = 0; i < catalog.size(); ++i) {
                    if (catalog[i] == movement) {
                        found = true;
                    }
                }
                if (!found) {
                    Node *down = newNode(movement, checklist.top());
                    checklist.top()->child.push_back(down);
                    down->cost = getCost(down->key, mode);
                    down->shifter = findShifter(down->key);
                    catalog.push_back(movement);
                    checklist.push(down);
                }
            }
            
            // Queue has been extended
            checklist.pop();
        }
    }
    return NULL;
}

int main() {

    string userInput = "ANGELICA.";

    vector<int> choices;
    // Input menu
    choices = printMenu(userInput);

    if (choices.size() == 3) {
        switch(choices[2]) {
        case 1:
            userInput = trivialState;
            break;
        case 2:
            userInput = easyState;
            break;
        case 3:
            userInput = mediumState;
            break;
        case 4:
            userInput = hardState;
            break;
        case 5:
            userInput = difficultState;
            break;
        default:
            userInput = trivialState;
        }
    }

    Node * temp = generalSearch(userInput, choices[0]);
    
    
    cout << "\nSTART ---------------------------------------------\n\n";
    
    // prints the solution and progrssion of the winning solution from top to bottom in that order.
    // States what steps to take for the winning game

    vector<string> solution;
    while (temp != NULL) {
        solution.push_back(temp->key);
        temp = temp->parent;
    }
    for (int i = solution.size() - 1; i >= 0; --i) {
        printPuzzle(solution[i]);
        cout << "\n";
    }
    

    system("pause");

    return 0;
}