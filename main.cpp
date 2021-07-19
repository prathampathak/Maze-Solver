#include<iostream>
#include<fstream>
#include <stdlib.h>
#include <chrono>
#include <thread>

using namespace std;

unsigned int microseconds;

struct Coordinate {
	int x;
	int y;
	bool isCheckpoint = false;
	bool *directions = new bool[4];

	Coordinate(int _x, int _y) {
		x = _x;
		y = _y;
		initDirections();
	}
	Coordinate() {
		x = y = 0;
		initDirections();
	}

	void initDirections() {
		directions[0] = directions[1] = directions[2] = directions[3] = false;
	}
	int directionsCount() {
		int count = 0;
		for (int i = 0; i < 4; i++) {
			if (directions[i]) {
				count++;
			}
		}
		return count;
	}
	int* getIndexesOfDirection() {
		int count = directionsCount();
		int* indexes = new int[count];
		int j = 0;
		for (int i = 0; i < 4; i++) {
			if (directions[i]) {
				indexes[j] = i;
				j++;
			}
		}

		return indexes;
	}

};

class Stack {
public:
	Stack() :top(0) {

	}
	~Stack() {

		while (!isEmpty()) {
			pop();
		}
		isEmpty();
	}

	Stack(const Stack& lhs) {
		if (!lhs.top) {
			top = new StackNode(lhs.top->data,nullptr);
		}
		else {
			top = nullptr;
			return;
		}

		StackNode *pothernode = lhs.top;
		StackNode *pthisnode = top;


		while (pothernode->next)
		{
			pthisnode->next = new StackNode(pothernode->next->data, top);

			pothernode = pothernode->next;
			pthisnode = pthisnode->next;
		}

	}


	Stack& operator=(const Stack& rhs) {}

	void push(const Coordinate& object);
	Coordinate pop();
	const Coordinate& topElement();
	bool isEmpty();

private:
	struct StackNode {
		Coordinate data;
		StackNode *next;


		StackNode(const Coordinate& newData, StackNode *nextNode)
			: data(newData), next(nextNode) {}
	};
	StackNode *top;

};

void Stack::push(const Coordinate& obj) {
	top = new StackNode(obj, top);
}

Coordinate Stack::pop() {
	if (!isEmpty()) {
		StackNode *topNode = top;
		top = top->next;
		Coordinate data = topNode->data;
		delete topNode;
		return data;
	}
}

const Coordinate& Stack::topElement() {
	if (!isEmpty()) {
		return top->data;
	}
}

bool Stack::isEmpty() {
	if (top == 0) {
		return true;
	}
	else {
		return false;
	}
}

const int matrixWidth = 21;
const int matrixHeight = 21;

void readFile(char * fileName) {
	char ch[matrixWidth + 1];
	ifstream fin;
	fin.open(fileName);
	if (fin.is_open())
	{
		cout << "The " << fileName << " Maze:\n";
		while (fin.getline(ch, matrixWidth + 1)) cout << ch << endl;
	}
	fin.close();
}

void readMatrix(bool **matrix) {
	cout << endl;
	char closed = 219;
	for (int i = 0; i < matrixWidth; i++) {
		for (int j = 0; j < matrixHeight; j++) {
			if (matrix[i][j]) {
				cout << ' ';
			}
			else {
				cout << closed;
			}
		}
		cout << endl;
	}
}

void show(bool **matrix, Coordinate coordinate, char symbol) {
	system("CLS");
	cout << endl;
	char closed = 219;
	for (int i = 0; i < matrixWidth; i++) {
		for (int j = 0; j < matrixHeight; j++) {
			if (matrix[i][j]) {
				if (i == coordinate.y && j == coordinate.x) {
					cout << symbol;
				}
				else {
					cout << ' ';
				}
			}
			else {
				cout << closed;
			}
		}
		cout << endl;
	}

	cout << "At coordinate [" << coordinate.x << "][" << coordinate.y << "] " << coordinate.directions[0] << coordinate.directions[1] << coordinate.directions[2] << coordinate.directions[3] << endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void createMatrix(bool **matrix, char *fileName) {

	char rowCharArray[matrixWidth+1];
	char readData[100];

	ifstream fin;
	fin.open(fileName);

	if (fin.is_open())
	{
		int currentRow = 0;
		while (fin.getline(rowCharArray, matrixWidth+1)) {
			for (int i = 0; i < matrixWidth; i++) {
				matrix[currentRow][i] = (rowCharArray[i] == ' ');
			}
			currentRow++;
		}
	}
	fin.close();
}

bool** allocateMatrix() {
	bool **matrix = new bool*[matrixWidth];
	for (int i = 0; i < matrixWidth; ++i)
		matrix[i] = new bool[matrixHeight];
	return matrix;
}

Coordinate findStart(bool *row) {
	for (int i = 0; i < matrixWidth; i++) {
		if (row[i]) {
			return Coordinate(i, 0);
		}
	}
	return Coordinate(1, 0);
}
Coordinate findFinish(bool *row) {
	for (int i = 0; i < matrixHeight; i++) {
		if (row[i]) {
			return Coordinate(i, matrixHeight - 1);
		}
	}
	return Coordinate(1, matrixHeight - 1);
}

void updateAndPush(Stack *pathStack, Coordinate current, Coordinate next) {
	pathStack->pop();
	pathStack->push(current);
	pathStack->push(next);
}

void findThePath(bool **matrix, Stack *pathStack) {

	Coordinate start = findStart(matrix[0]);
	Coordinate finish = findFinish(matrix[matrixHeight - 1]);

	pathStack->push(start);

	pathStack->push(Coordinate(start.x, start.y + 1));

	Coordinate current = pathStack->topElement();

	int prevDirection = 3;

	while (!(current.x == finish.x && current.y == finish.y)) {

		Coordinate leftC = Coordinate(current.x - 1, current.y);
		Coordinate topC = Coordinate(current.x, current.y - 1);
		Coordinate rightC = Coordinate(current.x + 1, current.y);
		Coordinate bottomC = Coordinate(current.x, current.y + 1);

		if (!current.isCheckpoint) {
			//Left
			if (matrix[leftC.y][leftC.x]) {
				if (prevDirection != 2) {
					current.directions[0] = true;
				}
			}

			//Top
			if (matrix[topC.y][topC.x]) {
				if (prevDirection != 3) {
					current.directions[1] = true;
				}
			}

			//Right
			if (matrix[rightC.y][rightC.x]) {
				if (prevDirection != 0) {
					current.directions[2] = true;
				}
			}

			//Bottom
			if (matrix[bottomC.y][bottomC.x]) {
				if (prevDirection != 1) {
					current.directions[3] = true;
				}
			}
		}

		//To not escape through start
		if (topC.x == start.x && topC.y == start.y) {
			current.directions[1] = false;
		}

		show(matrix, current, '*');

		int count = current.directionsCount();

		if (count != 0) {

			if (count > 1) {
				current.isCheckpoint = true;
			}
			//In case of much free space
			if (count == 3) {

				int diffX = finish.x - current.x;

				if (current.directions[3]) {
					current.directions[3] = false;
					prevDirection = 3;
					updateAndPush(pathStack, current, bottomC);
				}
				else {
					if (diffX > 0 && current.directions[2]) {
						current.directions[2] = false;
						prevDirection = 2;
						updateAndPush(pathStack, current, rightC);
					}
					else {
						current.directions[0] = false;
						prevDirection = 0;
						updateAndPush(pathStack, current, leftC);
					}
				}

			} else {
				int nextStep = rand() % count;
				int next = current.getIndexesOfDirection()[nextStep];

				current.directions[next] = false;

				pathStack->pop();
				pathStack->push(current);

				prevDirection = next;

				switch (next)
				{
				case 0:
					pathStack->push(leftC);
					break;
				case 1:
					pathStack->push(topC);
					break;
				case 2:
					pathStack->push(rightC);
					break;
				case 3:
					pathStack->push(bottomC);
					break;
				}
			}
		}
		else {
			Coordinate buf = pathStack->topElement();
			while (buf.directionsCount() == 0) {
				show(matrix, pathStack->pop(), '!');
				buf = pathStack->topElement();
			}
		}

		current = pathStack->topElement();
	}
}

void showResult(bool **matrix, bool **pathMatrix) {
	cout << endl;
	char closed = 219;
	char path = 248;
	for (int i = 0; i < matrixWidth; i++) {
		for (int j = 0; j < matrixHeight; j++) {
			if (matrix[i][j]) {
				if (pathMatrix[i][j]) {
					cout << path;
				}
				else {
					cout << ' ';
				}
			}
			else {
				cout << closed;
			}
		}
		cout << endl;
	}
}

void showResultPath(bool **matrix, Stack *pathStack) {
	bool **resultMatrix = allocateMatrix();
	for (int i = 0; i < matrixWidth; i++) {
		for (int j = 0; j < matrixHeight; j++) {
			resultMatrix[i][j] = false;
		}
	}

	while (!pathStack->isEmpty()) {
		Coordinate partOfPath = pathStack->pop();
		resultMatrix[partOfPath.y][partOfPath.x] = true;
	}
	readMatrix(resultMatrix);
	showResult(matrix, resultMatrix);
}

int main()
{

	bool **matrix = allocateMatrix();

	char sourceFileEmpty[10] = "Maze0.txt";
	char sourceFile1[10] = "Maze1.txt";
	char sourceFile2[10] = "Maze2.txt";
	int select;
	char cont;

	do {

		readFile(sourceFileEmpty);
		readFile(sourceFile1);
		readFile(sourceFile2);

		do {
			cout << "Please pick a Maze (1/2/3):" << endl;

			cin >> select;
		} while (select > 3 || select < 1);

		char sourceFile[10];

		switch (select)
		{
		case 1:
			createMatrix(matrix, sourceFileEmpty);
			break;
		case 2:
			createMatrix(matrix, sourceFile1);
			break;
		case 3:
			createMatrix(matrix, sourceFile2);
			break;
		}

		Stack pathStack = Stack();
		findThePath(matrix, &pathStack);
		showResultPath(matrix, &pathStack);

		cout << "Continue?" << endl;
		cin >> cont;
	} while (cont == 'y');
  return 0;
}
