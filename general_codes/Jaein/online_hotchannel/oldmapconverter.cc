#include "Mapping.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

std::map<std::string, int> port_to_id_map = {
    {"A1", 0}, {"A2", 1}, {"A3", 2}, {"B1", 3}, {"B2", 4}, {"B3", 5}, {"C1", 6}, {"C2", 7}, {"C3", 8}, {"D1", 9}, {"D2", 10}, {"D3", 11}};

void oldmapconverter(string inputfile ="mask_ch_south.txt")
{
    cout<<inputfile<<endl;
    ifstream file(inputfile); // Open the file
    string outfilename = "mask_ch_";
    size_t found = inputfile.find("south");
    bool IsSouth = false;
    if (found != string::npos)
    {
        cout << "THIS IS SOUTH" << endl;
        outfilename += "south_updated.txt";
        IsSouth = true;
    }
    else
    {
        cout << "THIS IS NORTH" << endl;
        outfilename += "north_updated.txt";
    }
    if (!file.is_open())
    { // If failed to open the file
        cout << "Cannot open the file." << endl;
        return 1;
    }
        ofstream writeFile(outfilename);
    if (!writeFile.is_open()) {
        cout << "Cannot open the file." << endl;
        return 1;
    }
    
    string line;
    while (getline(file, line))
    {                          // Read the file line by line
        stringstream ss(line); // Convert the line into stringstream
        string servername = "intt";
        int nth_number = 0;
        vector<int> numbers;
        int number;
        while (ss >> number)
        { // Extract numbers from the line
            numbers.push_back(number);
        }
        int ROC_ID = -1;        // 0~7
        int FelixCh_ID = -1;                                   // 0~13
        int port_ID =-1;                   // 0~11
        int chip_ID = -1;                                      // 1 ~ 26
        int chan_ID = -1;  
        // Process or print the numbers
        for (int num : numbers)
        {
            cout << num << " ";
            if(nth_number==0)
            {
                int server = num/2;
                if(!IsSouth)
                server += 4;
                servername = servername+to_string(server)+"_";
                ROC_ID = num;
            }
            if(nth_number==1)
            {
               servername = servername+to_string(num);
               FelixCh_ID = num; 
            }
            if(nth_number==2)
            {
                port_ID = num;
            }
            if(nth_number==3)
            {
                chip_ID = num;
            }
            if(nth_number==4)
            {
                chan_ID = num;
            }
            nth_number++;
        }
        cout << servername;
        cout << endl;
        string port = serverFC_toinfo_map.at(servername).Port; // Port name ex) A1, B2, C3,... # of Ports : 12
                                 // 0~127
        int original_port = port_ID;
        port_ID = port_to_id_map[port];
        writeFile << ROC_ID << " " << FelixCh_ID << " " << port_ID << " " << chip_ID << " " << chan_ID << " " << "0 0 0 0 0";
        if(original_port!=port_ID)
        {
            writeFile<<"DIFF";
        }
        writeFile<<endl; 
    }

    file.close(); // Close the file

    return 0;
}