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
 
 #include "BluetoothSocket.h"
 using namespace std;
 
///////////////////////////////////////////////////////////////////////////////
// BluetoothException
///////////////////////////////////////////////////////////////////////////////

BluetoothException::BluetoothException(const string &message) throw() :
  runtime_error(message) {
}

BluetoothException::BluetoothException(const string &message, const string &detail) throw() :
  runtime_error(message + ": " + detail) {
}

///////////////////////////////////////////////////////////////////////////////
// BluetoothAddress
///////////////////////////////////////////////////////////////////////////////
 
 BluetoothAddress::BluetoothAddress(const char *host, uint8_t channel, AddressType atype) 
 throw(BluetoothException)  {
	addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = channel;
	switch (atype) {
		case BLUETOOTH_SOCKET: str2ba( host, &addr.rc_bdaddr ); break;
		case BLUETOOTH_SERVER:  addr.rc_bdaddr = {0}; break;
		default: throw BluetoothException("invalid addresstype");
	}
	addrLen = sizeof(addr);
 }
 
 BluetoothAddress::BluetoothAddress(sockaddr *addrVal, socklen_t addrLenVal) {
	addrLen = addrLenVal;
	memcpy( &addr, addrVal, addrLen );
 }
 
 std::string BluetoothAddress::getAddress() const throw(BluetoothException) {
	 char dest[19];
	 ba2str( &addr.rc_bdaddr, dest );
	 return string(dest);
 }
 
 uint8_t BluetoothAddress::getChannel() const throw(BluetoothException) {
	 return addr.rc_channel;
 }
 
 
 BluetoothAddress Socket::getLocalAddress() throw(BluetoothException) {
	 throw BluetoothException("getLocalAddress not implemented");
 }
 
  ///////////////////////////////////////////////////////////////////////////////
// Socket
///////////////////////////////////////////////////////////////////////////////
 
 Socket::Socket() {

  // No socket descriptor defined yet.
  sockDesc = -1;
}

Socket::~Socket() {
  
}

void Socket::close() {

  ::close(sockDesc);
  sockDesc = -1;
}


///////////////////////////////////////////////////////////////////////////////
// CommunicatingSocket
///////////////////////////////////////////////////////////////////////////////

void CommunicatingSocket::send(const void *buffer, int bufferLen)
  throw(BluetoothException) {
  if (::send(sockDesc, (raw_type *)buffer, bufferLen, 0) < 0) {
    throw BluetoothException("Send failed (send())");
  }
}

size_t CommunicatingSocket::recv(void *buffer, int bufferLen)
  throw(BluetoothException) {
  int rtn = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0);
  if (rtn < 0) {
    throw BluetoothException("Receive failed (recv())");
  }

  return rtn;
}

size_t CommunicatingSocket::recvFully(void *buffer, int bufferLen)
  throw(BluetoothException) {
  int rcount = 0;
  int len = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0);
  while (len > 0 && rcount + len < bufferLen) {
    rcount += len;
    len = ::recv(sockDesc, (raw_type *) (((char *) buffer) + rcount),
                 bufferLen - rcount, 0);
  }

  if (len < 0)
    throw BluetoothException("Receive failed (recv())");

  return rcount + len;
}


BluetoothAddress CommunicatingSocket::getForeignAddress() throw(BluetoothException) {
  sockaddr addr;
  socklen_t addrLen;

  if (getpeername(sockDesc,  &addr, &addrLen) < 0) {
    throw BluetoothException("Fetch of foreign address failed (getpeername())");
  }

  return BluetoothAddress(&addr, addrLen);
  
  
}

///////////////////////////////////////////////////////////////////////////////
// SocketStreamBuffer
///////////////////////////////////////////////////////////////////////////////

/** Subclass of basic_streambuf for reading and writing to
    instances of BluetoothSocket. */
template <class CharT, class Traits = std::char_traits<CharT> >
class SocketStreamBuffer : public std::basic_streambuf<CharT, Traits> {
public:
  typedef typename Traits::int_type                 int_type;

  SocketStreamBuffer(BluetoothSocket *sock) {
    // Save the a copy of the socket we are wrapping.
    SocketStreamBuffer::sock = sock;
    this->setg(inBuffer, inBuffer + sizeof(inBuffer),
         inBuffer + sizeof(inBuffer));
    this->setp(outBuffer, outBuffer + sizeof(outBuffer));
    extra = 0;
  }

protected:
  int_type overflow(int_type c = Traits::eof()) {
    // Push out anything in the buffer.
    sync();

    // If an extra character was passed in, put it into our buffer.
    if (c != Traits::eof()) {
      this->sputc(Traits::to_char_type(c));
    }

    return 0;
  }

  int sync() {
    // Write out the contents of the buffer.
    sock->send(outBuffer, (this->pptr() - outBuffer) * sizeof(CharT));
    this->setp(outBuffer, outBuffer + sizeof(outBuffer));
    return 0;
  }

  int_type underflow() {
    // Read up to a buffer full.
    size_t len = sock->recv(inBuffer, sizeof(inBuffer) * sizeof(CharT));

    // Report eof if we didn't get anything.
    if (len == 0) {
      return Traits::eof();
    }

    // Adjust the base class buffer pointers and return the first
    // character read.
    this->setg(inBuffer, inBuffer, inBuffer + len / sizeof(CharT));
    return this->sgetc();
  }

private:
  // Pointer to the socket used as the associated sequence.
  BluetoothSocket *sock;

  // Input and output buffers for characters waiting to write or ready
  // to read.
  CharT inBuffer[1024];
  CharT outBuffer[1024];

  // Number of extra bytes remaining from the last read but not returned
  // to the caller because they are fewer than sizeof(CharT)
  // We don't use this yet, but we should.
  size_t extra;
};

///////////////////////////////////////////////////////////////////////////////
// BluetoothSocket
///////////////////////////////////////////////////////////////////////////////

BluetoothSocket::BluetoothSocket() {
  myStream = NULL;
}

BluetoothSocket::~BluetoothSocket() {
  if (myStream != NULL) {
    delete myStream;
    delete myStreambuf;
  }
}

BluetoothSocket::BluetoothSocket(const char *foreignAddress, uint8_t foreignChannel)
  throw(BluetoothException) {
	myStream = NULL;
	sockDesc = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(sockDesc < 0) throw BluetoothException("error creating socket");
  
  
  // Try to connect the socket to the requested foreign address.
	BluetoothAddress btaddr(foreignAddress, foreignChannel);
    connect(btaddr);
}

BluetoothSocket::BluetoothSocket(int desc) {
  myStream = NULL;
  sockDesc = desc;
}


void BluetoothSocket::connect(const BluetoothAddress &foreignAddress)
  throw(BluetoothException) {

  if (::connect(sockDesc, foreignAddress.getSockaddr(),
                foreignAddress.getSockaddrLen()) < 0)
    throw BluetoothException(string("Call to connect() failed :  ") +
                          strerror(errno));
}

iostream& BluetoothSocket::getStream()  {
  if (myStream == NULL) {
    myStreambuf = new SocketStreamBuffer<char>(this);
    myStream = new iostream(myStreambuf);
  }
  return *myStream;
}

MessageBox& BluetoothSocket::getMessageBox() {
	if (myMessageBox == NULL) {
		myMessageBox = new MessageBox(getStream());
	}
	return *myMessageBox;
}
		

///////////////////////////////////////////////////////////////////////////////
// BluetoothServerSocket
///////////////////////////////////////////////////////////////////////////////


BluetoothServerSocket::BluetoothServerSocket() {
}

BluetoothServerSocket::BluetoothServerSocket(uint8_t localChannel, int queueLen)
  throw(BluetoothException) {
  
	sockDesc = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(sockDesc < 0) throw BluetoothException("error creating socket");
	//BluetoothAddress locaddr(NULL, localChannel, BluetoothAddress::BLUETOOTH_SERVER);
	//bind(locaddr);
	struct sockaddr_rc loc_addr = { 0 };
	loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = {0};
    loc_addr.rc_channel = localChannel;
    ::bind(sockDesc, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
	setListen(queueLen);
}

void BluetoothServerSocket::bind(const BluetoothAddress &localAddress)
  throw(BluetoothException) {

  if (::bind(sockDesc, localAddress.getSockaddr(), localAddress.getSockaddrLen()) < 0)
    throw BluetoothException(string("Call to bind() failed :  ") + strerror(errno));

}

BluetoothSocket *BluetoothServerSocket::accept() throw(BluetoothException) {
  int newConnSD;
  if ((newConnSD = ::accept(sockDesc, NULL, 0)) < 0) {
    throw BluetoothException("Accept failed (accept())");
  }

  return new BluetoothSocket(newConnSD);
}

void BluetoothServerSocket::setListen(int queueLen) throw(BluetoothException) {
  if (listen(sockDesc, queueLen) < 0) {
    throw BluetoothException("Set listening socket failed (listen())");
  }
}

///////////////////////////////////////////////////////////////////////////////
// MessageBox
///////////////////////////////////////////////////////////////////////////////

MessageBox::MessageBox(std::iostream& theStream):theStream(theStream) {
	thr = new std::thread(&MessageBox::handleMessages, this);
	thr->detach();
	running = true;
}

 
MessageBox::~MessageBox() {
	delete thr;
}

std::string MessageBox::readMessage() {
	mtx.lock();
	if(theQueue.empty()) {
		std::string retval = "";
		mtx.unlock();
		return retval;
	}
	else {
		std::string retval = theQueue.front();
		theQueue.pop();
		mtx.unlock();
		return retval;
	}
}

void MessageBox::handleMessages() {
	std::string message;
	try {
		while(theStream >> message) {
			mtx.lock();
			theQueue.push(message);
			mtx.unlock();
		}
	}
	catch(BluetoothException& be) {
		cout << "connection lost" << endl;
		running = false;
	}
}