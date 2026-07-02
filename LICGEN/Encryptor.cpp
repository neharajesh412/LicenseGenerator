//Headers from crypt++
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

//SHA 256 Encryption of license.json
string sha(const string& jsonString)
{
    SHA256 hash;
    string digest;

    StringSource(
        jsonString,
        true,
        new HashFilter(
            hash,
            new HexEncoder(
                new StringSink(digest),
                false        // false = lowercase hex
            )
        )
    );

    return digest;
}

//Sign the JSON directly
void SignJSON(const std::string& jsonString,
              const std::string& privateKeyFile,
              const std::string& signatureFile)
{
    // Load private key
    RSA::PrivateKey privateKey;
    FileSource fs(privateKeyFile.c_str(), true);
    privateKey.Load(fs);

    // Create signer
    AutoSeededRandomPool rng;
    RSASS<PSS, SHA256>::Signer signer(privateKey);

    std::string signature;

    // Sign the JSON (SHA-256 is computed internally)
    StringSource(
        jsonString,
        true,
        new SignerFilter(
            rng,
            signer,
            new StringSink(signature)
        )
    );

    // Save signature
   FileSink out(signatureFile.c_str());
    out.Put(
        (const CryptoPP::byte*)signature.data(),
        signature.size());

    out.MessageEnd();
}




//Generate keys
void GenerateRSAKeys()
{
    // Check if both keys already exist
    if (fs::exists("private.key") && fs::exists("public.key"))
    {
        cout << "RSA key pair already exists.\n";
        return;
    }

    cout << "Generating RSA key pair...\n";

    AutoSeededRandomPool rng;

    // Generate a 2048-bit RSA key pair
    InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 2048);

    RSA::PrivateKey privateKey(params);
    RSA::PublicKey publicKey(params);

    // Save private key
    privateKey.Save(FileSink("private.key").Ref());

    // Save public key
    publicKey.Save(FileSink("public.key").Ref());

    cout << "RSA key pair generated successfully.\n";
}

//Encrypting AES key



// Signing the hash digest


int main(){

    //Reading the JSON
    ifstream input_file("license.json");
    json loaded;
    input_file>>loaded;
    string json_string=loaded.dump(4);
    
    //Creating the Hash digest of the details in json
    string hash=sha(json_string);
    cout<<"HASH: "<<hash<<endl;

    

    //creating PUBLIC & PRIVATE KEYS using RSA
    GenerateRSAKeys();

    //Signing the license
    SignJSON(json_string,"private.key","signature.bin");


}