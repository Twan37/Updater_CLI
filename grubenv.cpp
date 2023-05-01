#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  string filename = "/boot/grub/grubenv";
  ifstream infile(filename);

  if (!infile.is_open()) {
    cout << "Error: failed to open " << filename << endl;
    return 1;
  }

  string line;
  while (getline(infile, line)) {
    if (line.size() > 0 && line[0] != '#') {
      cout << line << endl;
    }
  }

  infile.close();
  return 0;
}