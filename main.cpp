 #include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <filesystem>
#include "include/json.hpp"

using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

template<typename K, typename V>
void map_to_json(const map<K, V>& m, string filename) {
    // Read the existing JSON file
    filename="OutputFiles/"+filename;
    filename.append(".json");
    
    json j;
    ifstream inFile(filename);
    if (inFile.is_open()) {
        inFile >> j;
        inFile.close();
    }
    

    // Append new data to the existing JSON object
    for (const auto& [key, value] : m) {
        j[key] = value;
    }

    // Write the updated JSON object back to the file
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << j.dump(4) << endl; // Pretty print with 4 spaces indentation
        outFile.close();
        cout << "JSON data has been updated in " << filename << endl;
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
}




void PrintVect(vector<string> vect) 
{
    for (unsigned int i = 0; i < vect.size(); i++) 
    {
        cout << i << " " << vect[i] << endl;
    }
}


vector<string> splitAndStore(const string& l) {
    vector<string> result;
    string token;
    istringstream tokenStream(l);
    char ch;

    while (tokenStream.get(ch)) {
        if (ch == '*' )
        {
            result.push_back(token);
            token.clear();
        } 
        else 
        {
            token += ch;
        }
    }

    // Push the last token if there is one
    result.push_back(token);
    return result;
}


void PrintMap(map<string, string> MappedElementsOfCurrentLine) {
    map<string, string> mp = MappedElementsOfCurrentLine;
    map<string, string>::iterator it = mp.begin();

    while (it != mp.end()) 
    {
        cout << it->first << ": " << it->second << endl;
        ++it;
    }

}

bool inProfandInst(string loopname,string SegmentName)
{
    map<string,string> temp = { {loopname,SegmentName} };

    
    map<string,string> uniques = {

        {"NM1","2310A"}, {"2310A","REF"},{"2310B","NM1"},{"2310B","REF"},
        {"2310D","NM1"}, {"2310D","REF"},{"2310E","NM1"},{"2310E","N3"},
        {"2310E","N4"}, {"2310F","NM1"},{"2330C","NM1"},{"2330C","REF"},
        {"2330D","NM1"},{"2330D","REF"},{"2330E","NM1"},{"2330E","REF"},
        {"2330F","NM1"},{"2330F","REF"},{"2330G","NM1"},{"2330G","REF"},
        {"2400","SV1"},{"2400","AMT"}, {"2300","DTP"}, {"2300","REF"} //UNIQUES
    };

    for (const auto& pair : temp) {
        auto it = uniques.find(pair.first);
        if (it != uniques.end() && it->second == pair.second) {
            return true;
        }
    }

    return false;
}
map<string,string> Map_Elements_Loop(vector<string> elements, string CL, map<string, string> pairs, string loopname,string SegmentSpecificIdentifier,string ClaimType) {
    //cout <<"\n";
    map<string, string> data_pairs = pairs;
    string CurrentLine = CL;
    vector<string> CL_Elements = elements;
    map<string, string> map_general;

    string Segment = "";
    for (unsigned int i = 0; i < CurrentLine.size(); i++) 
    {
        if (CurrentLine.at(i) == '*') 
        {
            break;
        } 
        else 
        {
            Segment = Segment + CurrentLine.at(i);
        }
    }

    for (unsigned int i = 0; i < CL_Elements.size(); i++) 
    {
        if ((CL_Elements[i].empty() || CL_Elements[i].at(0) == ' ') && CL_Elements[i].substr(0,3)!="CLM") 
        {
            continue;
        }
        string curr_val = Segment;
        string SegNum = to_string(i + 1);
        if (SegNum.size() < 2) 
        {
            SegNum = "0" + SegNum;
        }

          curr_val = curr_val + SegNum;
          if(SegmentSpecificIdentifier!="")
          {
            curr_val = curr_val +"_"+loopname+"_"+SegmentSpecificIdentifier;
          }
          else
            curr_val = curr_val +"_"+loopname;
        if(inProfandInst(loopname,Segment) && ClaimType =="I")
        {
            curr_val=curr_val+"_I";
        }
        

        if (pairs.find(curr_val) != pairs.end()) 
        {
            curr_val = pairs[curr_val]; // curr_val changes to formatted value
        } 
        map_general[curr_val] = CL_Elements[i];
    }
    return map_general;
}


map<string,string> Map_Elements_General(vector<string> elements, string CL, map<string, string> pairs) {
    //cout <<"\n";
    map<string, string> data_pairs = pairs;
    string CurrentLine = CL;
    vector<string> CL_Elements = elements;
    map<string, string> map_general;

    string Segment = "";
    for (unsigned int i = 0; i < CurrentLine.size(); i++) 
    {
        if (CurrentLine.at(i) == '*') 
        {
            break;
        } 
        else 
        {
            Segment = Segment + CurrentLine.at(i);
        }
    }

    for (unsigned int i = 0; i < CL_Elements.size(); i++) 
    {
        if ((CL_Elements[i].empty() || CL_Elements[i].at(0) == ' ') && CL_Elements[i].substr(0,3)!="CLM") 
        {
            continue;
        }

        string curr_val = Segment;

        string SegNum = to_string(i + 1);
        if (SegNum.size() < 2) 
        {
            SegNum = "0" + SegNum;
        }
          curr_val = curr_val + SegNum;
        if (pairs.find(curr_val) != pairs.end()) 
        {
            curr_val = pairs[curr_val]; // curr_val changes to formatted value
        } 
        map_general[curr_val] = CL_Elements[i];
    }
    return map_general;
}
void initialize_data_pairs(map<string,string>& DataElement_Name_Pairs)
{
    
    std::ifstream file("DataElementNamePairs.json");
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return;
    }
    json j;
    file >> j;
    DataElement_Name_Pairs = j.get<std::map<std::string, std::string>>();
    file.close();



    // final code does not use this, instead uses an existing json of these pairs


   /* DataElement_Name_Pairs["ISA01"] = "ISA01_Authorization Information Qualifier";
    DataElement_Name_Pairs["ISA02"] =  "ISA02_Authorization Information";
    DataElement_Name_Pairs["ISA03"] =  "ISA03_Security Information Qualifier";
    DataElement_Name_Pairs["ISA04"] =  "ISA04_Security Information";
    DataElement_Name_Pairs["ISA05"] =  "ISA05_Interchange ID Qualifier";
    DataElement_Name_Pairs["ISA06"] =  "ISA06_Interchange Sender ID";
    DataElement_Name_Pairs["ISA07"] =  "ISA07_Interchange ID Qualifier";
    DataElement_Name_Pairs["ISA08"] =  "ISA08_Interchange Receiver ID";
    DataElement_Name_Pairs["ISA09"] =  "ISA09_Interchange Date";
    DataElement_Name_Pairs["ISA10"] =  "ISA10_Interchange Time";
    DataElement_Name_Pairs["ISA11"] =  "ISA11_Interchange Control Standards Identifier";
    DataElement_Name_Pairs["ISA12"] =  "ISA12_Interchange Control Version Number";
    DataElement_Name_Pairs["ISA13"] =  "ISA13_Interchange Control Number";
    DataElement_Name_Pairs["ISA14"] =  "ISA14_Acknowledgement Requested";
    DataElement_Name_Pairs["ISA15"] =  "ISA15_Usage Indicator";
    DataElement_Name_Pairs["ISA16"] =  "ISA16_Component Element Separator";
    DataElement_Name_Pairs["IEA01_Trailer"] =  "IEA01_Number of Included Functional Groups";
    DataElement_Name_Pairs["IEA02_Trailer"] =  "IEA02_Interchange Control Number";
    DataElement_Name_Pairs["GS01_Header"] =  "GS01_Functional Identifier Code";
    DataElement_Name_Pairs["GS02_Header"] =  "GS02_Application Sender's Code";
    DataElement_Name_Pairs["GS03_Header"] =  "GS03_Application Receiver's Code";
    DataElement_Name_Pairs["GS04_Header"] =  "GS04_Date";
    DataElement_Name_Pairs["GS05_Header"] =  "GS05_Time";
    DataElement_Name_Pairs["GS06_Header"] =  "GS06_Group Control Number";
    DataElement_Name_Pairs["GS07_Header"] =  "GS07_Responsible Agency Code";
    DataElement_Name_Pairs["GS08_Header"] =  "GS08_Version/Release Industry ID Code";
    DataElement_Name_Pairs["GE01_Trailer"] =  "GE01_Number of Transaction Sets Included";
    DataElement_Name_Pairs["GE02_Trailer"] =  "GE02_Group Control Number";
    DataElement_Name_Pairs["ST01_Header"] =  "ST01_Transaction Set Identifier Code";
    DataElement_Name_Pairs["ST02_Header"] =  "ST02_Transaction Set Control Number";
    DataElement_Name_Pairs["ST03_Header"] =  "ST03_Implementation Convention Reference";
    DataElement_Name_Pairs["ST01"] =  "ST01_Transaction Set Identifier Code";
    DataElement_Name_Pairs["ST02"] =  "ST02_Transaction Set Control Number";
    DataElement_Name_Pairs["ST03"] =  "ST03_Implementation Convention Reference";

    DataElement_Name_Pairs["BHT01_Header"] = "BHT01_Hierarchical Structure Code";
    DataElement_Name_Pairs["BHT02_Header"] = "BHT02_Information Source, Subscriber, Dependent";
    DataElement_Name_Pairs["BHT03_Header"] = "BHT03_Reference Identification";
    DataElement_Name_Pairs["BHT04_Header"] = "BHT04_Date";
    DataElement_Name_Pairs["BHT05_Header"] = "BHT05_Time";
    DataElement_Name_Pairs["BHT06_Header"] = "BHT06_Transaction Type Code";

    
 //NM1_1000A
    DataElement_Name_Pairs["NM101_1000A"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_1000A"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_1000A"] = "NM103_Submitter Last or Organization Name";
    DataElement_Name_Pairs["NM104_1000A"] = "NM104_Submitter First Name";
    DataElement_Name_Pairs["NM105_1000A"] = "NM105_Submitter Middle Name or Initial";
    DataElement_Name_Pairs["NM106_1000A"] = "NM106_Name Prefix";
    DataElement_Name_Pairs["NM107_1000A"] = "NM107_Name Suffix";


    DataElement_Name_Pairs["NM108_1000A"] = "NM108_Submitter Identifier";
    DataElement_Name_Pairs["NM109_1000A"] = "NM109_Submitter Primary Identifier";
//NM1_1000B
    DataElement_Name_Pairs["NM101_1000B"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_1000B"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_1000B"] = "NM103_Receiver Name";
    DataElement_Name_Pairs["NM108_1000B"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_1000B"] = "NM109_Receiver Primary Identifier";

//NM1_2010AA
    DataElement_Name_Pairs["NM101_2010AA"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2010AA"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2010AA"] = "NM103_Billing Provider Last or Organizational Name";
    DataElement_Name_Pairs["NM104_2010AA"] = "NM104_Billing Provider First Name";
    DataElement_Name_Pairs["NM105_2010AA"] = "NM105_Billing Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2010AA"] = "NM107_Billing Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2010AA"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2010AA"] = "NM109_Billing Provider Identifier";
//NM1_2010AB
    DataElement_Name_Pairs["NM101_2010AB"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2010AB"] = "NM102_Entity Type Qualifier";
//NM1_2010BA
    DataElement_Name_Pairs["NM101_2010BA"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2010BA"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2010BA"] = "NM103_Subscriber Last Name";
    DataElement_Name_Pairs["NM104_2010BA"] = "NM104_Subscriber First  Name";
    DataElement_Name_Pairs["NM105_2010BA"] = "NM105_Subscriber Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2010BA"] = "NM107_Subscriber Name Suffix";
    DataElement_Name_Pairs["NM108_2010BA"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2010BA"] = "NM109_Subscriber Primary Identifier";
//NM1_2010BB
    DataElement_Name_Pairs["NM101_2010BB"] = "NM101_Entity Identifier code";
    DataElement_Name_Pairs["NM102_2010BB"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2010BB"] = "NM103_Payer Name";
    DataElement_Name_Pairs["NM108_2010BB"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2010BB"] = "NM109_Payer Identifier";
//NM1_2310A
    DataElement_Name_Pairs["NM101_2310A"] = "NM101_Entity Id Code";
    DataElement_Name_Pairs["NM102_2310A"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310A"] = "NM103_Name or organization name";
    DataElement_Name_Pairs["NM104_2310A"] = "NM104_Name First";
    DataElement_Name_Pairs["NM105_2310A"] = "NM105_Name Middle";
    DataElement_Name_Pairs["NM107_2310A"] = "NM107_Name Suffix";
    DataElement_Name_Pairs["NM108_2310A"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310A"] = "NM109_Identification Code";

    DataElement_Name_Pairs["NM101_2310A_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310A_I"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310A_I"] = "NM103_Attending Provider Last Name";
    DataElement_Name_Pairs["NM104_2310A_I"] = "NM104_Attending Provider First Name";
    DataElement_Name_Pairs["NM105_2310A_I"] = "NM105_Attending Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2310A_I"] = "NM107_Attending Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2310A_I"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310A_I"] = "NM109_Attending Provider Primary Identifier";

    DataElement_Name_Pairs["NM101_2310A_DN"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310A_DN"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310A_DN"] = "NM103_Referring Provider Last Name";
    DataElement_Name_Pairs["NM104_2310A_DN"] = "NM104_Referring Provider First Name";
    DataElement_Name_Pairs["NM105_2310A_DN"] = "NM105_Referring Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2310A_DN"] = "NM105_Referring Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2310A_DN"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310A_DN"] = "NM109_Referring Provider Identifier";
//NM1_2310B
    DataElement_Name_Pairs["NM101_2310B"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310B"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310B"] = "NM103_Rendering Provider Last or Organization Name";
    DataElement_Name_Pairs["NM104_2310B"] = "NM104_Rendering Provider First Name";
    DataElement_Name_Pairs["NM105_2310B"] = "NM105_Rendering Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2310B"] = "NM107_Rendering Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2310B"] = "NM108_Rendering Provider Identifier";
    DataElement_Name_Pairs["NM109_2310B"] = "NM109_Identification Code";

    DataElement_Name_Pairs["NM101_2310B_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310B_I"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310B_I"] = "NM103_Operating Physician Last Name";
    DataElement_Name_Pairs["NM104_2310B_I"] = "NM104_Operating Physician First Name";
    DataElement_Name_Pairs["NM105_2310B_I"] = "NM105_Operating Physician Middle Name";
    DataElement_Name_Pairs["NM107_2310B_I"] = "NM107_Operating Physician Name Suffix";
    DataElement_Name_Pairs["NM108_2310B_I"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310B_I"] = "NM109_Operating Physician Primary Identifier";
//NM1_2310C
    DataElement_Name_Pairs["NM101_2310C"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310C"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310C"] = "NM103_Laboratory or Facility Name";
    DataElement_Name_Pairs["NM108_2310C"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310C"] = "NM109_Laboratory or Facility Primary Identifier";
//NM1_2330A
    DataElement_Name_Pairs["NM101_2330A"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330A"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2330A"] = "NM103_Other Insured Last Name";
    DataElement_Name_Pairs["NM104_2330A"] = "NM104_Other Insured First Name";
    DataElement_Name_Pairs["NM105_2330A"] = "NM105_Other Insured Middle Name";
    DataElement_Name_Pairs["NM107_2330A"] = "NM107_Other Insured Name Suffix";
    DataElement_Name_Pairs["NM108_2330A"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2330A"] = "NM109_Other Insured Identifier";
//NM1_2330B
    DataElement_Name_Pairs["NM101_2330B"] = "NM101_Entity Identifier";
    DataElement_Name_Pairs["NM102_2330B"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2330B"] = "NM103_Other Payer Organization Name";
    DataElement_Name_Pairs["NM108_2330B"] = "NM108_ID Code Qualifier";
    DataElement_Name_Pairs["NM109_2330B"] = "NM109_Other Payer Primary Identifier";

    DataElement_Name_Pairs["NM101_2330C"] = "NM101_Entity Identifier";
    DataElement_Name_Pairs["NM102_2330C"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330C_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330C_I"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330D"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330D"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330D_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330D_I"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330E"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330E"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330E_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330E_I"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330F"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330F"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330F_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330F_I"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330G_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330G_I"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330H"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330H"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2330I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2330I"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2010AC"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2010AC"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2010AC"] = "NM103_Pay-to Plan Organizational Name";
    DataElement_Name_Pairs["NM104_2010AC"] = "NM104_First Name";
    DataElement_Name_Pairs["NM105_2010AC"] = "NM105_Name Middle";
    DataElement_Name_Pairs["NM107_2010AC"] = "NM107_Name Suffix";
    DataElement_Name_Pairs["NM108_2010AC"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2010AC"] = "NM109_Pay-To Plan Primary Identifier";

    DataElement_Name_Pairs["NM101_2010CA"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2010CA"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2010CA"] = "NM103_Patient Last Name";
    DataElement_Name_Pairs["NM104_2010CA"] = "NM104_Patient First Name";
    DataElement_Name_Pairs["NM105_2010CA"] = "NM105_Patient Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2010CA"] = "NM107_Patient Name Suffix";

    DataElement_Name_Pairs["NM101_2310D"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310D"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310D"] = "NM103_Rendering Provider Last or Organization Name";
    DataElement_Name_Pairs["NM104_2310D"] = "NM104_Rendering Provider First Name";
    DataElement_Name_Pairs["NM105_2310D"] = "NM105_Rendering Provider Middle Name";
    DataElement_Name_Pairs["NM107_2310D"] = "NM107_Rendering Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2310D"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310D"] = "NM109_Rendering Provider Identifier";

    DataElement_Name_Pairs["NM101_2310D_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310D_I"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310D_I"] = "NM103_Rendering Provider Last Name";
    DataElement_Name_Pairs["NM104_2310D_I"] = "NM104_Rendering Provider First Name";
    DataElement_Name_Pairs["NM105_2310D_I"] = "NM105_Rendering Provider Middle Name";
    DataElement_Name_Pairs["NM107_2310D_I"] = "NM107_Rendering Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2310D_I"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310D_I"] = "NM109_Rendering Provider Identifier";

    DataElement_Name_Pairs["NM101_2310E"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310E"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2310E_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310E_I"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310E_I"] = "NM103_Laboratory or Facility Name";
    DataElement_Name_Pairs["NM108_2310E_I"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310E_I"] = "NM109_Laboratory or Facility Primary Identifier";

    DataElement_Name_Pairs["NM101_2310F"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310F"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310F"] = "NM102_Ambulance Drop-off Location";

    DataElement_Name_Pairs["NM101_2310F_I"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2310F_I"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2310F_I"] = "NM103_Referring Provider Last Name";
    DataElement_Name_Pairs["NM104_2310F_I"] = "NM104_Referring Provider First Name";
    DataElement_Name_Pairs["NM105_2310F_I"] = "NM105_Referring Provider Middle Name";
    DataElement_Name_Pairs["NM107_2310F_I"] = "NM107_Referring Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2310F_I"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2310F_I"] = "NM109_Referring Provider Identifier";


    DataElement_Name_Pairs["NM101_2420A"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420A"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420A"] = "NM103_Rendering Provider Last or Organization Name";
    DataElement_Name_Pairs["NM104_2420A"] = "NM104_Rendering Provider First Name";
    DataElement_Name_Pairs["NM105_2420A"] = "NM105_Rendering Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2420A"] = "NM107_Rendering Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2420A"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2420A"] = "NM109_Rendering Provider Identifier";

    DataElement_Name_Pairs["NM101_2420B"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420B"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM108_2420B"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2420B"] = "NM109_Purchased Service Provider Identifier";

    DataElement_Name_Pairs["NM101_2420C"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420C"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420C"] = "NM103_Laboratory or Facility Name";
    DataElement_Name_Pairs["NM108_2420C"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2420C"] = "NM109_Laboratory or Facility Primary Identifier";

    DataElement_Name_Pairs["NM101_2420D"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420D"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420D"] = "NM103_Supervising Provider Last Name";
    DataElement_Name_Pairs["NM104_2420D"] = "NM104_Supervising Provider First Name";
    DataElement_Name_Pairs["NM105_2420D"] = "NM105_Supervising Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2420D"] = "NM107_Supervising Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2420D"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2420D"] = "NM109_Supervising Provider Identifier";

    DataElement_Name_Pairs["NM101_2420E"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420E"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420E"] = "NM103_Ordering Provider Last Name";
    DataElement_Name_Pairs["NM104_2420E"] = "NM104_Ordering Provider First Name";
    DataElement_Name_Pairs["NM105_2420E"] = "NM105_Ordering Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2420E"] = "NM107_Ordering Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2420E"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2420E"] = "NM109_Ordering Provider Identifier";

    DataElement_Name_Pairs["NM101_2420F"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420F"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420F"] = "NM103_Referring Provider Last Name";
    DataElement_Name_Pairs["NM104_2420F"] = "NM104_Referring Provider First Name";
    DataElement_Name_Pairs["NM105_2420F"] = "NM105_Referring Provider Middle Name or Initial";
    DataElement_Name_Pairs["NM107_2420F"] = "NM107_Referring Provider Name Suffix";
    DataElement_Name_Pairs["NM108_2420F"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109_2420F"] = "NM109_Referring Provider Identifier";

    DataElement_Name_Pairs["NM101_2420G"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420G"] = "NM102_Entity Type Qualifier";

    DataElement_Name_Pairs["NM101_2420G"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420G"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420G"] = "NM103_Ambulance Drop-off Location";

    DataElement_Name_Pairs["NM101_2420H"] = "NM101_Entity Identifier Code";
    DataElement_Name_Pairs["NM102_2420H"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103_2420H"] = "NM103_Ambulance Drop-off Location";

// HL_2000A
    DataElement_Name_Pairs["HL01_2000A"] = "HL01_Hierarchical ID Number";
    DataElement_Name_Pairs["HL03_2000A"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04_2000A"] = "HL04_Hierarchical Child Code";
// HL_2000B
    DataElement_Name_Pairs["HL01_2000B_1"] = "HL01_Hierarchical Level";
    DataElement_Name_Pairs["HL02_2000B_1"] = "HL02_Hierarchical Parent ID Number";
    DataElement_Name_Pairs["HL03_2000B_1"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04_2000B_1"] = "HL04_Hierarchical Child Code";

    DataElement_Name_Pairs["HL01_2000B_2"] = "HL01_Hierarchical ID Number";
    DataElement_Name_Pairs["HL02_2000B_2"] = "HL02_Hierarchical Parent ID Number";
    DataElement_Name_Pairs["HL03_2000B_2"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04_2000B_2"] = "HL04_Hierarchical Child Code";

    DataElement_Name_Pairs["HL01_2000B"] = "HL01_Hierarchical ID Number";
    DataElement_Name_Pairs["HL02_2000B"] = "HL02_Hierarchical Parent ID Number";
    DataElement_Name_Pairs["HL03_2000B"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04_2000B"] = "HL04_Hierarchical Child Code";

    DataElement_Name_Pairs["HL01_2000C_3"] = "HL01_Hierarchical ID Number";
    DataElement_Name_Pairs["HL02_2000C_3"] = "HL02_Hierarchical Parent ID Number";
    DataElement_Name_Pairs["HL03_2000C_3"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04_2000C_3"] = "HL04_Hierarchical Child Code";


    DataElement_Name_Pairs["HL01_2000C"] = "HL01_Hierarchical ID Number";
    DataElement_Name_Pairs["HL02_2000C"] = "HL02_Hierarchical Parent ID Number";
    DataElement_Name_Pairs["HL03_2000C"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04_2000C"] = "HL04_Hierarchical Child Code";
//HL_general (for cases where loop isnt found)

    DataElement_Name_Pairs["HL01"] = "HL01_Hierarchical ID Number";
    DataElement_Name_Pairs["HL02"] = "HL02_Hierarchical Parent ID Number";
    DataElement_Name_Pairs["HL03"] = "HL03_Hierarchical Level Code";
    DataElement_Name_Pairs["HL04"] = "HL04_Hierarchical Child Code";

    DataElement_Name_Pairs["CLM01"] = "CLM01_Claim Submitter's Identifier";
    DataElement_Name_Pairs["CLM02"] = "CLM02_Monetary Amount";
    DataElement_Name_Pairs["CLM03"] = "CLM03_Claim Filling Indicator Code";
    DataElement_Name_Pairs["CLM04"] = "CLM04_Non-Institutional Claim Type Code";
    DataElement_Name_Pairs["CLM05"] = "CLM05_Health Care Service Location Information";

    DataElement_Name_Pairs["CLM06"] = "CLM06_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CLM07"] = "CLM07_Provider Accept Assignment Code";
    DataElement_Name_Pairs["CLM08"] = "CLM08_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CLM09"] = "CLM09_Release of Information Code";
    DataElement_Name_Pairs["CLM10"] = "CLM10_Patient Signature Source Code";
    DataElement_Name_Pairs["CLM11"] = "CLM11_Related Causes Information";
    DataElement_Name_Pairs["CLM12"] = "CLM12_Special Program Code";
    DataElement_Name_Pairs["CLM13"] = "CLM13_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CLM14"] = "CLM14_Level of Service Code";
    DataElement_Name_Pairs["CLM15"] = "CLM15_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CLM16"] = "CLM16_Provider Agreement Code";
    DataElement_Name_Pairs["CLM17"] = "CLM17_Claim Status Code";
    DataElement_Name_Pairs["CLM18"] = "CLM18_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CLM19"] = "CLM19_Claim Submission Reason Code";
    DataElement_Name_Pairs["CLM20"] = "CLM20_Delay Reason Code";

    DataElement_Name_Pairs["CLM01_2300"] = "CLM01_Patient Control Number";
    DataElement_Name_Pairs["CLM02_2300"] = "CLM02_Total Claim Charge Amount";
    DataElement_Name_Pairs["CLM05_2300"] = "CLM05_Health Care Service Location Information";
    DataElement_Name_Pairs["CLM06_2300"] = "CLM06_Provider or Supplier Signature Indicator";
    DataElement_Name_Pairs["CLM07_2300"] = "CLM07_Assignment or Plan Participation Code";
    DataElement_Name_Pairs["CLM08_2300"] = "CLM08_Benefits Assignment Certification Indicator";
    DataElement_Name_Pairs["CLM09_2300"] = "CLM09_Release of Information Code";
    DataElement_Name_Pairs["CLM10_2300"] = "CLM10_Patient Signature Source Code";
    DataElement_Name_Pairs["CLM11_2300"] = "CLM11_Related Causes Information";
    DataElement_Name_Pairs["CLM12_2300"] = "CLM12_Special Program Indicator";
    DataElement_Name_Pairs["CLM20_2300"] = "CLM20_Daily Reason Code";


    DataElement_Name_Pairs["PRV01"] = "PRV01_Provider Code ";
    DataElement_Name_Pairs["PRV02"] = "PRV02_Reference Identification Qualifier";
    DataElement_Name_Pairs["PRV03"] = "PRV03_Reference Identification";
    DataElement_Name_Pairs["PRV04"] = "PRV04_State or Province Code";
    DataElement_Name_Pairs["PRV05"] = "PRV05_Provider Specialty Information";
    DataElement_Name_Pairs["PRV06"] = "PRV06_Provider Organization Code";

    DataElement_Name_Pairs["PRV01_2000A"] = "PRV01_Provider Code ";
    DataElement_Name_Pairs["PRV02_2000A"] = "PRV02_Reference Identification Qualifier";
    DataElement_Name_Pairs["PRV03_2000A"] = "PRV03_Provider Taxonomy Code";
    DataElement_Name_Pairs["PRV04_2000A"] = "PRV04_State or Province Code";
    DataElement_Name_Pairs["PRV05_2000A"] = "PRV05_Provider Specialty Information";
    DataElement_Name_Pairs["PRV06_2000A"] = "PRV06_Provider Organization Code";

    DataElement_Name_Pairs["PRV01_2310B"] = "PRV01_Provider Code";
    DataElement_Name_Pairs["PRV02_2310B"] = "PRV02_Reference Identification Qualifier";
    DataElement_Name_Pairs["PRV03_2310B"] = "PRV03_Provider Taxonomy Code";

    DataElement_Name_Pairs["PRV01_2310A"] = "PRV01_Provider Code";
    DataElement_Name_Pairs["PRV02_2310A"] = "PRV02_Reference Identification Qualifier";
    DataElement_Name_Pairs["PRV03_2310A"] = "PRV03_Provider Taxonomy Code";

    DataElement_Name_Pairs["PRV01_2420A"] = "PRV01_Provider Code";
    DataElement_Name_Pairs["PRV02_2420A"] = "PRV02_Reference Identification Qualifier";
    DataElement_Name_Pairs["PRV03_2420A"] = "PRV03_Provider Taxonomy Code";
    
    
    DataElement_Name_Pairs["N301"] = "N301_Address Information";
    DataElement_Name_Pairs["N302"] = "N302_Address Information Second Address Line";

    DataElement_Name_Pairs["N301_2010AA"] = "N301_Billing Provider Address Line";
    DataElement_Name_Pairs["N302_2010AA"] = "N302_Billing Provider Address Line";

    DataElement_Name_Pairs["N301_2010AB"] = "N301_Pay-To Address Line";
    DataElement_Name_Pairs["N302_2010AB"] = "N302_Pay-To Address Line";

    DataElement_Name_Pairs["N301_2010AC"] = "N301_Pay-To Plan Address Line";
    DataElement_Name_Pairs["N302_2010AC"] = "N302_Pay-To Plan Address Line";

    DataElement_Name_Pairs["N301_2010BA"] = "N301_Subscriber Address Line";
    DataElement_Name_Pairs["N302_2010BA"] = "N302_Subscriber Address Line";

    DataElement_Name_Pairs["N301_2010BB"] = "N301_Payer Address Line";
    DataElement_Name_Pairs["N302_2010BB"] = "N302_Payer Address Line";

    DataElement_Name_Pairs["N301_2010CA"] = "N301_Patient Address Line";
    DataElement_Name_Pairs["N302_2010CA"] = "N302_Patient Address Line";

    DataElement_Name_Pairs["N301_2310C"] = "N301_Laboratory or Facility Address Line";
    DataElement_Name_Pairs["N302_2310C"] = "N302_Laboratory or Facility Address Line";

    DataElement_Name_Pairs["N301_2310E"] = "N301_Ambulance Pick-up Address Line";
    DataElement_Name_Pairs["N302_2310E"] = "N302_Ambulance Pick-up Address Line";

    DataElement_Name_Pairs["N301_2310E_I"] = "N301_Laboratory or Facility Address Line";
    DataElement_Name_Pairs["N302_2310E_I"] = "N302_Laboratory or Facility Address Line";

    DataElement_Name_Pairs["N301_2310F"] = "N301_Ambulance Drop-off Address Line";
    DataElement_Name_Pairs["N302_2310F"] = "N302_Ambulance Drop-off Address Line";

    DataElement_Name_Pairs["N301_2330A"] = "N301_Other Subscriber Address Line";
    DataElement_Name_Pairs["N302_2330A"] = "N302_Other Insured Address Line";

    DataElement_Name_Pairs["N301_2330B"] = "N301_Other Payer Address Line";
    DataElement_Name_Pairs["N302_2330B"] = "N302_Other Payer Address Line";

    DataElement_Name_Pairs["N301_2420C"] = "N301_Laboratory or Facility Address Line";
    DataElement_Name_Pairs["N302_2420C"] = "N302_Laboratory or Facility Address Line";

    DataElement_Name_Pairs["N301_2420E"] = "N301_Ordering Provider Address Line";
    DataElement_Name_Pairs["N302_2420E"] = "N302_Ordering Provider Address Line";

    DataElement_Name_Pairs["N301_2420G"] = "N301_Ambulance Pick-up Address Line";
    DataElement_Name_Pairs["N302_2420G"] = "N302_Ambulance Pick-up Address Line";

    DataElement_Name_Pairs["N301_2420H"] = "N301_Ambulance Drop-off Address Line";
    DataElement_Name_Pairs["N302_2420H"] = "N302_Ambulance Drop-off Address Line";

    DataElement_Name_Pairs["N401"] = "N401_City";
    DataElement_Name_Pairs["N402"] = "N402_State";
    DataElement_Name_Pairs["N403"] = "N403_Zip";
    DataElement_Name_Pairs["N404"] = "N404_Country Code";
    DataElement_Name_Pairs["N407"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2010AA"] = "N401_Billing Provider City Name";
    DataElement_Name_Pairs["N402_2010AA"] = "N402_Billing Provider State or Province Code";
    DataElement_Name_Pairs["N403_2010AA"] = "N403_Billing Provider Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2010AA"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2010AA"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2010AB"] = "N401_Pay-to Address City Name";
    DataElement_Name_Pairs["N402_2010AB"] = "N402_Pay-to Address State Code";
    DataElement_Name_Pairs["N403_2010AB"] = "N403_Pay-to Address Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2010AB"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2010AB"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2010AC"] = "N401_Pay-to Plan City Name";
    DataElement_Name_Pairs["N402_2010AC"] = "N402_Pay-to Plan State or Province Code";
    DataElement_Name_Pairs["N403_2010AC"] = "N403_Pay-to Plan Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2010AC"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2010AC"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2010BA"] = "N401_Subscriber City Name";
    DataElement_Name_Pairs["N402_2010BA"] = "N402_Subscriber State Code";
    DataElement_Name_Pairs["N403_2010BA"] = "N403_Subscriber Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2010BA"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2010BA"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2010BB"] = "N401_Payer City Name";
    DataElement_Name_Pairs["N402_2010BB"] = "N402_Payer State or Province Code";
    DataElement_Name_Pairs["N403_2010BB"] = "N403_Payer Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2010BB"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2010BB"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2010CA"] = "N401_Patient City Name";
    DataElement_Name_Pairs["N402_2010CA"] = "N402_Patient State Code";
    DataElement_Name_Pairs["N403_2010CA"] = "N403_Patient Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2010CA"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2010CA"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2310C"] = "N401_Laboratory or Facility City Name";
    DataElement_Name_Pairs["N402_2310C"] = "N402_Laboratory or Facility State or Province Code";
    DataElement_Name_Pairs["N403_2310C"] = "N403_Laboratory or Facility Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2310C"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2310C"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2310E"] = "N401_Ambulance Pick-up City Name";
    DataElement_Name_Pairs["N402_2310E"] = "N402_Ambulance Pick-up State or Province Code";
    DataElement_Name_Pairs["N403_2310E"] = "N403_Ambulance Pick-up Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2310E"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2310E"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2310E_I"] = "N401_Laboratory or Facility City Name";
    DataElement_Name_Pairs["N402_2310E_I"] = "N402_Laboratory or Facility State or Province Code";
    DataElement_Name_Pairs["N403_2310E_I"] = "N403_Laboratory or Facility Postal Zone or Zip Code";
    DataElement_Name_Pairs["N404_2310E_I"] = "N404_Laboratory/Facility Country Code";
    DataElement_Name_Pairs["N407_2310E_I"] = "N407_Country Subdivision Code";



    DataElement_Name_Pairs["N401_2310F"] = "N401_Ambulance Drop-off City Name";
    DataElement_Name_Pairs["N402_2310F"] = "N402_Ambulance Drop-off State or Province Code";
    DataElement_Name_Pairs["N403_2310F"] = "N403_Ambulance Drop-off Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2310F"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2310F"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2330A"] = "N401_Other Subscriber City Name";
    DataElement_Name_Pairs["N402_2330A"] = "N402_Other Subscriber State or Province Code";
    DataElement_Name_Pairs["N403_2330A"] = "N403_Other Subscriber Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2330A"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2330A"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2330B"] = "N401_Other Payer City Name";
    DataElement_Name_Pairs["N402_2330B"] = "N402_Other Payer State or Province Code";
    DataElement_Name_Pairs["N403_2330B"] = "N403_Other Payer Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2330B"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2330B"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2330B"] = "N401_Laboratory or Facility City Name";
    DataElement_Name_Pairs["N402_2330B"] = "N402_Laboratory or Facility State or Province Code";
    DataElement_Name_Pairs["N403_2330B"] = "N403_Laboratory or Facility Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2330B"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2330B"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2420E"] = "N401_Ordering Provider City Name";
    DataElement_Name_Pairs["N402_2420E"] = "N402_Ordering Provider State or Province Code";
    DataElement_Name_Pairs["N403_2420E"] = "N403_Ordering Provider Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2420E"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2420E"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2420G"] = "N401_Ambulance Pick-up City Name";
    DataElement_Name_Pairs["N402_2420G"] = "N402_Ambulance Pick-up State or Province Code";
    DataElement_Name_Pairs["N403_2420G"] = "N403_Ambulance Pick-up Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2420G"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2420G"] = "N407_Country Subdivision Code";

    DataElement_Name_Pairs["N401_2420H"] = "N401_Ambulance Drop-off City Name";
    DataElement_Name_Pairs["N402_2420H"] = "N402_Ambulance Drop-off State or Province Code";
    DataElement_Name_Pairs["N403_2420H"] = "N403_Ambulance Drop-off Postal Zone or ZIP Code";
    DataElement_Name_Pairs["N404_2420H"] = "N404_Country Code";
    DataElement_Name_Pairs["N407_2420H"] = "N407_Country Subdivision Code";



    DataElement_Name_Pairs["REF01_2010AA_0B"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010AA_0B"] = "REF02_Billing Provider Tax Identification Number";

    DataElement_Name_Pairs["REF01_2010AA_EI"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010AA_EI"] = "REF02_Billing Provider Tax Identification Number";

    DataElement_Name_Pairs["REF01_2010AA_2U"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010AA_2U"] = "REF02_Pay-to Plan Identification Number";

    DataElement_Name_Pairs["REF01_2010BB_2U"] ="REF01_Payer Identification Number Qualifier";
    DataElement_Name_Pairs["REF02_2010BB_2U"] ="REF02_Payer Identification Number";

    DataElement_Name_Pairs["REF01_2010AA_EI"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010AA_EI"] = "REF02_Pay-to Plan Secondary Identifier";

    DataElement_Name_Pairs["REF01_2010AC_2U"] ="REF01_Payer Identification Number Qualifier";
    DataElement_Name_Pairs["REF02_2010AC_2U"] ="REF02_Payer Identification Number";

    DataElement_Name_Pairs["REF01_2010AC_EI"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010AC_EI"] = "REF02_Pay-to Plan Secondary Identifier";

    DataElement_Name_Pairs["REF01_2010BA_SY"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010BA_SY"] = "REF02_Subscriver Supplemental Identifier";

    DataElement_Name_Pairs["REF01_2010BA_Y4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010BA_Y4"] = "REF02_Property Casualty Claim Number";

    DataElement_Name_Pairs["REF01_2010BB_FY"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010BB_FY"] = "REF02_Payer Secondary Identifier";

    DataElement_Name_Pairs["REF01_2010BB_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010BB_G2"] = "REF02_Billing Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2010CA_Y4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010CA_Y4"] = "REF02_Billing Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2010CA_SY"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2010CA_SY"] = "REF02_Property Casualty Claim Number";

    DataElement_Name_Pairs["REF01_2300_4N"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_4N"] = "REF02_Service Authorization Exception Code";

    DataElement_Name_Pairs["REF01_2300_4N_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_4N_I"] = "REF02_Service Authorization Exception Code";

    DataElement_Name_Pairs["REF01_2300_F5"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_F5"] = "REF02_Medicare Section 4081 Indicator";

    DataElement_Name_Pairs["REF01_2300_EW"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_EW"] = "REF02_Mammography Certification Number";

    DataElement_Name_Pairs["REF01_2300_9F"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_9F"] = "REF02_Referral Number";

    DataElement_Name_Pairs["REF01_2300_9F_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_9F_I"] = "REF02_Service Authorization Exception Code";

    DataElement_Name_Pairs["REF01_2300_G1"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_G1"] = "REF02_Prior Authorization Number";

    DataElement_Name_Pairs["REF01_2300_G1_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_G1_I"] = "REF02_Prior Authorization or Referral Number";

    DataElement_Name_Pairs["REF01_2300_G4_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_G4_I"] = "REF02_PRO Number";

    DataElement_Name_Pairs["REF01_2300_F8"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_F8"] = "REF02_Payer Claim Control Number";

    DataElement_Name_Pairs["REF01_2300_F8_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_F8_I"] = "REF02_Claim Original Reference Number";

    DataElement_Name_Pairs["REF01_2300_X4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_X4"] = "REF02_Clinical Laboratory Improvement Amendment Number";
    
    DataElement_Name_Pairs["REF01_2300_9A"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_9A"] = "REF02_Repriced Claim Reference Number";

    DataElement_Name_Pairs["REF01_2300_9A_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_9A_I"] = "REF02_Repriced Claim Reference Number";

    DataElement_Name_Pairs["REF01_2300_9C"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_9C"] = "REF02_Repriced Claim Reference Number";

    DataElement_Name_Pairs["REF01_2300_9C_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_9C_I"] = "REF02_ Adjusted Repriced Claim Reference Number";

    DataElement_Name_Pairs["REF01_2300_LX"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_LX"] = "REF02_Investigational Device Exemption Identifier";

    DataElement_Name_Pairs["REF01_2300_LX_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_LX_I"] = "REF02_Investigational Device Exemption Number";

    DataElement_Name_Pairs["REF01_2300_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_LU_I"] = "REF02_Auto Accident State or Province";

    DataElement_Name_Pairs["REF01_2300_D9"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_D9"] = "REF02_Value Added Network Trace Number";

    DataElement_Name_Pairs["REF01_2300_D9_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_D9_I"] = "REF02_Clearinghouse Trace Number";

    DataElement_Name_Pairs["REF01_2300_P4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_P4"] = "REF02_Demonstration Project Identifier";

    DataElement_Name_Pairs["REF01_2300_P4_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_P4_I"] = "REF02_Demonstration Project Identifier";

    DataElement_Name_Pairs["REF01_2300_1J"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_1J"] = "REF02_Care Plan Oversight Number";

    DataElement_Name_Pairs["REF01_2300_EA"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_EA"] = "REF02_Medical Record Number";

    DataElement_Name_Pairs["REF01_2300_EA_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2300_EA_I"] = "REF02_Medical Record Number";

    DataElement_Name_Pairs["REF01_2310A_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310A_G2"] = "REF02_Referring Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310E_0B"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310E_0B"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310E_LU"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310E_LU"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310E_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310E_G2"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310E_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310E_G2_I"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310E_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310E_0B_I"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310E_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310E_LU_I"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310A_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310A_G2_I"] = "REF02_Attending Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310A_1G_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310A_1G_I"] = "REF02_Attending Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310A_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310A_0B_I"] = "REF02_Attending Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310A_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310A_LU_I"] = "REF02_Attending Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310B_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310B_G2"] = "REF02_Rendering Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310B_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310B_G2_I"] = "REF02_Operating Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310B_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310B_0B_I"] = "REF02_Operating Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310B_1G_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310B_1G_I"] = "REF02_Operating Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310B_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310B_LU_I"] = "REF02_Operating Physician Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310C_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310C_G2"] = "REF02_Laboratory or Facility Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310D_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310D_G2"] = "REF02_Supervising Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310D_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310D_G2_I"] = "REF02_Rendering Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310D_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310D_0B_I"] = "REF02_Rendering Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310D_1G_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310D_1G_I"] = "REF02_Rendering Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2310D_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2310D_LU_I"] = "REF02_Rendering Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330A_SY"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330A_SY"] = "REF02_Other Insured Additional Identifier";

    DataElement_Name_Pairs["REF01_2330B_2U"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330B_2U"] = "REF02_Other Payer Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330B_G1"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330B_G1"] = "REF02_Other Payer Prior Authorization Number";

    DataElement_Name_Pairs["REF01_2330B_9F"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330B_9F"] = "REF02_Other Payer Prior Authorization or Referral Number";

    DataElement_Name_Pairs["REF01_2330B_T4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330B_T4"] = "REF02_Other Payer Claim Adjustment Indicator";

    DataElement_Name_Pairs["REF01_2330B_F8"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330B_F8"] = "REF02_Other Payer's Claim Control Number";

    DataElement_Name_Pairs["REF01_2330C_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330C_G2"] = "REF02_Other Payer Referring Provider Identifier";

    DataElement_Name_Pairs["REF01_2330C_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330C_0B_I"] = "REF02_Other Payer Attending Provider Identifier";

    DataElement_Name_Pairs["REF01_2330C_1G_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330C_1G_I"] = "REF02_Other Payer Attending Provider Identifier";

    DataElement_Name_Pairs["REF01_2330C_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330C_G2_I"] = "REF02_Other Payer Attending Provider Identifier";

    DataElement_Name_Pairs["REF01_2330C_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330C_LU_I"] = "REF02_Other Payer Attending Provider Identifier";

    DataElement_Name_Pairs["REF01_2330D_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330D_G2"] = "REF02_Other Payer Rendering Provider Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330D_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330D_0B_I"] = "REF02_Other Payer Operating Provider Identifier";

    DataElement_Name_Pairs["REF01_2330D_1G_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330D_1G_I"] = "REF02_Other Payer Operating Provider Identifier";

    DataElement_Name_Pairs["REF01_2330D_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330D_G2_I"] = "REF02_Other Payer Operating Provider Identifier";

    DataElement_Name_Pairs["REF01_2330D_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330D_LU_I"] = "REF02_Other Payer Operating Provider Identifier";

    DataElement_Name_Pairs["REF01_2330E_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330E_G2"] = "REF02_Other Payer Service Facility Location Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330E_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330E_0B_I"] = "REF02_Other Payer Other Operating Provider";

    DataElement_Name_Pairs["REF01_2330E_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330E_LU_I"] = "REF02_Other Payer Other Operating Provider";

    DataElement_Name_Pairs["REF01_2330E_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330E_G2_I"] = "REF02_Other Payer Other Operating Provider";

    DataElement_Name_Pairs["REF01_2330F_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330F_G2"] = "REF02_Other Payer Supervising Provider Identifier";

    DataElement_Name_Pairs["REF01_2330F_0B_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330F_0B_I"] = "REF02_Other Payer Service Facility Location Identifier";

    DataElement_Name_Pairs["REF01_2330F_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330F_LU_I"] = "REF02_Other Payer Service Facility Location Identifier";

    DataElement_Name_Pairs["REF01_2330F_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330F_G2_I"] = "REF02_Other Payer Service Facility Location Identifier";

    DataElement_Name_Pairs["REF01_2330G_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330G_G2"] = "REF02_Other Payer Supervising Provider Identifier";

    DataElement_Name_Pairs["REF01_2330H_1B"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330H_1B"] = "REF02_ Other Payer Service Facility Location Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330H_1C"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330H_1C"] = "REF02_ Other Payer Service Facility Location Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330H_1D"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330H_1D"] = "REF02_ Other Payer Service Facility Location Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330H_EI"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330H_EI"] = "REF02_ Other Payer Service Facility Location Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330H_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330H_G2"] = "REF02_ Other Payer Service Facility Location Secondary Identifier";

    DataElement_Name_Pairs["REF01_2330G_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330G_G2_I"] = "REF02_Other Payer Rendering Provider Identifier";

    DataElement_Name_Pairs["REF01_2330I_G2_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330I_G2_I"] = "REF02_Other Payer Billing Provider Secondary Identification";

    DataElement_Name_Pairs["REF01_2330I_LU_I"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2330I_LU_I"] = "REF02_Other Payer Billing Provider Secondary Identification";

    DataElement_Name_Pairs["REF01_2400_9B"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_9B"] = "REF02_Repriced Line Item Reference Number";

    DataElement_Name_Pairs["REF01_2400_9D"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_9D"] = "REF02_Adjusted Repriced Line Item Reference Number";

    DataElement_Name_Pairs["REF01_2400_G1"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_G1"] = "REF02_Prior Authorization or Referral Number";
    DataElement_Name_Pairs["REF04_2400_G1"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2400_6R"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_6R"] = "REF02_Line Item Control Number";

    DataElement_Name_Pairs["REF01_2400_EW"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_EW"] = "REF02_Mammography Certification Number";

    DataElement_Name_Pairs["REF01_2400_X4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_X4"] = "REF02_Clinical Laboratory Improvement Amendment Number";

    DataElement_Name_Pairs["REF01_2400_F4"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_F4"] = "REF02_Referring CLIA Number";

    DataElement_Name_Pairs["REF01_2400_BT"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_BT"] = "REF02_Immunization Batch Number";

    DataElement_Name_Pairs["REF01_2400_9F"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2400_9F"] = "REF02_Referral Number";
    DataElement_Name_Pairs["REF04_2400_9F"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2420A_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2420A_G2"] = "REF02_Rendering Provider Secondary Identifier";
    DataElement_Name_Pairs["REF04_2420A_G2"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2420B_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2420B_G2"] = "REF02_Purchased Service Provider Secondary Identifier";
    DataElement_Name_Pairs["REF04_2420B_G2"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2420C_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2420C_G2"] = "REF02_Service Facility Location Secondary Identifier";
    DataElement_Name_Pairs["REF04_2420C_G2"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2420D_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2420D_G2"] = "REF02_Supervising Provider Secondary Identifier";
    DataElement_Name_Pairs["REF04_2420D_G2"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2420E_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2420E_G2"] = "REF02_Ordering Provider Secondary Identifier";
    DataElement_Name_Pairs["REF04_2420E_G2"] = "REF04_Reference Identifier";

    DataElement_Name_Pairs["REF01_2420F_G2"] = "REF01_Reference Identification Qualifier";
    DataElement_Name_Pairs["REF02_2420F_G2"] = "REF02_Referring Provider Secondary Identifier";
    DataElement_Name_Pairs["REF04_2420F_G2"] = "REF04_Reference Identifier";


    DataElement_Name_Pairs["SBR01"] = "SBR01_Payer Responsibility Sequence Number code"; 
    DataElement_Name_Pairs["SBR02"] = "SBR02_Individual Relationship Code";
    DataElement_Name_Pairs["SBR03"] = "SBR03_Subscriber Ref ID";
    DataElement_Name_Pairs["SBR04"] = "SBR04_Name";
    DataElement_Name_Pairs["SBR05"] = "SBR05_Insurance Type Code";
    DataElement_Name_Pairs["SBR06"] = "SBR06_Coordination of Benefits Code";
    DataElement_Name_Pairs["SBR07"] = "SBR07_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["SBR08"] = "SBR08_Employment Status Code";
    DataElement_Name_Pairs["SBR09"] = "SBR09_Claim Filling Indicator Code";

    DataElement_Name_Pairs["SBR01_2000B"] = "SBR01_Payer Responsibility Sequence Number code"; 
    DataElement_Name_Pairs["SBR02_2000B"] = "SBR02_Individual Relationship Code";
    DataElement_Name_Pairs["SBR03_2000B"] = "SBR03_Reference Identification";
    DataElement_Name_Pairs["SBR04_2000B"] = "SBR04_Name";
    DataElement_Name_Pairs["SBR05_2000B"] = "SBR05_Insurance Type Code";
    DataElement_Name_Pairs["SBR09_2000B"] = "SBR09_Claim Filling Indicator Code";

    DataElement_Name_Pairs["SBR01_2320"] = "SBR01_Payer Responsibility Sequence Number code"; 
    DataElement_Name_Pairs["SBR02_2320"] = "SBR02_Individual Relationship Code";
    DataElement_Name_Pairs["SBR03_2320"] = "SBR03_Insured Group or Policy Number";
    DataElement_Name_Pairs["SBR04_2320"] = "SBR04_Other Insured Group Name";
    DataElement_Name_Pairs["SBR05_2320"] = "SBR05_Insurance Type Code";
    DataElement_Name_Pairs["SBR09_2320"] = "SBR09_Claim Filling Indicator Code";

    DataElement_Name_Pairs["NTE01"] = "NTE01_Note Reference Code";
    DataElement_Name_Pairs["NTE02"] = "NTE02_Description";

    DataElement_Name_Pairs["NTE01_2300"] = "NTE01_Note Reference Code";
    DataElement_Name_Pairs["NTE02_2300"] = "NTE02_Claim Note Text";

    DataElement_Name_Pairs["NTE01_2400"] = "NTE01_Note Reference Code";
    DataElement_Name_Pairs["NTE02_2400"] = "NTE02_Line Note Text";



    DataElement_Name_Pairs["HCP01"] = "HCP01_Pricing Methodology";
    DataElement_Name_Pairs["HCP02"] = "HCP02_Monetary Amount";
    DataElement_Name_Pairs["HCP03"] = "HCP03_Monetary Amount";
    DataElement_Name_Pairs["HCP04"] = "HCP04_Reference Identification";
    DataElement_Name_Pairs["HCP05"] = "HCP05_Rate";
    DataElement_Name_Pairs["HCP06"] = "HCP06_Reference Identification";
    DataElement_Name_Pairs["HCP07"] = "HCP07_Monetary Amount";
    DataElement_Name_Pairs["HCP08"] = "HCP08_Product/Service ID";
    DataElement_Name_Pairs["HCP09"] = "HCP09_Product/Service ID Qualifier";
    DataElement_Name_Pairs["HCP10"] = "HCP10_Product/Service ID";
    DataElement_Name_Pairs["HCP11"] = "HCP11_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["HCP12"] = "HCP12_Quantity";
    DataElement_Name_Pairs["HCP13"] = "HCP13_Reject Reason Code";
    DataElement_Name_Pairs["HCP14"] = "HCP14_Policy Compliance Code";
    DataElement_Name_Pairs["HCP15"] = "HCP15_Exception Code";

    DataElement_Name_Pairs["HCP01_2300"] = "HCP01_Pricing Methodology";
    DataElement_Name_Pairs["HCP02_2300"] = "HCP02_Repriced Allowed Amount";
    DataElement_Name_Pairs["HCP03_2300"] = "HCP03_Repriced Saving Amount";
    DataElement_Name_Pairs["HCP04_2300"] = "HCP04_Repricing Organization Identifier";
    DataElement_Name_Pairs["HCP05_2300"] = "HCP05_Repricing Per Diem or Flat Rate Amount";
    DataElement_Name_Pairs["HCP06_2300"] = "HCP06_Repriced Approved Ambulatory Patient Group (APG) Code";
    DataElement_Name_Pairs["HCP07_2300"] = "HCP07_Repriced Approved Ambulatory Patient Group (APG) Amount";
    DataElement_Name_Pairs["HCP13_2300"] = "HCP13_Reject Reason Code";
    DataElement_Name_Pairs["HCP14_2300"] = "HCP14_Policy Complicance Code";
    DataElement_Name_Pairs["HCP15_2300"] = "HCP15_Exception Code";

    DataElement_Name_Pairs["HCP01_2400"] = "HCP01_Pricing Methodology";
    DataElement_Name_Pairs["HCP02_2400"] = "HCP02_Repriced Allowed Amount";
    DataElement_Name_Pairs["HCP03_2400"] = "HCP03_Repriced Saving Amount";
    DataElement_Name_Pairs["HCP04_2400"] = "HCP04_Repricing Organization Identifier";
    DataElement_Name_Pairs["HCP05_2400"] = "HCP05_Repricing Per Diem or Flat Rate Amount";
    DataElement_Name_Pairs["HCP06_2400"] = "HCP06_Repriced Approved Ambulatory Patient Group (APG) Code";
    DataElement_Name_Pairs["HCP07_2400"] = "HCP07_Repriced Approved Ambulatory Patient Group (APG) Amount";
    DataElement_Name_Pairs["HCP09_2400"] = "HCP09_Product or Service ID Qualifier";
    DataElement_Name_Pairs["HCP10_2400"] = "HCP10_Repriced Approved HCPCS Code";
    DataElement_Name_Pairs["HCP11_2400"] = "HCP11_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["HCP12_2400"] = "HCP12_Repriced Approved Service Unit Count";
    DataElement_Name_Pairs["HCP13_2400"] = "HCP13_Reject Reason Code";
    DataElement_Name_Pairs["HCP14_2400"] = "HCP14_Policy Compliance Code";
    DataElement_Name_Pairs["HCP15_2400"] = "HCP15_Exception Code";

    DataElement_Name_Pairs["QTY01_2400_FL"] = "QTY01_Quantity Qualifier";
    DataElement_Name_Pairs["QTY02_2400_FL"] = "QTY02_Obstetric Additional Units";

    DataElement_Name_Pairs["QTY01_2400_PT"] = "QTY01_Quantity Qualifier";
    DataElement_Name_Pairs["QTY02_2400_PT"] = "QTY02_Ambulance Patient Count";

    DataElement_Name_Pairs["SBR04"] = "SBR04_Name";
    DataElement_Name_Pairs["SBR05"] = "SBR05_Insurance Type Code";
    DataElement_Name_Pairs["SBR09"] = "SBR09_Claim Filing Indicator";

    DataElement_Name_Pairs["AMT01_2300_F5"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2300_F5"] = "AMT02_Patient Amount Paid";

    DataElement_Name_Pairs["AMT01_2320_D"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2320_D"] = "AMT02_Payer Paid Amount";

    DataElement_Name_Pairs["AMT01_2320_A8"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2320_A8"] = "AMT02_Non-Covered Charge Amount";

    DataElement_Name_Pairs["AMT01_2320_EAF"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2320_EAF"] = "AMT02_Remaining Patient Liability";

    DataElement_Name_Pairs["AMT01"] = "AMT01_Amount Qualifier ";
    DataElement_Name_Pairs["AMT02"] = "AMT02_Monetary Amount";

    DataElement_Name_Pairs["AMT01_2400_T"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2400_T"] = "AMT02_Sales Tax Amount";

    DataElement_Name_Pairs["AMT01_2400_F4"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2400_F4"] = "AMT02_Postage Claimed Amount";

    DataElement_Name_Pairs["AMT01_2400_GT_I"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2400_GT_I"] = "AMT02_Service Tax Amount";

    DataElement_Name_Pairs["AMT01_2400_N8_I"] = "AMT01_Amount Qualifier Code";
    DataElement_Name_Pairs["AMT02_2400_N8_I"] = "AMT02_Facility Tax Amount";

    DataElement_Name_Pairs["OI01"] = "OI01_Claim Filling Indicator Code";
    DataElement_Name_Pairs["OI02"] = "OI02_Claim Submission Reason Code";
    DataElement_Name_Pairs["OI03"] = "OI03_Benefits Assignment";
    DataElement_Name_Pairs["OI04"] = "OI04_Patient Signature Source";
    DataElement_Name_Pairs["OI05"] = "OI05_Provider Agreement Code";
    DataElement_Name_Pairs["OI06"] = "OI06_Release of Information Code";

    DataElement_Name_Pairs["MIA01_2320"] = "MIA01_Covered Days or Visits Count";
    DataElement_Name_Pairs["MIA03_2320"] = "MIA03_Lifetime Psychiatric Days ";
    DataElement_Name_Pairs["MIA04_2320"] = "MIA04_Remaining Patient Liability Amount";
    DataElement_Name_Pairs["MIA05_2320"] = "MIA05_Claim Payment Remark Code";
    DataElement_Name_Pairs["MIA06_2320"] = "MIA06_Claim Disproportionate Share Amoun";
    DataElement_Name_Pairs["MIA07_2320"] = "MIA07_Claim MSP Pass-through Amount";
    DataElement_Name_Pairs["MIA08_2320"] = "MIA08_Claim PPS Capital Amount";
    DataElement_Name_Pairs["MIA09_2320"] = "MIA09_PPS-Capital FSP DRG Amount";
    DataElement_Name_Pairs["MIA10_2320"] = "MIA10_PPS-Capital HSP DRG Amount";
    DataElement_Name_Pairs["MIA11_2320"] = "MIA11_PPS-Capital DSH DRG Amount";
    DataElement_Name_Pairs["MIA12_2320"] = "MIA12_Old Capital Amount";
    DataElement_Name_Pairs["MIA13_2320"] = "MIA13_PPS-Capital IME Amount";
    DataElement_Name_Pairs["MIA14_2320"] = "MIA14_PPS-Operating Hospital Specific DRG Amount";
    DataElement_Name_Pairs["MIA15_2320"] = "MIA15_Cost Report Day Count ";
    DataElement_Name_Pairs["MIA16_2320"] = "MIA16_PPS-Operating Federal Specific DRG Amount";
    DataElement_Name_Pairs["MIA17_2320"] = "MIA17_Claim PPS Capital Outlier Amount";
    DataElement_Name_Pairs["MIA18_2320"] = "MIA18_Claim Indirect Teaching Amount";
    DataElement_Name_Pairs["MIA19_2320"] = "MIA19_Non-Payable Professional Component Billed Amount";
    DataElement_Name_Pairs["MIA20_2320"] = "MIA20_Claim Payment Remark Code";
    DataElement_Name_Pairs["MIA21_2320"] = "MIA21_Claim Payment Remark Code";
    DataElement_Name_Pairs["MIA22_2320"] = "MIA22_Claim Payment Remark Code";
    DataElement_Name_Pairs["MIA23_2320"] = "MIA23_Claim Payment Remark Code";
    DataElement_Name_Pairs["MIA24_2320"] = "MIA24_PPS-Capital Exception Amoun";



    DataElement_Name_Pairs["OI03_2320"] = "OI03_Benefits Assignment Certification Indicator";
    DataElement_Name_Pairs["OI04_2320"] = "OI04_Patient Signature Source Code";
    DataElement_Name_Pairs["OI06_2320"] = "OI06_Release of Information Code";

    DataElement_Name_Pairs["MOA01"] = "MOA01_Percentage as Decimal";
    DataElement_Name_Pairs["MOA02"] = "MOA02_Monetary Amount";
    DataElement_Name_Pairs["MOA03"] = "MOA03_Reference Identification";
    DataElement_Name_Pairs["MOA04"] = "MOA04_Reference Identification";
    DataElement_Name_Pairs["MOA05"] = "MOA05_Reference Identification";
    DataElement_Name_Pairs["MOA06"] = "MOA06_Reference Identification";
    DataElement_Name_Pairs["MOA07"] = "MOA07_Reference Identification";
    DataElement_Name_Pairs["MOA08"] = "MOA08_Monetary Amount";
    DataElement_Name_Pairs["MOA09"] = "MOA09_Monetary Amount";

    DataElement_Name_Pairs["MOA01_2320"] = "MOA01_Reimbursement Rate";
    DataElement_Name_Pairs["MOA02_2320"] = "MOA02_HCPCS Payable Amount";
    DataElement_Name_Pairs["MOA03_2320"] = "MOA03_Claim Payment Remark Code";
    DataElement_Name_Pairs["MOA04_2320"] = "MOA04_Claim Payment Remark Code";
    DataElement_Name_Pairs["MOA05_2320"] = "MOA05_Claim Payment Remark Code";
    DataElement_Name_Pairs["MOA06_2320"] = "MOA06_Claim Payment Remark Code";
    DataElement_Name_Pairs["MOA07_2320"] = "MOA07_Claim Payment Remark Code";
    DataElement_Name_Pairs["MOA08_2320"] = "MOA08_End Stage Renal Disease Payment Amount";
    DataElement_Name_Pairs["MOA09_2320"] = "MOA09_Non-Payable Professional Component Billed Amount";


    DataElement_Name_Pairs["DTP01"] = "DTP01_Date/Time Qualifier";
    DataElement_Name_Pairs["DTP02"] = "DTP02_Format Qualifier";
    DataElement_Name_Pairs["DTP03"] = "DTP03_Date Time Period";

    DataElement_Name_Pairs["DTP01_2300_431"] = "DTP01_Date/Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_431"] = "DTP02_Onset of Current Symptoms or Illness";
    DataElement_Name_Pairs["DTP03_2300_431"] = "DTP03_Onset of Current Symptoms or Injury Date";

    DataElement_Name_Pairs["DTP01_2300_454"] = "DTP01_Date/Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_454"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_454"] = "DTP03_Initial Treatment Date";

    DataElement_Name_Pairs["DTP01_2300_304"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_304"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_304"] = "DTP03_Last Seen Date";

    DataElement_Name_Pairs["DTP01_2300_453"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_453"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_453"] = "DTP03_Acute Manifestation Date";

    DataElement_Name_Pairs["DTP01_2300_439"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_439"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_439"] = "DTP03_Accident Date";

    DataElement_Name_Pairs["DTP01_2300_484"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_484"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_484"] = "DTP03_Last Menstrual Period";

    DataElement_Name_Pairs["DTP01_2300_455"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_455"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_455"] = "DTP03_Last X-Ray Date";

    DataElement_Name_Pairs["DTP01_2300_471"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_471"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_471"] = "DTP03_Prescription Date";

    DataElement_Name_Pairs["DTP01_2300_360"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_360"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_360"] = "DTP03_Disability From Date";

    DataElement_Name_Pairs["DTP01_2300_297"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_297"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_297"] = "DTP03_Last Worked Date";

    DataElement_Name_Pairs["DTP01_2300_296"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_296"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_296"] = "DTP03_Work Return Date";

    DataElement_Name_Pairs["DTP01_2300_435"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_435"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_435"] = "DTP03_Related Hospitalization Admission Date";

    DataElement_Name_Pairs["DTP01_2300_096"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_096"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_096"] = "DTP03_Related Hospitalization Discharge Date";



    DataElement_Name_Pairs["DTP01_2300_090"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_090"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_090"] = "DTP03_Assumed or Relinquished Care Date";

    DataElement_Name_Pairs["DTP01_2300_444"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_444"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_444"] = "DTP03_Date Time Period";

    DataElement_Name_Pairs["DTP01_2300_050"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_050"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_050"] = "DTP03_Repricer Received Date";

    DataElement_Name_Pairs["DTP01_2330B_573"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2330B_573"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2330B_573"] = "DTP03_Adjudication or Payment Date";

    DataElement_Name_Pairs["DTP01_2400_472"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_472"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_472"] = "DTP03_Service Date";

    DataElement_Name_Pairs["DTP01_2400_471"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_471"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_471"] = "DTP03_Prescription Date";

    DataElement_Name_Pairs["DTP01_2400_607"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_607"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_607"] = "DTP03_Certification Revision or Recertification Date";

    DataElement_Name_Pairs["DTP01_2400_463"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_463"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_463"] = "DTP03_Begin Therapy Date";

    DataElement_Name_Pairs["DTP01_2400_461"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_461"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_461"] = "DTP03_Last Certification Date";

    DataElement_Name_Pairs["DTP01_2400_304"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_304"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_304"] = "DTP03_Treatment or Therapy Date";

    DataElement_Name_Pairs["DTP01_2400_738"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_738"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_738"] = "DTP03_Test Performed Date";

    DataElement_Name_Pairs["DTP01_2400_011"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_011"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_011"] = "DTP03_Shipped Date";

    DataElement_Name_Pairs["DTP01_2400_001"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_001"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_001"] = "DTP03_Service Date";

    DataElement_Name_Pairs["DTP01_2400_455"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_455"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_455"] = "DTP03_Last X-Ray Date";

    DataElement_Name_Pairs["DTP01_2300_96"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_96"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_96"] = "DTP03_Last X-Ray Date";
    

    DataElement_Name_Pairs["DTP01_2400_454"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2400_454"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2400_454"] = "DTP03_Initial Treatment Date";

    DataElement_Name_Pairs["DTP01_2300_096_I"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_096_I"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_096_I"] = "DTP03_Discharge Time";

    DataElement_Name_Pairs["DTP01_2300_434_I"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_434_I"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_434_I"] = "DTP03_Statement From and To Date";

    DataElement_Name_Pairs["DTP01_2300_435_I"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_435_I"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_435_I"] = "DTP03_Admission Date and Hour";

    DataElement_Name_Pairs["DTP01_2300_050_I"] = "DTP01_Date Time Qualifier";
    DataElement_Name_Pairs["DTP02_2300_050_I"] = "DTP02_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DTP03_2300_050_I"] = "DTP03_Order Date";

    DataElement_Name_Pairs["PWK01_2300"] = "PWK01_Attatchment Report Type Code";
    DataElement_Name_Pairs["PWK02_2300"] = "PWK02_Attatchment Transmission Code";
    DataElement_Name_Pairs["PWK05_2300"] = "PWK05_Identification Code Qualifier";
    DataElement_Name_Pairs["PWK06_2300"] = "PWK06_Attatchment Control Number";

    DataElement_Name_Pairs["PWK01_2400"] = "PWK01_Attatchment Report Type Code";
    DataElement_Name_Pairs["PWK02_2400"] = "PWK02_Attatchment Transmission Code";
    DataElement_Name_Pairs["PWK03_2400"] = "PWK03_Report Copies Needed";
    DataElement_Name_Pairs["PWK05_2400"] = "PWK05_Identification Code Qualifier";
    DataElement_Name_Pairs["PWK06_2400"] = "PWK06_Attatchment Control Number";


    DataElement_Name_Pairs["LX01"] = "LX01_Assigned Number";

    DataElement_Name_Pairs["LX01_2400"] = "LX01_Assigned Number";

    DataElement_Name_Pairs["MEA01_2400"] = "MEA01_Measurement Reference Identification Code";
    DataElement_Name_Pairs["MEA02_2400"] = "MEA02_Measurement Qualifier";
    DataElement_Name_Pairs["MEA03_2400"] = "MEA03_Test Results";

    


    DataElement_Name_Pairs["SV101"] = "SV101_Composite Medical Procedure Identifier";
    DataElement_Name_Pairs["SV102"] = "SV101-1_Product/Service ID Qualifier";
    DataElement_Name_Pairs["SV103"] = "SV101-2_Procedure Code";
    DataElement_Name_Pairs["SV104"] = "SV101-3_Procedure Modifier";
    DataElement_Name_Pairs["SV105"] = "SV101-4_Procedure Modifier";
    DataElement_Name_Pairs["SV106"] = "SV101-5_Procedure Modifier";
    DataElement_Name_Pairs["SV107"] = "SV101-6_Procedure Modifier";
    DataElement_Name_Pairs["SV108"] = "SV101-7_Description";
    DataElement_Name_Pairs["SV109"] = "SV101-8_Product/Service ID";
    DataElement_Name_Pairs["SV102"] = "SV102_Monetary Amount";
    DataElement_Name_Pairs["SV103"] = "SV103_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["SV104"] = "SV104_Quantity";
    DataElement_Name_Pairs["SV105"] = "SV105_Facility Code Value";
    DataElement_Name_Pairs["SV106"] = "SV106_Place Of Service Code";
    DataElement_Name_Pairs["SV107"] = "SV107_Diagnosis Code Pointer";

    DataElement_Name_Pairs["SV101_2400"] = "SV101_Composite Medical Procedure Identifier";
    DataElement_Name_Pairs["SV102_2400"] = "SV102_Line Item Charge Amount";
    DataElement_Name_Pairs["SV103_2400"] = "SV103_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["SV104_2400"] = "SV104_Service Unit Count";
    DataElement_Name_Pairs["SV105_2400"] = "SV105_Place of Service Code";
    DataElement_Name_Pairs["SV107_2400"] = "SV107_Composite Diagnosis Code Pointer";
    DataElement_Name_Pairs["SV109_2400"] = "SV109_Emergency Indicator";
    DataElement_Name_Pairs["SV111_2400"] = "SV111_EPSDT Indicator";
    DataElement_Name_Pairs["SV112_2400"] = "SV112_Family Planning Indicator";
    DataElement_Name_Pairs["SV115_2400"] = "SV115_Co-Pay Status Code";

    DataElement_Name_Pairs["PS101_2400"] = "PS101_Purchased Service Provider Identifier";
    DataElement_Name_Pairs["PS102_2400"] = "PS102_Purchased Service Charge Amount";



    DataElement_Name_Pairs["SVD01"] = "SVD01_Identification Code";
    DataElement_Name_Pairs["SVD02"] = "SVD02_Monetary Amount";
    DataElement_Name_Pairs["SVD03"] = "SVD03-1_Product/Service ID Qualifier";
    DataElement_Name_Pairs["SVD04"] = "SVD03-2_Product/Service ID";
    DataElement_Name_Pairs["SVD05"] = "SVD03-3_Procedure Modifier";
    DataElement_Name_Pairs["SVD06"] = "SVD03-4_Procedure Modifier";
    DataElement_Name_Pairs["SVD07"] = "SVD03-5_Procedure Modifier";
    DataElement_Name_Pairs["SVD08"] = "SVD03-6_Procedure Modifier";
    
    DataElement_Name_Pairs["SV501_2400"] = "SV501_Composite Medical Procedure Identifier";
    DataElement_Name_Pairs["SV502_2400"] = "SV502_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["SV503_2400"] = "SV503_Length of Medical Necessity";
    DataElement_Name_Pairs["SV504_2400"] = "SV504_DME Rental Price";
    DataElement_Name_Pairs["SV505_2400"] = "SV505_DME Purchase Price";
    DataElement_Name_Pairs["SV506_2400"] = "SV506_Rental Unit Price Indicator";

    DataElement_Name_Pairs["CL101"] = "CL101_Admission Type Code";
    DataElement_Name_Pairs["CL102"] = "CL102_Admission Source Code";
    DataElement_Name_Pairs["CL103"] = "CL103_Patient Status Code";
    DataElement_Name_Pairs["PWK01"] = "PWK01_Report Type Code";
    DataElement_Name_Pairs["PWK02"] = "PWK02_Report Transmission Code";
    DataElement_Name_Pairs["PWK03"] = "PWK03_Report Copies Needed";
    DataElement_Name_Pairs["PWK04"] = "PWK04_Entity Identifier Code";
    DataElement_Name_Pairs["PWK05"] = "PWK05_Identification Code Qualifier";
    DataElement_Name_Pairs["PWK06"] = "PWK06_Identification Code";
    DataElement_Name_Pairs["PWK07"] = "PWK07_Description";
    DataElement_Name_Pairs["PWK08"] = "PWK08_Actions Indicated";
    DataElement_Name_Pairs["PWK09"] = "PWK09_Request Category Code";

    DataElement_Name_Pairs["CL101_2300"] = "CL101_Admission Type Code";
    DataElement_Name_Pairs["CL102_2300"] = "CL102_Admission Source Code";
    DataElement_Name_Pairs["CL103_2300"] = "CL103_Patient Status Code";

    

    DataElement_Name_Pairs["HI01"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02"] = "HI02_Health Care Code Information";
    DataElement_Name_Pairs["HI03"] = "HI03_Health Care Code Information";
    DataElement_Name_Pairs["HI04"] = "HI04_Health Care Code Information";
    DataElement_Name_Pairs["HI05"] = "HI05_Health Care Code Information";
    DataElement_Name_Pairs["HI06"] = "HI06_Health Care Code Information";
    DataElement_Name_Pairs["HI07"] = "HI07_Health Care Code Information";
    DataElement_Name_Pairs["HI08"] = "HI08_Health Care Code Information";
    DataElement_Name_Pairs["HI09"] = "HI09_Health Care Code Information";
    DataElement_Name_Pairs["HI10"] = "HI10_Health Care Code Information";
    DataElement_Name_Pairs["HI11"] = "HI11_Health Care Code Information";
    DataElement_Name_Pairs["HI12"] = "HI12_Health Care Code Information";

    DataElement_Name_Pairs["HI01_2300_BG"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02_2300_BG"] = "HI02_Health Care Code Information";
    DataElement_Name_Pairs["HI03_2300_BG"] = "HI03_Health Care Code Information";
    DataElement_Name_Pairs["HI04_2300_BG"] = "HI04_Health Care Code Information";
    DataElement_Name_Pairs["HI05_2300_BG"] = "HI05_Health Care Code Information";
    DataElement_Name_Pairs["HI06_2300_BG"] = "HI06_Health Care Code Information";
    DataElement_Name_Pairs["HI07_2300_BG"] = "HI07_Health Care Code Information";
    DataElement_Name_Pairs["HI08_2300_BG"] = "HI08_Health Care Code Information";
    DataElement_Name_Pairs["HI09_2300_BG"] = "HI09_Health Care Code Information";
    DataElement_Name_Pairs["HI10_2300_BG"] = "HI10_Health Care Code Information";
    DataElement_Name_Pairs["HI11_2300_BG"] = "HI11_Health Care Code Information";
    DataElement_Name_Pairs["HI12_2300_BG"] = "HI12_Health Care Code Information";

    DataElement_Name_Pairs["HI01_2300_BE"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02_2300_BE"] = "HI02_Health Care Code Information";
    DataElement_Name_Pairs["HI03_2300_BE"] = "HI03_Health Care Code Information";
    DataElement_Name_Pairs["HI04_2300_BE"] = "HI04_Health Care Code Information";
    DataElement_Name_Pairs["HI05_2300_BE"] = "HI05_Health Care Code Information";
    DataElement_Name_Pairs["HI06_2300_BE"] = "HI06_Health Care Code Information";
    DataElement_Name_Pairs["HI07_2300_BE"] = "HI07_Health Care Code Information";
    DataElement_Name_Pairs["HI08_2300_BE"] = "HI08_Health Care Code Information";
    DataElement_Name_Pairs["HI09_2300_BE"] = "HI09_Health Care Code Information";
    DataElement_Name_Pairs["HI10_2300_BE"] = "HI10_Health Care Code Information";
    DataElement_Name_Pairs["HI11_2300_BE"] = "HI11_Health Care Code Information";
    DataElement_Name_Pairs["HI12_2300_BE"] = "HI12_Health Care Code Information";

    DataElement_Name_Pairs["HI01_2300_BK"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02_2300_BK"] = "HI02_Health Care Code Information";
    DataElement_Name_Pairs["HI03_2300_BK"] = "HI03_Health Care Code Information";
    DataElement_Name_Pairs["HI04_2300_BK"] = "HI04_Health Care Code Information";
    DataElement_Name_Pairs["HI05_2300_BK"] = "HI05_Health Care Code Information";
    DataElement_Name_Pairs["HI06_2300_BK"] = "HI06_Health Care Code Information";
    DataElement_Name_Pairs["HI07_2300_BK"] = "HI07_Health Care Code Information";
    DataElement_Name_Pairs["HI08_2300_BK"] = "HI08_Health Care Code Information";
    DataElement_Name_Pairs["HI09_2300_BK"] = "HI09_Health Care Code Information";
    DataElement_Name_Pairs["HI10_2300_BK"] = "HI10_Health Care Code Information";
    DataElement_Name_Pairs["HI11_2300_BK"] = "HI11_Health Care Code Information";
    DataElement_Name_Pairs["HI12_2300_BK"] = "HI12_Health Care Code Information";

    DataElement_Name_Pairs["HI01_2300_BP"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02_2300_BP"] = "HI02_Health Care Code Information";

    DataElement_Name_Pairs["HI01_2300_BF"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02_2300_BF"] = "HI02_Health Care Code Information";
    DataElement_Name_Pairs["HI03_2300_BF"] = "HI03_Health Care Code Information";
    DataElement_Name_Pairs["HI04_2300_BF"] = "HI04_Health Care Code Information";
    DataElement_Name_Pairs["HI05_2300_BF"] = "HI05_Health Care Code Information";
    DataElement_Name_Pairs["HI06_2300_BF"] = "HI06_Health Care Code Information";
    DataElement_Name_Pairs["HI07_2300_BF"] = "HI07_Health Care Code Information";
    DataElement_Name_Pairs["HI08_2300_BF"] = "HI08_Health Care Code Information";
    DataElement_Name_Pairs["HI09_2300_BF"] = "HI09_Health Care Code Information";
    DataElement_Name_Pairs["HI10_2300_BF"] = "HI10_Health Care Code Information";
    DataElement_Name_Pairs["HI11_2300_BF"] = "HI11_Health Care Code Information";
    DataElement_Name_Pairs["HI12_2300_BF"] = "HI12_Health Care Code Information";

    DataElement_Name_Pairs["HI01_2300_BH"] = "HI01_Health Care Code Information";
    DataElement_Name_Pairs["HI02_2300_BH"] = "HI02_Health Care Code Information";
    DataElement_Name_Pairs["HI03_2300_BH"] = "HI03_Health Care Code Information";
    DataElement_Name_Pairs["HI04_2300_BH"] = "HI04_Health Care Code Information";
    DataElement_Name_Pairs["HI05_2300_BH"] = "HI05_Health Care Code Information";
    DataElement_Name_Pairs["HI06_2300_BH"] = "HI06_Health Care Code Information";
    DataElement_Name_Pairs["HI07_2300_BH"] = "HI07_Health Care Code Information";
    DataElement_Name_Pairs["HI08_2300_BH"] = "HI08_Health Care Code Information";
    DataElement_Name_Pairs["HI09_2300_BH"] = "HI09_Health Care Code Information";
    DataElement_Name_Pairs["HI10_2300_BH"] = "HI10_Health Care Code Information";
    DataElement_Name_Pairs["HI11_2300_BH"] = "HI11_Health Care Code Information";
    DataElement_Name_Pairs["HI12_2300_BH"] = "HI12_Health Care Code Information";


    DataElement_Name_Pairs["SV201"] = "SV201_Product/Service ID";
    DataElement_Name_Pairs["SV202"] = "SV202-1_Product/Service ID Qualifier";
    DataElement_Name_Pairs["SV203"] = "SV202-2_Product/Service ID";
    DataElement_Name_Pairs["SV204"] = "SV202-3_Product/Service Modifier";
    DataElement_Name_Pairs["SV205"] = "SV202-4_Product/Service Modifier";
    DataElement_Name_Pairs["SV206"] = "SV202-5_Product/Service Modifier";
    DataElement_Name_Pairs["SV207"] = "SV205_Quantity";

    DataElement_Name_Pairs["SV201_2400"] = "SV201_Revenue Code"; 
    DataElement_Name_Pairs["SV202_2400"] = "SV202_COMPOSITE";
    DataElement_Name_Pairs["SV203_2400"] = "SV203_Line Item Charge Amount";
    DataElement_Name_Pairs["SV204_2400"] = "SV204_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["SV205_2400"] = "SV205_Service Unit Count";
    DataElement_Name_Pairs["SV207_2400"] = "SV207_Monetary Amount";


    DataElement_Name_Pairs["PER01"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02"] = "PER02_Submitter Contact Name";
    DataElement_Name_Pairs["PER03"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06"] = "PER06_Communication Number";
    DataElement_Name_Pairs["PER07"] = "PER07_Communication Number Qualifier";
    DataElement_Name_Pairs["PER08"] = "PER08_Communication Number";
    DataElement_Name_Pairs["PER09"] = "PER09_Contant Inquiry Reference";

    DataElement_Name_Pairs["PER01_2010AA"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_2010AA"] = "PER02_Billing Provider Contact Name";
    DataElement_Name_Pairs["PER03_2010AA"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_2010AA"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05_2010AA"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_2010AA"] = "PER06_Communication Number";
    DataElement_Name_Pairs["PER07_2010AA"] = "PER07_Communication Number Qualifier";
    DataElement_Name_Pairs["PER08_2010AA"] = "PER08_Communication Number";

    DataElement_Name_Pairs["PER01_2010CA"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_2010CA"] = "PER02_Name";
    DataElement_Name_Pairs["PER03_2010CA"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_2010CA"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05_2010CA"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_2010CA"] = "PER06_Communication Number";

    DataElement_Name_Pairs["PER01_2420E"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_2420E"] = "PER02_Ordering Provider Contact Name";
    DataElement_Name_Pairs["PER03_2420E"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_2420E"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05_2420E"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_2420E"] = "PER06_Communication Number";
    DataElement_Name_Pairs["PER07_2420E"] = "PER07_Communication Number Qualifier";
    DataElement_Name_Pairs["PER08_2420E"] = "PER08_Communication Number";


    DataElement_Name_Pairs["PER01_2010BA"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_2010BA"] = "PER02_Name";
    DataElement_Name_Pairs["PER03_2010BA"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_2010BA"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05_2010BA"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_2010BA"] = "PER06_Communication Number";

    DataElement_Name_Pairs["PER01_2010CA"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_2010CA"] = "PER02_Name";
    DataElement_Name_Pairs["PER03_2010CA"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_2010CA"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05_2010CA"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_2010CA"] = "PER06_Communication Number";

    DataElement_Name_Pairs["PER01_2310C"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_2310C"] = "PER02_Name";
    DataElement_Name_Pairs["PER03_2310C"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_2310C"] = "PER04_Communication Number";  
    DataElement_Name_Pairs["PER05_2310C"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_2310C"] = "PER06_Communication Number";

    DataElement_Name_Pairs["SE01"] = "SE01_Transaction Set Control Number";
    DataElement_Name_Pairs["SE02"] = "SE02_Transaction Set Control Number";

    DataElement_Name_Pairs["DMG01"] = "DMG01_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DMG02"] = "DMG02_Date Time Period";
    DataElement_Name_Pairs["DMG03"] = "DMG03_Gender Code";
    DataElement_Name_Pairs["DMG04"] = "DMG04_Marital Status Code";
    DataElement_Name_Pairs["DMG05"] = "DMG05_Composite Race or Ethnicity Information";
    DataElement_Name_Pairs["DMG06"] = "DMG06_Citizenship Status Code";
    DataElement_Name_Pairs["DMG07"] = "DMG07_Country Code";
    DataElement_Name_Pairs["DMG08"] = "DMG08_Basis of Verification Code";
    DataElement_Name_Pairs["DMG09"] = "DMG09_Quantity";
    DataElement_Name_Pairs["DMG10"] = "DMG10_Code List Qualifier Code";
    DataElement_Name_Pairs["DMG11"] = "DMG11_Industry Code";

    DataElement_Name_Pairs["DMG01_2010BA"] = "DMG01_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DMG02_2010BA"] = "DMG02_Subscriber Birth Date";
    DataElement_Name_Pairs["DMG03_2010BA"] = "DMG03_Subscriber Gender Code";

    DataElement_Name_Pairs["DMG01_2010CA"] = "DMG01_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["DMG02_2010CA"] = "DMG02_Patient Birth Date";
    DataElement_Name_Pairs["DMG03_2010CA"] = "DMG03_Patient Gender Code";



    DataElement_Name_Pairs["CUR01"] = "CUR01_Entity Identifier Code";
    DataElement_Name_Pairs["CUR02"] = "CUR02_Currency Code";

    DataElement_Name_Pairs["CUR01_2000A"] = "CUR01_Entity Identifier Code";
    DataElement_Name_Pairs["CUR02_2000A"] = "CUR02_Currency Code";

    DataElement_Name_Pairs["NM101"] = "NM101_Entitiy Identifier Code";
    DataElement_Name_Pairs["NM102"] = "NM102_Entity Type Qualifier";
    DataElement_Name_Pairs["NM103"] = "NM103_Name Last or Organization Name";
    DataElement_Name_Pairs["NM104"] = "NM104_Name First";
    DataElement_Name_Pairs["NM105"] = "NM105_Name Middle";
    DataElement_Name_Pairs["NM106"] = "NM106_Name Prefix";
    DataElement_Name_Pairs["NM107"] = "NM107_Name Suffix";
    DataElement_Name_Pairs["NM108"] = "NM108_Identification Code Qualifier";
    DataElement_Name_Pairs["NM109"] = "NM109_Identification Code";
    DataElement_Name_Pairs["NM110"] = "NM110_Entitiy Relationship Code";
    DataElement_Name_Pairs["NM111"] = "NM111_Entity Identifier Code";
    DataElement_Name_Pairs["NM112"] = "NM112_Name Last or Organization Name";

    DataElement_Name_Pairs["PAT01"] = "Individual Relationship Code";
    DataElement_Name_Pairs["PAT02"] = "PAT02_Patient Location Code";
    DataElement_Name_Pairs["PAT03"] = "PAT03_Employment Status Code";
    DataElement_Name_Pairs["PAT04"] = "PAT04_Student Status Code";
    DataElement_Name_Pairs["PAT05"] = "PAT05_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["PAT06"] = "PAT06_Date Time Period";
    DataElement_Name_Pairs["PAT07"] = "PAT07_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["PAT08"] = "PAT08_Weight";
    DataElement_Name_Pairs["PAT09"] = "PAT09_Yes/No Condition or Response Code";


    DataElement_Name_Pairs["PAT05_2000B"] = "PAT05_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["PAT06_2000B"] = "PAT06_Patient Death Date";
    DataElement_Name_Pairs["PAT07_2000B"] = "PAT07_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["PAT08_2000B"] = "PAT08_Patient Weight";
    DataElement_Name_Pairs["PAT09_2000B"] = "PAT09_Pregnancy Indicator";

    DataElement_Name_Pairs["PAT01_2000C"] = "PAT01_Individual Relationship Code";
    DataElement_Name_Pairs["PAT05_2000C"] = "PAT05_Date Time Period Format Qualifier";
    DataElement_Name_Pairs["PAT06_2000C"] = "PAT06_Patient Death Date";
    DataElement_Name_Pairs["PAT07_2000C"] = "PAT07_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["PAT08_2000C"] = "PAT08_Patient Weight";
    DataElement_Name_Pairs["PAT09_2000C"] = "PAT09_Pregnancy Indicator";

    DataElement_Name_Pairs["CN101"] = "CN101_Contract Type Code";
    DataElement_Name_Pairs["CN102"] = "CN102_Monetary Amount";
    DataElement_Name_Pairs["CN103"] = "CN103_Percent, Decimal Format";
    DataElement_Name_Pairs["CN104"] = "CN104_Reference Identification";
    DataElement_Name_Pairs["CN105"] = "CN105_Terms Discount Percent";
    DataElement_Name_Pairs["CN106"] = "CN106_Version Identifier";

    DataElement_Name_Pairs["CN101_2300"] = "CN101_Contract Type Code";
    DataElement_Name_Pairs["CN102_2300"] = "CN102_Contract Amount";
    DataElement_Name_Pairs["CN103_2300"] = "CN103_Contract Percentage";
    DataElement_Name_Pairs["CN104_2300"] = "CN104_Contract Code";
    DataElement_Name_Pairs["CN105_2300"] = "CN105_Terms Discount Percent";
    DataElement_Name_Pairs["CN106_2300"] = "CN106_Contract Version Identifier";

    DataElement_Name_Pairs["CN101_2400"] = "CN101_Contract Type Code";
    DataElement_Name_Pairs["CN102_2400"] = "CN102_Contract Amount";
    DataElement_Name_Pairs["CN103_2400"] = "CN103_Contract Percentage";
    DataElement_Name_Pairs["CN104_2400"] = "CN104_Contract Code";
    DataElement_Name_Pairs["CN105_2400"] = "CN105_Terms Discount Percent";
    DataElement_Name_Pairs["CN106_2400"] = "CN106_Contract Version Identifier";

    DataElement_Name_Pairs["K301"] = "K301_Fixed Format Information";
    DataElement_Name_Pairs["K302"] = "K302_Record Format Code";
    DataElement_Name_Pairs["K303"] = "K303_Composite Unit of Measure";

    DataElement_Name_Pairs["K301_2300"] = "K301_Fixed Format Information";

    DataElement_Name_Pairs["K301_2400"] = "K301_Fixed Format Information";


    DataElement_Name_Pairs["CR101"] = "CR101_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR102"] = "CR102_Weight";
    DataElement_Name_Pairs["CR103"] = "CR103_Ambulance Transport Code";
    DataElement_Name_Pairs["CR104"] = "CR104_Ambulance Transport Reason Code";
    DataElement_Name_Pairs["CR105"] = "CR105_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR106"] = "CR106_Quantity";
    DataElement_Name_Pairs["CR107"] = "CR107_Address Information";
    DataElement_Name_Pairs["CR108"] = "CR108_Address Information";
    DataElement_Name_Pairs["CR109"] = "CR109_Description";
    DataElement_Name_Pairs["CR110"] = "CR110_Description";

    DataElement_Name_Pairs["CR201"] = "CR201_Count";
    DataElement_Name_Pairs["CR202"] = "CR202_Quantity";
    DataElement_Name_Pairs["CR203"] = "CR203_Subluxation Level Code";
    DataElement_Name_Pairs["CR204"] = "CR204_Subluxation Level Code";
    DataElement_Name_Pairs["CR205"] = "CR205_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR206"] = "CR206_Quantity";
    DataElement_Name_Pairs["CR207"] = "CR207_Quantity";
    DataElement_Name_Pairs["CR208"] = "CR208_Nature of Condition Code";
    DataElement_Name_Pairs["CR209"] = "CR209_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CR210"] = "CR210_Description";
    DataElement_Name_Pairs["CR211"] = "CR211_Description";
    DataElement_Name_Pairs["CR212"] = "CR212_Yes/No Condition or Response Code";

    DataElement_Name_Pairs["CR208_2300"] = "CR208_Patient Condition Code";
    DataElement_Name_Pairs["CR210_2300"] = "CR210_Patient Condition Description";
    DataElement_Name_Pairs["CR211_2300"] = "CR211_Patient Condition Description";

    DataElement_Name_Pairs["CR301_2400"] = "CR301_Certification Type Code";
    DataElement_Name_Pairs["CR302_2400"] = "CR302_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR303_2400"] = "CR303_Durable Medical Equipment Duration";



    DataElement_Name_Pairs["CR101_2300"] = "CR01_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR102_2300"] = "CR02_Patient Weight";
    DataElement_Name_Pairs["CR104_2300"] = "CR04_Ambulance Transport Reason Code";
    DataElement_Name_Pairs["CR105_2300"] = "CR05_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR106_2300"] = "CR06_Transport Distance";
    DataElement_Name_Pairs["CR109_2300"] = "CR07_Round Trip Purpose Description";
    DataElement_Name_Pairs["CR110_2300"] = "CR10_Stretcher Purpose Description";    

    DataElement_Name_Pairs["CR101_2400"] = "CR01_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR102_2400"] = "CR02_Patient Weight";
    DataElement_Name_Pairs["CR104_2400"] = "CR04_Ambulance Transport Reason Code";
    DataElement_Name_Pairs["CR105_2400"] = "CR05_Unit or Basis for Measurement Code";
    DataElement_Name_Pairs["CR106_2400"] = "CR06_Transport Distance";
    DataElement_Name_Pairs["CR109_2400"] = "CR07_Round Trip Purpose Description";
    DataElement_Name_Pairs["CR110_2400"] = "CR10_Stretcher Purpose Description";    

    DataElement_Name_Pairs["CRC01"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02"] = "CRC02_Yes/No Condition or Response Code";
    DataElement_Name_Pairs["CRC03"] = "CRC03_Condition Indicator";
    DataElement_Name_Pairs["CRC04"] = "CRC04_Condition Indicator";
    DataElement_Name_Pairs["CRC05"] = "CRC05_Condition Inidcator";
    DataElement_Name_Pairs["CRC06"] = "CRC06_Condition Inidcator";
    DataElement_Name_Pairs["CRC07"] = "CRC07_Condition Inidcator";

    DataElement_Name_Pairs["CRC01_2300_07"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02_2300_07"] = "CRC02_Certification Condition Indicator";
    DataElement_Name_Pairs["CRC03_2300_07"] = "CRC03_Condition Code";
    DataElement_Name_Pairs["CRC04_2300_07"] = "CRC04_Condition Code";
    DataElement_Name_Pairs["CRC05_2300_07"] = "CRC05_Condition Code";
    DataElement_Name_Pairs["CRC06_2300_07"] = "CRC06_Condition Code";
    DataElement_Name_Pairs["CRC07_2300_07"] = "CRC07_Condition Code";

    DataElement_Name_Pairs["CRC01_2300_E1"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02_2300_E1"] = "CRC02_Certification Condition Indicator";
    DataElement_Name_Pairs["CRC03_2300_E1"] = "CRC03_Condition Code";
    DataElement_Name_Pairs["CRC04_2300_E1"] = "CRC04_Condition Code";
    DataElement_Name_Pairs["CRC05_2300_E1"] = "CRC05_Condition Code";
    DataElement_Name_Pairs["CRC06_2300_E1"] = "CRC06_Condition Code";
    DataElement_Name_Pairs["CRC07_2300_E1"] = "CRC07_Condition Code";

    DataElement_Name_Pairs["CRC01_2300_75"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02_2300_75"] = "CRC02_Certification Condition Indicator";
    DataElement_Name_Pairs["CRC03_2300_75"] = "CRC03_Homebound Indicator";

    DataElement_Name_Pairs["CRC01_2300_ZZ"] = "CRC01_Code Qualifier";
    DataElement_Name_Pairs["CRC02_2300_ZZ"] = "CRC02_Certification Condition Code Applies Indicator";
    DataElement_Name_Pairs["CRC03_2300_ZZ"] = "CRC03_Condition Indicator";
    DataElement_Name_Pairs["CRC04_2300_ZZ"] = "CRC04_Condition Indicator";
    DataElement_Name_Pairs["CRC05_2300_ZZ"] = "CRC05_Condition Indicator";

    DataElement_Name_Pairs["CRC01_2400_07"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02_2400_07"] = "CRC02_Certification Condition Indicator";
    DataElement_Name_Pairs["CRC03_2400_07"] = "CRC03_Condition Indicator";
    DataElement_Name_Pairs["CRC04_2400_07"] = "CRC04_Condition Indicator";
    DataElement_Name_Pairs["CRC05_2400_07"] = "CRC05_Condition Indicator";
    DataElement_Name_Pairs["CRC06_2400_07"] = "CRC06_Condition Indicator";
    DataElement_Name_Pairs["CRC07_2400_07"] = "CRC07_Condition Indicator";

    DataElement_Name_Pairs["CRC01_2400_70"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02_2400_70"] = "CRC02_Hospice Employed Provider Indicator";
    DataElement_Name_Pairs["CRC03_2400_70"] = "CRC03_Condition Indicator";

    DataElement_Name_Pairs["CRC01_2400_09"] = "CRC01_Code Category";
    DataElement_Name_Pairs["CRC02_2400_09"] = "CRC02_Certification Condition Indicator";
    DataElement_Name_Pairs["CRC03_2400_09"] = "CRC03_Condition Indicator";
    DataElement_Name_Pairs["CRC04_2400_09"] = "CRC04_Condition Indicator";


    DataElement_Name_Pairs["CAS01"] = "CAS01_Claim Adjustment Group Code";
    DataElement_Name_Pairs["CAS02"] = "CAS02_Claim Adjustment Reason Code";
    DataElement_Name_Pairs["CAS03"] = "CAS03_Monetary Amount";
    DataElement_Name_Pairs["CAS04"] = "CAS04_Quantity";
    DataElement_Name_Pairs["CAS05"] = "CAS05_Claim Adjustment Reason Code";
    DataElement_Name_Pairs["CAS06"] = "CAS06_Monetary Amount";
    DataElement_Name_Pairs["CAS07"] = "CAS07_Quantity";
    DataElement_Name_Pairs["CAS08"] = "CAS08_Claim Adjstment Reason Code";
    DataElement_Name_Pairs["CAS09"] = "CAS09_Monetary Amount";
    DataElement_Name_Pairs["CAS10"] = "CAS10_Quantity";
    DataElement_Name_Pairs["CAS11"] = "CAS11_Claim Adjustment Reason Code";
    DataElement_Name_Pairs["CAS12"] = "CAS12_Monetary Amount";
    DataElement_Name_Pairs["CAS13"] = "CAS13_Quantity";
    DataElement_Name_Pairs["CAS14"] = "CAS14_Claim Adjustment Reason Code";
    DataElement_Name_Pairs["CAS15"] = "CAS15_Monetary Amount";
    DataElement_Name_Pairs["CAS16"] = "CAS16_Quantity";
    DataElement_Name_Pairs["CAS17"] = "CAS17_Claim Adjustment Reason Code";
    DataElement_Name_Pairs["CAS18"] = "CAS18_Monetary Amount";
    DataElement_Name_Pairs["CAS19"] = "CAS19_Quantity";

    DataElement_Name_Pairs["CAS01_2320"] = "CAS01_Claim Adjustment Group Code";
    DataElement_Name_Pairs["CAS02_2320"] = "CAS02_Adjustment Reason Code";
    DataElement_Name_Pairs["CAS03_2320"] = "CAS03_Adjustment Amount";
    DataElement_Name_Pairs["CAS04_2320"] = "CAS04_Adjustment Quantity";
    DataElement_Name_Pairs["CAS05_2320"] = "CAS05_Adjustment Reason Code";
    DataElement_Name_Pairs["CAS06_2320"] = "CAS06_Adjustment Amount";
    DataElement_Name_Pairs["CAS07_2320"] = "CAS07_Adjustment Quantity";
    DataElement_Name_Pairs["CAS08_2320"] = "CAS08_Adjustment Reason Code";
    DataElement_Name_Pairs["CAS09_2320"] = "CAS09_Adjustment Amount";
    DataElement_Name_Pairs["CAS10_2320"] = "CAS10_Adjustment Quantity";
    DataElement_Name_Pairs["CAS11_2320"] = "CAS11_Adjustment Reason Code";
    DataElement_Name_Pairs["CAS12_2320"] = "CAS12_Adjustment Amount";
    DataElement_Name_Pairs["CAS13_2320"] = "CAS13_Adjustment Quantity";
    DataElement_Name_Pairs["CAS14_2320"] = "CAS14_Adjustment Reason Code";
    DataElement_Name_Pairs["CAS15_2320"] = "CAS15_Adjustment Amount";
    DataElement_Name_Pairs["CAS16_2320"] = "CAS16_Adjustment Quantity";
    DataElement_Name_Pairs["CAS17_2320"] = "CAS17_Adjustment Reason Code";
    DataElement_Name_Pairs["CAS18_2320"] = "CAS18_Adjustment Amount";
    DataElement_Name_Pairs["CAS19_2320"] = "CAS19_Adjustment Quantity";

    DataElement_Name_Pairs["PER01_1000A"] = "PER01_Contact Function Code";
    DataElement_Name_Pairs["PER02_1000A"] = "PER02_Submitter Contact Name";
    DataElement_Name_Pairs["PER03_1000A"] = "PER03_Communication Number Qualifier";
    DataElement_Name_Pairs["PER04_1000A"] = "PER04_Communication Number";
    DataElement_Name_Pairs["PER05_1000A"] = "PER05_Communication Number Qualifier";
    DataElement_Name_Pairs["PER06_1000A"] = "PER06_Communication Number";
    DataElement_Name_Pairs["PER07_1000A"] = "PER07_Communication Number Qualifier";
    DataElement_Name_Pairs["PER08_1000A"] = "PER08_Communication Number";
*/
    
    map_to_json(DataElement_Name_Pairs,"DataElementNamePairs");
}
vector<string> HandleFileInput(string FileName)
{
    FileName = "InputFiles/"+FileName;
ifstream inputFile(FileName);
    string segment;
    vector<string> List_Lines;
    char ch;
    while (inputFile.get(ch)) 
    {
      if(ch != '\n' && ch != '\r' && ch!='~' )
        segment=segment+ch;
      if(ch=='~')
      {

        List_Lines.push_back(segment);
        segment.clear();
      }
    }
    inputFile.close();
    return List_Lines;
    
}

bool isBeginningOfLoop(string l)
{
    vector<string> SplitLine = splitAndStore(l);
    vector<string> LoopSegmentNames {"NM1", "HL","CLM","LX"};  
    string SegName = SplitLine.at(0);
    if (std::find(LoopSegmentNames.begin(), LoopSegmentNames.end(), SegName) != LoopSegmentNames.end())
        return true;
    else if(SplitLine.at(0)=="SBR" && SplitLine.at(1)=="S") //SBR special case can be within a loop.
        return true;
    else
        return false;

    
    
    
}
void MapLoop(vector<string> Loops, map<string,string> pairs,map<string,map<string, map<string,map<string,string>> >>& fullJSON,map<string, map<string,map<string,string>>>&temp2,map<string,map<string,string>>& temp,string& ClaimType)
{
    //cout <<"\n\n\n";
    //PrintVect(Loops);
    map<string,string> mappedPairs;
    string loopName = "empty";
    string LineSegmentName;
    string LoopIdentifier;
    string Segment_Specific_Identifier ="";
    string prof = "222A1";
    string inst = "223";
    //map<string,map<string,string>> temp;
    //map<string, map<string,map<string,string>>>temp2;
    for(unsigned int i = 0;i<Loops.size();i++)
    {
        vector<string> CurrLineList = splitAndStore(Loops.at(i));
        //PrintVect(CurrLineList);
        LineSegmentName = CurrLineList.at(0);
        
        if(LineSegmentName=="GS")
        {
            if(CurrLineList.at(8).find(prof)!=std::string::npos)
                ClaimType="";
            else if(CurrLineList.at(8).find(inst)!=std::string::npos)
                ClaimType="I";
        }

        if(isBeginningOfLoop(Loops.at(i))) 
        {
            if(LineSegmentName=="NM1") // Check for NM1 Loops
            {
                LoopIdentifier=CurrLineList.at(1);
                if(LoopIdentifier=="41") // 1000A
                {
                    loopName = "1000A";
                }
               /* else if (LoopIdentifier=="82")
                {
                    loopName="2330G";
                }*/
                else if (LoopIdentifier=="DN")
                {
                    loopName="2330C";
                }
                else if(LoopIdentifier=="45")
                {
                    loopName="2420H";
                }
                else if(LoopIdentifier=="QB")
                {
                    loopName="2420B";
                }
                else if(LoopIdentifier=="DK")
                {
                    loopName="2420E";
                }
                else if(LoopIdentifier == "40") //2010AA
                {
                    loopName = "1000B";
                }
                else if(LoopIdentifier == "85") //
                {
                    loopName = "2010AA";
                }
                else if(LoopIdentifier == "87")
                {
                    loopName = "2010AB";
                }
                else if(LoopIdentifier == "PE")
                {
                    loopName = "2010AC";
                }
                else if(LoopIdentifier == "IL")
                {
                    loopName = "2010BA";
                }
                else if(LoopIdentifier == "PR")
                {
                    loopName = "2010BB";
                }
                else if(LoopIdentifier == "QC")
                {
                    loopName = "2010CA";
                }
                else if(LoopIdentifier == "DN"||LoopIdentifier=="71")
                {
                    loopName = "2310A";
                }
                else if(LoopIdentifier == "82")
                {
                    loopName = "2310B";
                }
                else if(LoopIdentifier == "DQ")
                {
                    loopName = "2420D";
                }
                else if(LoopIdentifier == "PW")
                {
                    loopName = "2310E";
                }
                else if(LoopIdentifier == "77")
                {
                    loopName = "2310C";
                }
                else if(LoopIdentifier == "XX")
                {
                    loopName = "2420A";
                }
                else if (LoopIdentifier=="P3")
                {
                    loopName ="2310A";
                }

            }
            else if(LineSegmentName=="HL")
            {
                LoopIdentifier=CurrLineList.at(3);
                if(LoopIdentifier == "22")
                {
                    loopName = "2000B";
                }
                else if (LoopIdentifier == "23")
                {
                    loopName = "2000C";
                }
                else
                {
                    loopName = "2000A";
                }
            }
            else if(LineSegmentName=="CLM")
            {
                loopName = "2300";
            }
            else if(LineSegmentName=="LX")
            {
                loopName = "2400";
            }
            else if(LineSegmentName=="SBR")
            {
                loopName = "2320";
            }

            //cout<<"Loop: "<<loopName<<endl;

            CurrLineList.erase(CurrLineList.begin());

            mappedPairs = Map_Elements_Loop(CurrLineList,Loops.at(i),pairs,loopName,Segment_Specific_Identifier,ClaimType);
            //PrintMap(mappedPairs);
        }   
        else if(loopName!="empty"&&LineSegmentName!="SE") // loop number has been found.
        {
            if(LineSegmentName=="DTP" || LineSegmentName=="REF"||LineSegmentName=="HL"||LineSegmentName=="QTY"||LineSegmentName=="CRC"||LineSegmentName=="AMT")
            {
                Segment_Specific_Identifier = CurrLineList.at(1);
            }
            else if (LineSegmentName=="NM1" && (CurrLineList.at(1)=="DN"||CurrLineList.at(1)=="P3")  )
            {
                Segment_Specific_Identifier=CurrLineList.at(1);
            }
            else if (LineSegmentName=="HI")
            {
                Segment_Specific_Identifier=CurrLineList.at(1).substr(0,2);
            }
        
        
            
            CurrLineList.erase(CurrLineList.begin());
            mappedPairs = Map_Elements_Loop(CurrLineList,Loops.at(i),pairs,loopName,Segment_Specific_Identifier,ClaimType);
            //PrintMap(mappedPairs);
        }
        else // header segments
        {
            if(LineSegmentName=="SE"||LineSegmentName=="GE"||LineSegmentName=="IEA")
            {
                loopName="Trailer";
                temp.clear();
            }
            else if(LineSegmentName=="ISA"||LineSegmentName=="GS"||LineSegmentName=="ST"||LineSegmentName=="BHT")
            {
                loopName="Header";
                temp.clear();
            }

 
            CurrLineList.erase(CurrLineList.begin());
            mappedPairs=Map_Elements_General(CurrLineList,Loops.at(i),pairs);
            //PrintMap(mappedPairs);

        }
        //PrintMap(mappedPairs);
        if(Segment_Specific_Identifier!="")
            LineSegmentName= LineSegmentName+"_"+Segment_Specific_Identifier;

        temp[LineSegmentName] = mappedPairs;
        Segment_Specific_Identifier = "";

    }
        if(temp2.find(loopName)==temp2.end()||loopName=="Trailer")
        {
           // cout <<"Did not find:" <<loopName<<endl;
            temp2[loopName] = temp;
        }    
        else
        {
           // cout <<"Did find:" <<loopName<<endl;
            temp2[loopName+"_1"] = temp;
        }
        
            
    temp.clear();
}


void RunNewFile(string filen,map<string,string> DataElement_Name_Pairs)
{
    
    vector<string> List_Lines = HandleFileInput(filen);
    vector<vector<vector<string>>> ListClaims;
    vector<vector<string>> ListLoops;
    string line_current;
    map<string,map<string,map<string,map<string,string>>>> fullFile;
    vector<string> currLoop;
    
   for(unsigned int i =0;i<List_Lines.size();i++)
   {

    line_current = List_Lines.at(i);
    if(line_current.substr(0,2) =="SE")
    {
        currLoop.push_back(line_current);
        ListLoops.push_back(currLoop);
        ListClaims.push_back(ListLoops);
        currLoop.clear();
        ListLoops.clear();
    }
    else if(isBeginningOfLoop(line_current))
    {
        ListLoops.push_back(currLoop);
        currLoop.clear();
        currLoop.push_back(line_current);
    }
    else
    {
        currLoop.push_back(line_current);
    }
   }
if (!currLoop.empty()) {
    ListLoops.push_back(currLoop);
}
if (!ListLoops.empty()) {
    ListClaims.push_back(ListLoops);
}

    vector<vector<string>> lastElement = ListClaims.back();

    // Append the last element to the second-to-last element
    ListClaims[ListClaims.size() - 2].insert(
        ListClaims[ListClaims.size() - 2].end(),
        lastElement.begin(),
        lastElement.end()
    );



    // Remove the last element
    ListClaims.pop_back();
    vector<vector<string>> LastClaim = ListClaims.back();
    vector<string> SecondToLastLoop = LastClaim.at(LastClaim.size()-2);
    string SE_Segment_to_insert = SecondToLastLoop.back();

    ListClaims.back().at(LastClaim.size()-2).pop_back();

    ListClaims.back().back().insert(ListClaims.back().back().begin(), SE_Segment_to_insert);

    map<string, map<string,map<string,string>>>temp2;
    map<string,map<string,string>> temp;
    string ClaimType = "";
    for(unsigned int i = 0;i<ListClaims.size();i++)
    {
        string claimNum = "Claim " + to_string(i+1);
        //cout <<"Claim "<<i+1<<endl;
        for(unsigned int j = 0;j<ListClaims.at(i).size();j++)
        {
        //PrintVect(ListClaims.at(i).at(j));
        //cout<<endl;
        MapLoop(ListClaims.at(i).at(j),DataElement_Name_Pairs,fullFile,temp2,temp,ClaimType);
        temp.clear();
        }
    fullFile[claimNum] = temp2;
    temp2.clear();
    }
    map_to_json(fullFile,filen);
    

   
}
vector<string> IterateThroughFolder(string DirectoryName)
{
    std::string path = DirectoryName; // Specify your directory path here
    //path = "837_test_files";
    std::vector<std::string> file_names;

    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry.status())) {
                file_names.push_back(entry.path().filename().string());
                
            }
        }

        // Print the file names

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "General exception: " << e.what() << std::endl;
    }
    return file_names;
}
int main() 
{  

    
    cout <<"Running New File" << endl;
    string directory = "InputFiles";        // specify directory here
      
    vector<string> FileNames = IterateThroughFolder(directory);
    PrintVect(FileNames);
    map<string,string> pairs;
    initialize_data_pairs(pairs);
    


    for(unsigned int i = 0;i<FileNames.size();i++)
    {
        //cout<<endl<<endl<<endl<<"Running "<<FileNames.at(i)<<endl;
        RunNewFile(FileNames.at(i),pairs);
        //cout<<endl<<endl<<endl;
    }
    return 0;

}
