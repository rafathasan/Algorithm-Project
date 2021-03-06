//============================================================================
// Name        : Huffman.cpp
// Author      : Rafat Hasan
// Github      : github.com/rafathasan/Algorithm-Project/blob/master/huffman.cpp
// Copyright   : GPLv3
// Description : HuffmanAlgorithm(Array Based) in C++, Ansi-style
//============================================================================
#include <iostream>

#include <fstream>

using namespace std;

struct Row {
  string c; 											//represent Binary Code also letter combination
  int i; 												//represent letter also frequency
};

void initialize_tables(int * feq, Row * & feqRows, int ansii, int & h);
void initialize_feq(int * , int);
void populateFeq(int * );
void constructBinaryTable(Row * , int, string * & , int ansii);
void addString(string * & , string, char);
int max(Row * A, int s);
int num_dig(int);
int i_th_dig(int, int);
void counting_sort(Row * A, int s, int i);
void radix_sort(Row * A, int s);
int findMin(Row * ary, int s, int ignore);
void encodeText(string * );
void decodeText(Row * , int, int);
int isMatched(Row * , int, string, int);
bool isEqual(string x, string y);
void shortenHuffTree(string * huffTree, int ansii, int t, Row * & arr);
void writeMetaData(int * feq, int ansii, Row * srtTable, int t);
void readMetaData(Row * srtTable, int & t);

int main() {
  const int ansii = 128;
  int * feq = new int[128]; 						//letter frequency Table
  Row * feqRows; 									//Main Array (Heap Tree)
  string * huffTable = new string[ansii]; 			//Binary Code table
  int t; 											//Distinct letter size

  initialize_feq(feq, ansii);
  populateFeq(feq);
  initialize_tables(feq, feqRows, ansii, t);

  radix_sort(feqRows, t);

  constructBinaryTable(feqRows, t, huffTable, ansii);
  encodeText(huffTable);

  Row * srtTable = new Row[t];
  shortenHuffTree(huffTable, ansii, t, srtTable);
  decodeText(srtTable, ansii, t);
  writeMetaData(feq, ansii, srtTable, t);

  readMetaData(srtTable, t);
  decodeText(srtTable, ansii, t);

  return 0;
}

void shortenHuffTree(string * huffTree, int ansii, int t, Row * & arr) {
  int c = 0;
  for (int i = 0; i < ansii; ++i) {
    if (huffTree[i][0] != '\0') {
      arr[c].i = i;
      arr[c].c = huffTree[i];
      c++;
    }
  }
}

void writeMetaData(int * feq, int ansii, Row * srtTable, int t) {
  ofstream out("frequency.txt");
  if (out.is_open()) {
    out << "Letter Frequency Chart\n";
    out << "-----------------------\n";
    for (int i = 0; i < ansii; ++i) {
      if (feq[i] > 0)
        out << (char) i << "    =>    " << feq[i] << "\n";
    }
    														//writing binary char by char
  }
  if (out.fail()) 											//checking access and end of file
    cout << "error reading decoded.txt" << endl;
  out.close();

  out.open("codeTable.txt");
  if (out.is_open()) {
    out << "Huffman Code Table\n";
    out << "------------------\n";
    for (int i = 0; i < t; ++i) {
      out << (char) srtTable[i].i << "    =>    " << srtTable[i].c << "\n";
    }
    														//writing binary char by char
  }
  if (out.fail()) 											//checking access and end of file
    cout << "error reading decoded.txt" << endl;
  out.close();
}

void constructBinaryTable(Row * feqRows, int t, string * & huffTable, int ansii) {

  for (int i = 0; i < ansii; ++i) {
    huffTable[i] = "";
  }
  int x, y; 												//x- 1st lowest index, y- 2nd lowest index
  for (int i = 0; i < t; ++i) {
    x = findMin(feqRows, t, -1);
    y = findMin(feqRows, t, x);

    addString(huffTable, feqRows[x].c, '1');

    feqRows[x].i += feqRows[y].i;

    addString(huffTable, feqRows[y].c, '0');

    feqRows[y].i = -1;
    feqRows[x].c = feqRows[x].c + feqRows[y].c;
  }
}

bool isEqual(string x, string y) {
  bool f = true;
  bool k = true;
  for (int i = 0; f && k; ++i) {					//matching possible condition to fail
    if (x[i] == '\0') f = false;
    if (y[i] == '\0') k = false;
    if (f != k) return false;
    if (x[i] != y[i]) return false;
  }
  return true;
}

int isMatched(Row * srtTable, int ansii, string str, int t) {
  for (int i = 0; i < t; ++i) {							//if(isEqual(srtTable[i].c, str)) return i;												
    if (!srtTable[i].c.compare(str)) return srtTable[i].i;
  }
  return -1;
}

void decodeText(Row * srtTable, int ansii, int t) {
  ofstream out("decoded.txt");
  ifstream in ("binary.txt");
  char c;
  if ( in .is_open() && out.is_open()) {
    string str = "";
    int val;
    while ( in .good()) {
      in .get(c);										//reading char by char while writing decoded text
      str = str + c;
      val = isMatched(srtTable, ansii, str, t);
      if (val != -1) {
        cout << str << " - " << endl;
        out.put((char) val);
        str = "";
        val = -1;
      } 												//writing binary char by char
    }
  }
  if (! in .eof() && in .fail() || out.fail()) 			//checking access and end of file
    cout << "error reading binary.txt, decoded.txt" << endl; in .close();
}

void encodeText(string * huffTable) {
  ofstream out("binary.txt");
  ifstream in ("text.txt");
  char c;
  if ( in .is_open() && out.is_open()) {
    while ( in .good()) {
      in .get(c);
      out << huffTable[(int) c]; 						//writing binary char by char
    }
  }
  if (! in .eof() && in .fail() || out.fail()) 			//checking access and end of file
    cout << "error reading text.txt" << endl; in .close();
}

void addString(string * & ary, string s, char c) {
  string tm;
  tm += c;
  for (int i = 0; s[i] != '\0'; ++i) {
    ary[(int) s[i]] = tm + ary[(int) s[i]];

  }
}

int findMin(Row * ary, int s, int ignore) {
  int min = INT_MAX;								//finding minimum key in array
  int idx = -1;
  for (int i = 0; i < s; ++i) {
    if (i != ignore) {
      if (ary[i].i > 0 && min > ary[i].i) {
        min = ary[i].i;
        idx = i;
      }
    }
  }
  return idx;
}

void populateFeq(int * feq) {
  ifstream in ("text.txt");
  char c;

  if ( in .is_open()) {
    while ( in .good()) {
      in .get(c);
      feq[(int) c]++; 								//reading char by char and incrementing frequency table
    }
  }
  if (! in .eof() && in .fail()) 					//checking access and end of file
    cout << "error reading text.txt" << endl; in .close();
}

void initialize_tables(int * feq, Row * & feqRows, int ansii, int & h) {
  int c = 0;
  for (int i = 0; i < ansii; ++i) {
    if (feq[i] > 0)
      c++;
  }

  feqRows = new Row[c];

  h = 0;
  for (int i = 0; i < ansii; ++i) {
    if (feq[i] > 0) {
      feqRows[h].c = (char) i;
      feqRows[h].i = feq[i];
      h++;
    }
  }
}

void readMetaData(Row * srtTable, int & t) {
  ifstream in ("codeTable.txt");
  string dump;
  getline( in , dump);									//skipping 1st two line
  getline( in , dump);
  while (getline( in , dump))
    t++;												//counting the table

  t = t / 2;											//bacause of whitespaces, it counts twice
  														//that is why dividing by two
  srtTable = new Row[t]; in .seekg(0, std::ios::beg);
  getline( in , dump);
  getline( in , dump);
  int c = 0;
  if ( in .is_open()) {
    while ( in .good()) {
      in >> srtTable[c].i; in >> dump; in >> srtTable[c].c;
      c++;
    }
  }
  if (! in .eof() && in .fail()) 						//checking access and end of file
    cout << "error reading codeTable.txt" << endl; in .close();

}

void initialize_feq(int * feq, int ansii) {
  for (int i = 0; i < ansii; ++i)						//populating ascii table to array
    feq[i] = 0;
}

int i_th_dig(int n, int k) {							//returning the i th digit
  for (int i = 1; i < k; ++i) n /= 10;
  return n % 10;
}

int num_dig(int n) {									//getting digit length
  if (n < 10) return 1;
  return 1 + num_dig(n / 10);
}

int max(Row * A, int s) {								//finding maximum value
  int mx = A[0].i;
  for (int i = 1; i < s; ++i) {
    if (A[i].i > mx) mx = A[i].i;
  }
  return mx;
}

void counting_sort(Row * A, int s, int k) {
  Row * B = new Row[s];
  int * F = new int[10];
  int i;
  for (i = 0; i < 10; ++i) F[i] = 0;					//initialize 0 to 9 array

  for (i = 0; i < s; ++i) {
    F[i_th_dig(A[i].i, k)]++;							//counting digit places and increment
  }
  for (i = 1; i < 10; ++i) F[i] = F[i] + F[i - 1];		//adding value from previous key
  int t, tm;
  for (i = s - 1; i >= 0; --i) {
    t = i_th_dig(A[i].i, k);
    tm = --F[t];
    B[tm].c = A[i].c;
    B[tm].i = A[i].i;
  }
  for (i = 0; i < s; ++i) {
    A[i].c = B[i].c;
    A[i].i = B[i].i;
  }
}

void radix_sort(Row * A, int s) {
  int nod = num_dig(max(A, s));						//getting max digit length
  for (int i = 1; i <= nod; ++i) {
    counting_sort(A, s, i);
  }
}

// text.txt
//
// algorithm is an unambiguous specification of how to solve a class of problems. Algorithms can perform calculation,
// data processing, automated reasoning, and other tasks.
//
//
//
//binary.txt
//
//1111100011110010111011011110001011110000111001111010110001001010101111110101010101101110101111100111110011111000110010
//1011011110101101100101010100101000010100000100110110001000100110001001101111101111100011101101010101110100010010101000
//0111110100010100101010111111010101001011101000111000101011000001010111110101001101000111111100101001010101110100010010
//1010000101101111101100111100011100000100111100101011001101010001011110001111001011101101111000101111000011100111100101
//0101001101111110101010100001010000011011100010011101101110011110101001101111100011100110101101100011111110111110001110
//1101011001101010101100011111011111111010100001011011111010011010000010010100101100011010110010110011010101111101101101
//1111101001111111101111000001011000101011011100000111110010111011010110001101011001011001101010111111010101100010101110
//101111000011100000110111010101111111100101000101101001010110011011001
//
//
//
//frequcency.txt
//
// Letter Frequency Chart
// -----------------------
//      =>    23
// ,    =>    3
// .    =>    3
// A    =>    1
// a    =>    16
// b    =>    2
// c    =>    7
// d    =>    3
// e    =>    8
// f    =>    4
// g    =>    5
// h    =>    4
// i    =>    10
// k    =>    1
// l    =>    7
// m    =>    6
// n    =>    9
// o    =>    16
// p    =>    4
// r    =>    8
// s    =>    13
// t    =>    10
// u    =>    5
// v    =>    1
//
//
//
//
//codeTable.txt
//
// Huffman Code Table
// ------------------
//      =>    1010
// ,    =>    1100110
// .    =>    1011001
// A    =>    100010111
// a    =>    1111
// b    =>    1100111
// c    =>    100110
// d    =>    1011000
// e    =>    100000
// f    =>    1000100
// g    =>    110010
// h    =>    1000011
// i    =>    11000
// k    =>    100010110
// l    =>    100011
// m    =>    100111
// n    =>    11010
// o    =>    1110
// p    =>    1000010
// r    =>    11011
// s    =>    10010
// t    =>    10111
// u    =>    101101
// v    =>    100010101
// w    =>    100010100
//
//
//
//decoded.txt
//
//algorithm is an unambiguous specification of how to solve a class of problems. Algorithms can perform calculation,
// data processing, automated reasoning, and other tasks..