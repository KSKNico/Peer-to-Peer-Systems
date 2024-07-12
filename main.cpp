#include <iostream>
#include <future>
#include "peerConfig.hpp"
#include "peer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "sectorHandling/sectorHandler.hpp"
#include "sectorHandling/primeCalculation.hpp"



int main() {

    auto peer_addr_1 = Poco::Net::SocketAddress("127.0.0.1:5001");
    auto peer_addr_2 = Poco::Net::SocketAddress("127.0.0.1:5002");
    auto peer_addr_3 = Poco::Net::SocketAddress("127.0.0.1:5003");
    auto peer_addr_4 = Poco::Net::SocketAddress("127.0.0.1:5004");


    Peer peer_1(peer_addr_1);
    Peer peer_2(peer_addr_2, peer_addr_1);
    Peer peer_3(peer_addr_3, peer_addr_1);
    Peer peer_4(peer_addr_4, peer_addr_1);

    // start two threads that use the run method
    
    std::future<void> future_1 = std::async(std::launch::async, &Peer::run, &peer_1);
    std::future<void> future_2 = std::async(std::launch::async, &Peer::run, &peer_2);
    std::future<void> future_3 = std::async(std::launch::async, &Peer::run, &peer_3);
    std::future<void> future_4 = std::async(std::launch::async, &Peer::run, &peer_4);


    std::unique_lock lock2(peer_2.connectionsMutex);
    Message m = Message("PUT,127.0.0.1:5002,3000,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989");
    peer_2.connections[Hash::hashSocketAddress(peer_addr_1)]->ioInterface.queueOutgoingMessage(m);
    ///*
    Message m2 = Message("PUT,127.0.0.1:5003,3000,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989");


    std::unique_lock lock3(peer_3.connectionsMutex);
    peer_3.connections[Hash::hashSocketAddress(peer_addr_1)]->ioInterface.queueOutgoingMessage(m2);
    Message m3 = Message("PUT,127.0.0.1:5004,3000,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989");

    std::unique_lock lock4(peer_4.connectionsMutex);
    peer_4.connections[Hash::hashSocketAddress(peer_addr_1)]->ioInterface.queueOutgoingMessage(m3);
    peer_4.connections[Hash::hashSocketAddress(peer_addr_1)]->ioInterface.queueOutgoingMessage(m3);
    //*/
    lock2.unlock();
    lock3.unlock();
    lock4.unlock();

    while(1) { /*peer_1.printConnections()*/;}
}