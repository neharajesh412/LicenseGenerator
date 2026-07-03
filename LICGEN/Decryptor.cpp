#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/secblock.h>
#include <cryptopp/modes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/osrng.h>


#include <iostream>
#include<fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <nlohmann/json.hpp>

using namespace CryptoPP;
namespace fs = std::filesystem;
using json = nlohmann::json;
using namespace std;


bool VerifyJSON(const std::string& jsonString,
                const std::string& publicKeyFile,
                const std::string& signatureFile)
{
    // Load public key
    RSA::PublicKey publicKey;
    FileSource keyFile(publicKeyFile.c_str(), true);
    publicKey.Load(keyFile);

    // Read signature
    string signature;

    FileSource sigFile(
        signatureFile.c_str(),
        true,
        new StringSink(signature)
    );

    // Create verifier
    RSASS<PSS, SHA256>::Verifier verifier(publicKey);

    // Verify
    bool result = verifier.VerifyMessage(
        reinterpret_cast<const CryptoPP::byte*>(jsonString.data()),
        jsonString.size(),

        reinterpret_cast<const CryptoPP::byte*>(signature.data()),
        signature.size()
    );

    return result;
}
void timeLeft(time_t expiry, time_t now){
    
    time_t remaining = expiry-now;
    time_t days = remaining / 86400;
    remaining %= 86400;
    time_t years = days / 365;
    days %= 365;
    time_t months=days/30;
    days%=30;
    time_t hours = remaining / 3600;
    remaining %= 3600;
    time_t minutes = remaining / 60;
    time_t seconds = remaining % 60;
    cout <<"VALIDITY EXPIRES IN "<< years<<"years "<<months<<"months "<<days << "days "<< hours << "h "<< minutes << "m "<< seconds << "s\n";
}

int main(){
    string folder;
    cout<<"Enter License No:";
    cin>>folder;
    fs::current_path(folder);

    ifstream input_file("license.json");
    json loaded;
    input_file>>loaded;
    string json_string=loaded.dump(4);

    bool valid = VerifyJSON(json_string,"public.key","signature.bin");
    if(valid){
        cout << "Signature Verified\n";
        time_t exp=loaded["UTC"];
        time_t now = time(nullptr);
        if(exp>now){
            timeLeft(exp,now);
        }
        else
            cout<<"EXPIRED!";    
    }
    else
        cout << "Signature Invalid\n";
    return 0;
}
