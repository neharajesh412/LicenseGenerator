#include <iostream>
#include<fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <random>
#include<filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
namespace fs = filesystem;



time_t toUnixTimestamp(const string& timestamp)
{
    tm tm = {};

    istringstream ss(timestamp);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail())
        throw runtime_error("Invalid timestamp format.");

    return mktime(&tm);
}

namespace nlohmann{
    template<typename T>
    struct adl_serializer<optional<T>>
    {
        static void to_json(json &j, const optional<T> &s){
            if(s.has_value()){
                j=*s;
            }

        }

        static void from_json(const json &j,optional<T> &s){
            if(j.is_null()){
                s=nullopt;
            }
            else{
                s=j.get<string>();
            }
        }
    };
}

struct LicenseAttrib{
    string no;
    string macid;
    string exp;
    time_t exp_utc;

};

void readattr(LicenseAttrib &l){
    cout<<"Enter the License No:";
    cin>>l.no;
    cout<<"Enter the following details:\nMAC ID: ";
    cin>>l.macid;
    string exp_dt,exp_tm;
    // l.macid="3C:7A:B8:12:9F:E4";
    cout<<"\nEXPIRY DATE (in the format YYYY-MM-DD): ";
    cin>>exp_dt;
    cout<<"\nEXPIRY TIME (in the format HH:MM:SS): ";
    cin>>exp_tm;
    l.exp=exp_dt+" "+exp_tm;
    l.exp_utc=toUnixTimestamp(l.exp);

}

void to_json(json &j, const LicenseAttrib &l){
    j=json{
        {"MAC_ID",l.macid},
        {"EXPIRY",l.exp},
        {"UTC",l.exp_utc}
    };
}

void from_json(const json &j, LicenseAttrib &l){
   try{
        l.macid=j.at("MAC_ID").get<string>();
        l.exp=j.at("EXPIRY").get<string>();
        l.exp_utc=j.at("UTC").get<time_t>();
    }
    catch(const exception &e){
        cout<<"Invalid JSON format "<<e.what()<<endl;
    }
}


int main(){

    LicenseAttrib l1;
    readattr(l1);    
    
    json j_lic;
    to_json(j_lic,l1); 
    fs::create_directory(l1.no);

    ofstream output_file(l1.no+"/license.json");
    output_file<<j_lic.dump(4);
    output_file.close();

    return 0;
}
