1. Run Details.cpp first to create the JSON file in the format:
{
    "EXPIRY": "2030-04-05 14:20:26",
    "MAC_ID": "3C:7A:B8:12:9F:E4",
    "UTC": 1901609426
}
command: g++ Details.cpp -Iinclude -std=c++17 -o Details


2.Run Encryptor.cpp to encrypt the JSON and sign
->The Crypto++ library is used for encryption and decryption
->The JSON is encrypted using SHA 256.
->Generates RSA public key and private key.(Only run once for a license)
->The JSON is signed with the private key and stored in signature.bin
->Done using RSA Signer of Crypto++ library

command: g++ Encryptor.cpp -Iinclude -std=c++17 -o Encryptor -lcryptopp

3.Run Decryptor.cpp to verify the license is right one. Then the validity is calculated.
->Verifies the JSON string with the RSA public.key and signature.bin
->If the signature is verified then the validity is calculated using Unix Time stamp(UTC).

command: g++ Decryptor.cpp -Iinclude -std=c++17 -o Decryptor -lcryptopp