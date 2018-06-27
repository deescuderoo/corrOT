/**
* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
* Copyright (c) 2016 LIBSCAPI (http://crypto.biu.ac.il/SCAPI)
* This file is part of the SCAPI project.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* We request that any publication and/or code referring to and/or based on SCAPI contain an appropriate citation to SCAPI, including a reference to
* http://crypto.biu.ac.il/SCAPI.
*
* Libscapi uses several open source libraries. Please see these projects for any further licensing issues.
* For more information , See https://github.com/cryptobiu/libscapi/blob/master/LICENSE.MD
*
* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*/

#pragma once

#ifndef COMM_HPP
#define COMM_HPP

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <iostream>
using namespace std;

typedef unsigned char byte;

//#include "Message.hpp"
//#include "../infra/Common.hpp"

namespace boost_ip = boost::asio::ip; // reduce the typing a bit later...
using IpAddress = boost_ip::address;
using tcp = boost_ip::tcp;


class TimeoutException : public logic_error
{
public:
    TimeoutException(const string & msg) : logic_error(msg) {};
};

class DuplicatePartyException : public logic_error
{
public:
    DuplicatePartyException(const string & msg) : logic_error(msg) {};
};

class PartyCommunicationException : public logic_error
{
public:
    PartyCommunicationException(const string & msg) : logic_error(msg) {};
};

/**
* A marker interface. Each type of party should have a concrete class that implement this interface.
*/
class PartyData{};

/**
* This class holds the data of a party in a communication layer.
* It should be used in case the user wants to use the regular mechanism of communication using tcp sockets.
*/
class SocketPartyData : public PartyData {
private:
    IpAddress ipAddress; // party's address.
    int port; // port number to listen on.
    int compare(const SocketPartyData &other) const;
public:
    SocketPartyData() {};
    /**
    * Constructor that sets the given arguments.
    * @param ip Party's address.
    * @param port Port number to listen on.
    */
    SocketPartyData(IpAddress ip, int port) {
        ipAddress = ip;
        this->port = port;
    };
    IpAddress getIpAddress() { return ipAddress; };
    int getPort() { return port; };
    string to_log_string() {
        return ipAddress.to_string() + "|" + to_string(port);
    };
    /**
    * Compares two parties.
    *<0 if this party's string is smaller than the otherParty's string representation.
    *>0 if this party's string is larger than the otherParty's string representation.
    */
    bool operator==(const SocketPartyData &other) const { return (compare(other) == 0); };
    bool operator!=(const SocketPartyData &other) const { return (compare(other) != 0); };
    bool operator<=(const SocketPartyData &other) const { return (compare(other) <= 0); };
    bool operator>=(const SocketPartyData &other) const { return (compare(other) >= 0); };
    bool operator>(const SocketPartyData &other) const { return (compare(other) > 0); };
    bool operator<(const SocketPartyData &other) const { return (compare(other) < 0); };
};

/**
* A simple interface that encapsulate all network operations of one peer in a two peers (or more)
* setup.
*/
class CommParty {
public:
    /**
    * This method setups a double edge connection with another party.
    * It connects to the other party, and also accepts connections from it.
    * The method blocks until boths side are connected to each other.
    */
    virtual void join(int sleep_between_attempts, int timeout) = 0;
    /**
    * Write data from @param data to the other party.
    * Will write exactly @param size bytes
    */
    virtual void write(const byte* data, int size) = 0;
    /**
    * Read exactly @param sizeToRead bytes int @param buffer
    * Will block until all bytes are read.
    */
    virtual size_t read(byte* buffer, int sizeToRead) = 0;
    virtual void write(string s) { write((const byte *)s.c_str(), s.size()); };
    virtual void writeWithSize(const byte* data, int size);
    virtual int readSize();
    virtual size_t readWithSizeIntoVector(vector<byte> & targetVector);
    virtual void writeWithSize(string s) { writeWithSize((const byte*)s.c_str(), s.size()); };
    virtual ~CommParty() {};
};

class CommPartyTCPSynced : public CommParty {
public:
    CommPartyTCPSynced(boost::asio::io_service& ioService, SocketPartyData me, SocketPartyData other, int role = 2) : //0 : Server, 1: Client, 2 : Both
            ioServiceServer(ioService), ioServiceClient(ioService),
            acceptor_(ioService, tcp::endpoint(tcp::v4(), (role == 1) ? (10000 + me.getPort()) : me.getPort() )),
            serverSocket(ioService), clientSocket(ioService)
    {
        this->me = me;
        this->other = other;
        this->role = role;
    };
    void join(int sleepBetweenAttempts = 500, int timeout = 5000) override;

    void write(const byte* data, int size) override;
    size_t read(byte* data, int sizeToRead) override {
        return boost::asio::read(socketForRead(), boost::asio::buffer(data, sizeToRead));
    }
    virtual ~CommPartyTCPSynced();

private:
    boost::asio::io_service& ioServiceServer;
    boost::asio::io_service& ioServiceClient;
    tcp::acceptor acceptor_;
    tcp::socket serverSocket;
    tcp::socket clientSocket;
    SocketPartyData me;
    SocketPartyData other;
    int role;
    void setSocketOptions();
    tcp::socket & socketForRead() {
        if (role == 1)
            return clientSocket;
        return serverSocket;
    }

    tcp::socket & socketForWrite() {
        if (role == 0)
            return serverSocket;
        return clientSocket;

    }


};

#endif //COMM_HPP