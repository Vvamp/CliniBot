/*
 *   C++ bluetooth-sockets on Unix
 *   Copyright (C) 2019 Jan Zuurbier
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not,  If not, see <https://www.gnu.org/licenses/>
 */

#ifndef __BLUETOOTHSOCKET_INCLUDED__
#define __BLUETOOTHSOCKET_INCLUDED__


#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
typedef void raw_type;       // Type used for raw data on this platform


#include <stdio.h>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>


/**
 *   Signals a problem with the execution of a socket call.
 */
class BluetoothException : public std::runtime_error {
public:
  /**
   *   Construct a BluetoothException with a user message followed by a
   *   system detail message.
   *   @param message explanatory message
   */
  BluetoothException(const std::string &message) throw();
  
  /**
   *   Construct a BluetoothException with a explanatory message.
   *   @param message explanatory message
   *   @param detail detail message
   */
  BluetoothException(const std::string &message, const std::string &detail) throw();
};


/** 
    Container aggregating an address and a port for a bluetooth-socket.
    BluetoothAddress offers value semantics.
*/
class BluetoothAddress {
public:
  /** Type of address being requested. */
  enum AddressType { BLUETOOTH_SERVER, BLUETOOTH_SOCKET};
  
  /** Make a BluetoothAddress for the given host and channel. */
  BluetoothAddress(const char *host, uint8_t channel, AddressType atype = BLUETOOTH_SOCKET ) throw(BluetoothException);

  

  /** Make a BluetoothAddress that wraps a copy of the given sockaddr
      structure of the given addreLenVal legth in bytes.  If used as a
      default constructur, the BluetoothAddress is created in an
      uninitialized state, and none of its get methods should be used
      until it is initialized. */
  BluetoothAddress(sockaddr *addrVal = NULL, socklen_t addrLenVal = 0);

  /** Return a string representation of the address portion of this
      object. */
  std::string getAddress() const throw(BluetoothException);

  /** Return a numeric value for the port portion of this object. */
  uint8_t getChannel() const throw(BluetoothException);

  /** Return a pointer to the sockaddr structure wrapped by this object. */
  sockaddr *getSockaddr() const {
    return (sockaddr *)&addr;
  }

  /** Return the length of the sockaddr structure wrapped by this object. */
  socklen_t getSockaddrLen() const {
    return addrLen;
  }


private:
  // Raw address portion of this object.
  struct sockaddr_rc addr = {0};

  // Number of bytes used in the addr field.
  socklen_t addrLen = 0;
};

class Socket {
public:
  virtual ~Socket();

  /**
   *   Get the local address
   *   @return local address of socket
   *   @exception BluetoothException thrown if fetch fails
   */
  BluetoothAddress getLocalAddress() throw(BluetoothException);
  
  /** Close this socket. */
  void close();



private:
  // Prevent the user from trying to use value semantics on this object
  Socket(const Socket &sock);
  void operator=(const Socket &sock);

protected:
  /** Socket descriptor, protected so derived classes can read it
      easily (may want to change this) */
  int sockDesc;

  /** You can only construct this object via a derived class. */
  Socket();

};

/**
 *   Abstract base class representing a socket that, once connected, has
 *   a foreign address and can communicate with the socket at that foreign
 *   address.
 */
class CommunicatingSocket : public Socket {
public:
  /**
   *   Write bufferLen bytes from the given buffer to this socket.
   *   The socket must be connected before send() can be called.
   *   @param buffer buffer to be written
   *   @param bufferLen number of bytes from buffer to be written
   *   @exception BluetoothException thrown if unable to send data
   */
  void send(const void *buffer, int bufferLen) throw(BluetoothException);

  /**
   *   Read into the given buffer up to bufferLen bytes data from this
   *   socket.  The socket must be connected before recv can be called.
   *   @param buffer buffer to receive the data
   *   @param bufferLen maximum number of bytes to read into buffer
   *   @return number of bytes read, 0 for EOF.
   *   @exception BluetoothException thrown if unable to receive data
   */
  size_t recv(void *buffer, int bufferLen) throw(BluetoothException);

  /**
   *   Block until bufferLen bytes are read into the given buffer,
   *   until the socket is closed or an error is encoutered.  The
   *   socket must be connected before recvFully can be called.
   *   @param buffer buffer to receive the data
   *   @param bufferLen maximum number of bytes to read into buffer
   *   @return number of bytes read, 0 for EOF, and -1 for error
   *   @exception BluetoothException thrown if unable to receive data
   */
  size_t recvFully(void *buffer, int bufferLen) throw(BluetoothException);

  /**
   *   Get the address of the peer to which this socket is connected.
   *   The socket must be connected before this method can be called.
   *   @return foreign address
   *   @exception BluetoothException thrown if unable to fetch foreign address
   */
  BluetoothAddress getForeignAddress() throw(BluetoothException);
};


class MessageBox {
	public:
	MessageBox(std::iostream &theStream);
	~MessageBox();
	std::string readMessage() ;
	inline bool isRunning() { return running; }
	private:
	bool running = false;
	std::mutex mtx;
	std::iostream& theStream;
	std::thread* thr;
	std::queue<std::string> theQueue;
	void handleMessages();
};

/**
 *   TCP socket for communication with other TCP sockets
 */
class BluetoothSocket : public CommunicatingSocket {
public:
  /**
     Make a socket that is neither bound nor connected.
   */
  BluetoothSocket();

  ~BluetoothSocket();

  /**
   *   Construct a TCP socket with a connection to the given foreign
   *   address and port.  This is interface is provided as a convience
   *   for typical applications that don't need to worry about the
   *   local address and port.  
   *   @param foreignAddress foreign address (IP address or name) 
   *   @param foreignPort foreign port 
   *   @exception BluetoothException thrown if unable to create TCP socket
   */
  BluetoothSocket(const char *foreignAddress, uint8_t foreignChannel) 
    throw(BluetoothException);

  
  /**
     Connect this socket to the given foreign address.
   */
  void connect(const BluetoothAddress &foreignAddress) throw(BluetoothException);

  /**
   *   Return a reference to an I/O stream wrapper around this
   *   CommunicatingSocket.  The caller can use this object to send
   *   and receive text-encoded messages over the socket.  The returned
   *   stream is owned by the socket and is created on the first call
   *   to getStream.
   */
  std::iostream &getStream() ;
  
  MessageBox& getMessageBox();

private:
  // Access for BluetoothServerSocket::accept() connection creation
  friend class BluetoothServerSocket;
  BluetoothSocket(int sockDesc);

  /** iostream associated with this socket, or NULL if it doesn't have
      one. */
  std::iostream *myStream;

  /** Streambuffer managed by myStream. */
  std::streambuf *myStreambuf;
  
   /** messagebox associated with this socket, or NULL if it doesn't have
      one. */  
  MessageBox* myMessageBox;
};

/**
 *   Bluetooth socket class for servers
 */
class BluetoothServerSocket : public Socket {
public:
  /**
     Make an unbound socket.
   */
  BluetoothServerSocket();

  /**
   *   Construct a Bluetooth socket for use with a server, accepting connections
   *   on the specified port on any interface
   *   @param localChannel local port of server socket, a value of zero will
   *                   give a system-assigned unused port
   *   @param queueLen maximum queue length for outstanding 
   *                   connection requests (default 5)
   *   @exception BluetoothException thrown if unable to create Bluetooth server socket
   */
  BluetoothServerSocket(uint8_t localChannel, int queueLen = 5) 
      throw(BluetoothException);

  /**
     Bind this socket to the given local address.
   */
  void bind(const BluetoothAddress &localAddress) throw(BluetoothException);
  
  /**
   *   Blocks until a new connection is established on this socket or error
   *   @return new connection socket
   *   @exception BluetoothException thrown if attempt to accept a new connection fails
   */
  BluetoothSocket *accept() throw(BluetoothException);

private:
  void setListen(int queueLen) throw(BluetoothException);
};


	



#endif
