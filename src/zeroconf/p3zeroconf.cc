/*
 * libretroshare/src/zeroconf: p3zeroconf.cc
 *
 * ZeroConf interface for RetroShare.
 *
 * Copyright 2011-2011 by Robert Fernie.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License Version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to "retroshare@lunamutt.com".
 *
 */


#include "zeroconf/p3zeroconf.h"
#include <openssl/sha.h>
#include <sstream>
#include <iostream>

/* TODO
 *
 * - Get Local Port when it changes.... (need second interface?)
 * - Get GpgId, SslId for all peers.
 * - Able to install new sslIds for friends.
 * - Hold data for all peers.
 * - parse TxtRecord.
 *
 */

#define DEBUG_ZEROCONF	1


#define ZC_SERVICE_STOPPED	0
#define ZC_SERVICE_ACTIVE	1


p3ZeroConf::p3ZeroConf(std::string gpgid, std::string sslid, pqiConnectCb *cb, p3NetMgr *nm)
	:pqiNetAssistConnect(sslid, cb), mNetMgr(nm), mZcMtx("p3ZeroConf")
{
	mRegistered = false;
	mTextOkay = false;
	mPortOkay = false;

	mOwnGpgId = gpgid;
	mOwnSslId = sslid;

#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::p3ZeroConf()" << std::endl;
	std::cerr << "Using Id: " << sslid;
	std::cerr << std::endl;
#endif

	mRegisterStatus == ZC_SERVICE_STOPPED;
	mBrowseStatus == ZC_SERVICE_STOPPED;
	mResolveStatus == ZC_SERVICE_STOPPED;
	mQueryStatus == ZC_SERVICE_STOPPED;

	createTxtRecord();
}

p3ZeroConf::~p3ZeroConf()
{

}


void    p3ZeroConf::start()
{
#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::start()";
	std::cerr << std::endl;
#endif

}

	/* pqiNetAssist - external interface functions */
void    p3ZeroConf::enable(bool on)
{
#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::enable(" << on << ")";
	std::cerr << std::endl;
#endif

}
	
void    p3ZeroConf::shutdown() /* blocking call */
{
}


void	p3ZeroConf::restart()
{
}

bool    p3ZeroConf::getEnabled()
{
	return true;
}

bool    p3ZeroConf::getActive()
{
	return true;
}

bool    p3ZeroConf::getNetworkStats(uint32_t &netsize, uint32_t &localnetsize)
{
	//netsize = mUdpBitDht->statsNetworkSize();
	//localnetsize = mUdpBitDht->statsBDVersionSize();
	return true;
}

void 	p3ZeroConf::createTxtRecord()
{
	RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/
	std::ostringstream gpgidout;
	std::ostringstream sslidout;

	gpgidout << "gpgid=" << mOwnGpgId;	
	sslidout << "sslid=" << mOwnSslId;	

	std::string gpgid = gpgidout.str();
	std::string sslid = sslidout.str();

	mTextRecord += (char) gpgid.length();
	mTextRecord += gpgid;
	mTextRecord += (char) sslid.length();
	mTextRecord += sslid;

	std::cerr << "p3ZeroConf::createTxtRecord() Record: " << std::endl;
	std::cerr << "------------------------------------" << std::endl;
	std::cerr << mTextRecord;
	std::cerr << std::endl;
	std::cerr << "------------------------------------" << std::endl;

	mTextOkay = true;
}

        /*** OVERLOADED from pqiNetListener ***/
bool 	p3ZeroConf::resetListener(struct sockaddr_in &local)
{
	RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/

	mLocalPort = ntohs(local.sin_port);
	mPortOkay  = true;

	if (mRegisterStatus == ZC_SERVICE_ACTIVE)
	{
		locked_stopRegister();
	}
	return true;
}


/*****************************************************************************
 *********  pqiNetAssistConnect - external interface functions ***************
 *****************************************************************************/

/**** DUMMY FOR THE MOMENT *****/

bool 	p3ZeroConf::findPeer(std::string pid)
{
#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::findPeer(" << pid << ")";
	std::cerr << std::endl;
#endif

	return true;
}

bool 	p3ZeroConf::dropPeer(std::string pid)
{
#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::dropPeer(" << pid << ")";
	std::cerr << std::endl;
#endif

	return true;
}


	/* extract current peer status */
bool 	p3ZeroConf::getPeerStatus(std::string id, 
				struct sockaddr_in &/*laddr*/, struct sockaddr_in &/*raddr*/,
				uint32_t &/*type*/, uint32_t &/*mode*/)
{
	/* remove unused parameter warnings */
	(void) id;

#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::getPeerStatus(" << id << ")";
	std::cerr << std::endl;
#endif

	return false;
}

#if 0
bool 	p3ZeroConf::getExternalInterface(struct sockaddr_in &/*raddr*/,
					uint32_t &/*mode*/)
{

#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::getExternalInterface()";
	std::cerr << std::endl;
#endif


	return false;
}

#endif

bool 	p3ZeroConf::setAttachMode(bool on)
{

#ifdef DEBUG_ZEROCONF
	std::cerr << "p3ZeroConf::setAttachMode(" << on << ")";
	std::cerr << std::endl;
#endif

	return true;
}

int 	p3ZeroConf::addBadPeer(const struct sockaddr_in &addr, uint32_t reason, uint32_t flags, uint32_t age)
{
	return 1;
}

int 	p3ZeroConf::addKnownPeer(const std::string &pid, const struct sockaddr_in &addr, uint32_t flags)
{
	return 1;
}

void 	p3ZeroConf::ConnectionFeedback(std::string pid, int state)
{
	return;
}



/***********************************************************************************/
/*
 *
 *
 * ZeroConf Interface for Retroshare.
 * This will use the pqiNetAssistConnect Interface (like the DHT).
 * 
 * we will advertise ourselves using GPGID & SSLID on the zeroconf interface.
 *
 * Will search local network for any peers.
 * If they are our friends... callback to p3NetMgr to initiate connection.
 *
 * 
 * This should be very easy to implement under OSX.... so we start with that.
 * For Windows, we will use Apple's libraries... so that should be similar too.
 * Linux - won't bother with zeroconf for the moment - If someone feels like implementing that.. ok.
 *
 * Most important is that Windows and OSX can interact.
 *
 * We keep a simple system here....
 * 1) Register.
 * 2) Enable Browsing.
 * 3) Resolve Browse results one at a time.
 * 4) Callback Resolve results.
 *
 *
 */


int p3ZeroConf::tick()
{
	{
		RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/

		
		locked_startRegister();
		locked_startBrowse();
	}


	checkServiceFDs(); // will cause callbacks - if data is ready.

	checkResolveAction();
	checkQueryAction();
	checkQueryResults();
	
	return 0;
}

void p3ZeroConf::checkServiceFDs()
{
	RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/

	std::cerr << "p3ZeroConf::checkServiceFDs()";
	std::cerr << std::endl;
	
	if (mRegisterStatus == ZC_SERVICE_ACTIVE)
	{
		locked_checkFD(mRegisterRef);
	}
	
	if (mBrowseStatus == ZC_SERVICE_ACTIVE)
	{
		locked_checkFD(mBrowseRef);
	}
	
	if (mResolveStatus == ZC_SERVICE_ACTIVE)
	{
		locked_checkFD(mResolveRef);
	}
	
	if (mQueryStatus == ZC_SERVICE_ACTIVE)
	{
		locked_checkFD(mQueryRef);
	}
}


void p3ZeroConf::locked_checkFD(DNSServiceRef ref)
{
	std::cerr << "p3ZeroConf::locked_checkFD() Start";
	std::cerr << std::endl;

	int sockfd = DNSServiceRefSockFD(ref);

	fd_set ReadFDs, WriteFDs, ExceptFDs;
	FD_ZERO(&ReadFDs);
	FD_ZERO(&WriteFDs);
	FD_ZERO(&ExceptFDs);
	
	FD_SET(sockfd, &ReadFDs);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	
	if (select(sockfd + 1, &ReadFDs, &WriteFDs, &ExceptFDs, &timeout) < 0)
	{
		std::cerr << "p3zeroconf::checkFD() Select ERROR";
		std::cerr << std::endl;
		return;
	}
	
	if (FD_ISSET(sockfd, &ReadFDs))
	{
		DNSServiceErrorType err = DNSServiceProcessResult(ref);
		switch(err)
		{
			case 0:
				/* ok */
				break;
			default:
				std::cerr << "DNSServiceProcessResult() ERROR: ";
				std::cerr << displayDNSServiceError(err);
				std::cerr << std::endl;
				break;
		}
	}

	std::cerr << "p3ZeroConf::locked_checkFD() End";
	std::cerr << std::endl;
	return;

}



int p3ZeroConf::checkResolveAction()
{
	RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/

	if (mResolveStatus == ZC_SERVICE_ACTIVE)
	{
		/* Do nothing - if we are already resolving */

		return 0;
	}

	/* iterate through the Browse Results... look for unresolved one - and do it! */

	/* check the new results Queue first */
	if (mBrowseResults.size() > 0)
	{
		std::cerr << "p3ZeroConf::checkResolveAction() Getting Item from BrowseResults";
		std::cerr << std::endl;

		zcBrowseResult br = mBrowseResults.front();
		mBrowseResults.pop_front();

		locked_startResolve(br.interfaceIndex, br.serviceName, br.regtype, br.replyDomain);
	}
	return 1;
}


int p3ZeroConf::checkQueryAction()
{
	RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/

	if (mQueryStatus == ZC_SERVICE_ACTIVE)
	{
		/* Do nothing - if we are already resolving */

		return 0;
	}

	/* check the new results Queue first */
	if (mResolveResults.size() > 0)
	{
		std::cerr << "p3ZeroConf::checkQueryAction() Getting Item from ResolveResults";
		std::cerr << std::endl;

		zcResolveResult rr = mResolveResults.front();
		mResolveResults.pop_front();

		locked_startQueryIp(rr.interfaceIndex, rr.fullname);
	}
	return 1;
}


int p3ZeroConf::checkQueryResults()
{
	RsStackMutex stack(mZcMtx); /****** STACK LOCK MUTEX *******/

	/* check the new results Queue first */
	if (mQueryResults.size() > 0)
	{
		std::cerr << "p3ZeroConf::checkQueryResults() Getting Item from QueryResults";
		std::cerr << std::endl;

		zcQueryResult br = mQueryResults.front();
		mQueryResults.pop_front();

	}
	return 1;
}






/* First Step is to register the DNSService */

void p3ZeroConf_CallbackRegister( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, 
				const char *name, const char *regtype, const char *domain, void *context )
{
	p3ZeroConf *zc = (p3ZeroConf *) context;
	zc->callbackRegister(sdRef, flags, errorCode, name, regtype, domain);
}


int p3ZeroConf::locked_startRegister()
{
	if (mRegistered)
	{
		return 0;
	}

	/* need, port & txtRecord setup already */
	if (!mTextOkay)
	{
		std::cerr << "p3ZeroConf::locked_startRegister()";
		std::cerr << "Text Not Setup";
		std::cerr << std::endl;
		return 0;
	}

	if (!mPortOkay)
	{
		std::cerr << "p3ZeroConf::locked_startRegister()";
		std::cerr << "Port Not Set";
		std::cerr << std::endl;
		return 0;
	}

	if (mRegisterStatus == ZC_SERVICE_ACTIVE)
	{
		return 0;
	}
	

	std::cerr << "p3ZeroConf::locked_startRegister() Registering!";
	std::cerr << std::endl;
	std::cerr << "p3ZeroConf::locked_startRegister() Text: " << mTextRecord;
	std::cerr << std::endl;
	std::cerr << "p3ZeroConf::locked_startRegister() Port: " << mLocalPort;
	std::cerr << std::endl;

	DNSServiceRef *sdRef = &mRegisterRef;
	DNSServiceFlags flags = 0; /* no flags */
	uint32_t interfaceIndex = 0; /* all interfaces */
	char *name = NULL; /* may be NULL */

	std::string regtype = "_librs._tcp"; 
	char *domain = NULL; /* may be NULL for all domains. */
	char *host = NULL; /* may be NULL for default hostname */

	uint16_t port = htons(mLocalPort);
	uint16_t txtLen = mTextRecord.length();
	const void *txtRecord = (void *) mTextRecord.c_str(); 
	DNSServiceRegisterReply callBack = p3ZeroConf_CallbackRegister; 
	void *context = this;

	DNSServiceErrorType errcode = DNSServiceRegister (sdRef, flags, interfaceIndex, 
						name, regtype.c_str(), domain, host, port,
						txtLen, txtRecord, callBack, context);

	if (errcode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::locked_startRegister() ERROR: ";
		std::cerr << displayDNSServiceError(errcode);
		std::cerr << std::endl;
	}
	else
	{
		mRegisterStatus = ZC_SERVICE_ACTIVE;
		mRegistered = true;
	}

	return 1;
}



void p3ZeroConf::callbackRegister( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, 
				const char *name, const char *regtype, const char *domain)
{
	std::cerr << "p3ZeroConf::callbackRegister()";
	std::cerr << std::endl;

	/* handle queryIp */
	if (errorCode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::callbackRegister() FAILED ERROR: ";
		std::cerr << displayDNSServiceError(errorCode);
		std::cerr << std::endl;
		return;
	}

	std::cerr <<  "p3ZeroConf::callbackRegister() Success";
	std::cerr << std::endl;

}


void p3ZeroConf::locked_stopRegister()
{
	std::cerr << "p3ZeroConf::locked_stopRegister()";
	std::cerr << std::endl;

	if (mBrowseStatus != ZC_SERVICE_ACTIVE)
	{
		return;
	}

	DNSServiceRefDeallocate(mRegisterRef);

	mRegisterStatus = ZC_SERVICE_STOPPED;
	mRegistered = false;
}


void p3ZeroConf_CallbackBrowse( DNSServiceRef sdRef, DNSServiceFlags flags, 
				uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    				const char *serviceName, const char *regtype, const char *replyDomain, void *context )
{
	p3ZeroConf *zc = (p3ZeroConf *) context;
	zc->callbackBrowse(sdRef, flags, interfaceIndex, errorCode, serviceName, regtype, replyDomain);
}


int p3ZeroConf::locked_startBrowse()
{
	if (mBrowseStatus == ZC_SERVICE_ACTIVE)
	{
		return 0;
	}
	
	std::cerr << "p3ZeroConf::locked_startBrowse()";
	std::cerr << std::endl;

	DNSServiceRef *sdRef = &mBrowseRef;
	DNSServiceFlags flags = 0;
	uint32_t interfaceIndex = 0;
	const char *regtype = "_librs._tcp";
	const char *domain = NULL;
	DNSServiceBrowseReply callBack = p3ZeroConf_CallbackBrowse;
	void *context = this;

	DNSServiceErrorType errcode = DNSServiceBrowse(sdRef, flags, interfaceIndex, 
						regtype, domain, callBack, context);

	if (errcode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::locked_startBrowse() ERROR: ";
		std::cerr << displayDNSServiceError(errcode);
		std::cerr << std::endl;
	}
	else
	{
		mBrowseStatus = ZC_SERVICE_ACTIVE;
	}

	return 1;
}

 
void p3ZeroConf::callbackBrowse(DNSServiceRef /* sdRef */, DNSServiceFlags flags, 
				uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    				const char *serviceName, const char *regtype, const char *replyDomain)
{
	std::cerr << "p3ZeroConf::callbackBrowse()";
	std::cerr << std::endl;

	/* handle queryIp */
	if (errorCode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::callbackBrowse() FAILED ERROR: ";
		std::cerr << displayDNSServiceError(errorCode);
		std::cerr << std::endl;
		return;
	}

	zcBrowseResult br;

	br.flags = flags;
	br.interfaceIndex = interfaceIndex;
	br.serviceName = serviceName;
	br.regtype = regtype;
	br.replyDomain = replyDomain;

	std::cerr << "p3ZeroConf::callbackBrowse() ";
	std::cerr << " flags: " << flags;
	std::cerr << " interfaceIndex: " << interfaceIndex;
	std::cerr << " serviceName: " << serviceName;
	std::cerr << " regtype: " << regtype;
	std::cerr << " replyDomain: " << replyDomain;
	std::cerr << std::endl;

	mBrowseResults.push_back(br);
}


int p3ZeroConf::locked_stopBrowse()
{
	std::cerr << "p3ZeroConf::locked_stopBrowse()";
	std::cerr << std::endl;

	if (mBrowseStatus != ZC_SERVICE_ACTIVE)
	{
		return 0;
	}

	DNSServiceRefDeallocate(mBrowseRef);
	mBrowseStatus = ZC_SERVICE_STOPPED;
	return 1;
}



void p3ZeroConf_CallbackResolve( DNSServiceRef sdRef, DNSServiceFlags flags, 
				uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    				const char *fullname, const char *hosttarget, uint16_t port, 
				uint16_t txtLen, const unsigned char *txtRecord, void *context )  
{
	p3ZeroConf *zc = (p3ZeroConf *) context;
	zc->callbackResolve(sdRef, flags, interfaceIndex, errorCode, fullname, hosttarget, port, txtLen, txtRecord);

}

void p3ZeroConf::locked_startResolve(uint32_t idx, std::string name, 
				std::string regtype, std::string domain)
{
	if (mResolveStatus == ZC_SERVICE_ACTIVE)
	{
		return;
	}

	std::cerr << "p3ZeroConf::locked_startResolve()";
	std::cerr << std::endl;

	DNSServiceRef *sdRef = &mResolveRef;
	DNSServiceFlags flags = 0;
	DNSServiceResolveReply callBack = p3ZeroConf_CallbackResolve;
	void *context = (void *) this;

	DNSServiceErrorType errcode = DNSServiceResolve(sdRef, flags, idx,
			name.c_str(), regtype.c_str(), domain.c_str(), callBack, context);

	if (errcode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::locked_startResolve() ERROR: ";
		std::cerr << displayDNSServiceError(errcode);
		std::cerr << std::endl;
	}
	else
	{
		mResolveStatus = ZC_SERVICE_ACTIVE;
	}
}

 

void p3ZeroConf::callbackResolve( DNSServiceRef /* sdRef */, DNSServiceFlags flags, 
				uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    				const char *fullname, const char *hosttarget, uint16_t port, 
				uint16_t txtLen, const unsigned char *txtRecord)
{
	std::cerr << "p3ZeroConf::callbackResolve()";
	std::cerr << std::endl;

	if (errorCode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::callbackResolve() FAILED ERROR: ";
		std::cerr << displayDNSServiceError(errorCode);
		std::cerr << std::endl;
		return;
	}

	/* handle resolve */
	zcResolveResult rr;

	rr.flags = flags;
	rr.interfaceIndex = interfaceIndex;
	rr.fullname = fullname;
	rr.hosttarget = hosttarget;
	rr.port = port;
	rr.txtLen = txtLen;
	//rr.txtRecord = NULL; //malloc(rr.txtLen);
	//memcpy(rr.txtRecord, txtRecord, txtLen);
	// We must consume the txtRecord and translate into strings...


	std::cerr << "p3ZeroConf::callbackResolve() ";
	std::cerr << " flags: " << flags;
	std::cerr << " interfaceIndex: " << interfaceIndex;
	std::cerr << " fullname: " << fullname;
	std::cerr << " hosttarget: " << hosttarget;
	std::cerr << " port: " << port;
	std::cerr << " txtLen: " << txtLen;
	std::cerr << std::endl;

	mResolveResults.push_back(rr);

	if (flags & kDNSServiceFlagsMoreComing)
	{
		std::cerr << "p3ZeroConf::callbackResolve() Expecting More Results.. keeping Ref";
		std::cerr << std::endl;
	}
	else
	{
		std::cerr << "p3ZeroConf::callbackResolve() Result done.. closing Ref";
		std::cerr << std::endl;

		locked_stopResolve();
	}
}


int p3ZeroConf::locked_stopResolve()
{
	std::cerr << "p3ZeroConf::locked_stopResolve()";
	std::cerr << std::endl;

	if (mResolveStatus != ZC_SERVICE_ACTIVE)
	{
		return 0;
	}

	DNSServiceRefDeallocate(mResolveRef);
	mResolveStatus = ZC_SERVICE_STOPPED;
	return 1;
}




void p3ZeroConf_CallbackQueryIp( DNSServiceRef sdRef, DNSServiceFlags flags, 
				uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    				const char *fullname, uint16_t rrtype, uint16_t rrclass, 
    				uint16_t rdlen, const void *rdata, uint32_t ttl, void *context )  
{
	p3ZeroConf *zc = (p3ZeroConf *) context;
	zc->callbackQueryIp(sdRef, flags, interfaceIndex, errorCode, fullname, rrtype, rrclass, rdlen, rdata, ttl);

}

void p3ZeroConf::locked_startQueryIp(uint32_t idx, std::string fullname)
{
	if (mQueryStatus == ZC_SERVICE_ACTIVE)
	{
		return;
	}

	std::cerr << "p3ZeroConf::locked_startQueryIp()";
	std::cerr << std::endl;


	DNSServiceRef *sdRef = &mQueryRef;
	DNSServiceFlags flags = 0;
	uint16_t rrtype = kDNSServiceType_A;
	uint16_t rrclass = kDNSServiceClass_IN;
	DNSServiceQueryRecordReply callBack = p3ZeroConf_CallbackQueryIp;
	void *context = (void *) this;

	DNSServiceErrorType errcode = DNSServiceQueryRecord(sdRef, flags, idx, 
			fullname.c_str(), rrtype, rrclass, callBack, context);

	if (errcode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::locked_startQueryIp() ERROR: ";
		std::cerr << displayDNSServiceError(errcode);
		std::cerr << std::endl;
	}
	else
	{
		mQueryStatus = ZC_SERVICE_ACTIVE;
	}
}

void p3ZeroConf::callbackQueryIp( DNSServiceRef /* sdRef */, DNSServiceFlags flags, 
				uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
    				const char *fullname, uint16_t rrtype, uint16_t rrclass, 
    				uint16_t rdlen, const void *rdata, uint32_t ttl)
{
	std::cerr << "p3ZeroConf::callbackQueryIp()";
	std::cerr << std::endl;

	/* handle queryIp */
	if (errorCode != kDNSServiceErr_NoError)
	{
		std::cerr <<  "p3ZeroConf::callbackQueryIp() FAILED ERROR: ";
		std::cerr << displayDNSServiceError(errorCode);
		std::cerr << std::endl;
		return;
	}

	/* handle resolve */
	zcQueryResult qr;

	qr.flags = flags;
	qr.interfaceIndex = interfaceIndex;
	qr.fullname = fullname;
	qr.rrtype = rrtype;
	qr.rrclass = rrclass;
	qr.rdlen = rdlen;
	//qr.rdata = NULL; //malloc(rr.rdlen);
	//memcpy(rr.rdata, rdata, rdlen);
	qr.ttl = ttl;

	std::cerr << "p3ZeroConf::callbackQuery() ";
	std::cerr << " flags: " << flags;
	std::cerr << " interfaceIndex: " << interfaceIndex;
	std::cerr << " fullname: " << fullname;
	std::cerr << " rrtype: " << rrtype;
	std::cerr << " rrclass: " << rrclass;
	std::cerr << " rdlen: " << rdlen;
	std::cerr << " ttl: " << ttl;
	std::cerr << std::endl;

	mQueryResults.push_back(qr);

	if (flags & kDNSServiceFlagsMoreComing)
	{
		std::cerr << "p3ZeroConf::callbackQuery() Expecting More Results.. keeping Ref";
		std::cerr << std::endl;
	}
	else
	{
		std::cerr << "p3ZeroConf::callbackQuery() Result done.. closing Ref";
		std::cerr << std::endl;

		locked_stopQueryIp();
	}
}


int p3ZeroConf::locked_stopQueryIp()
{
	std::cerr << "p3ZeroConf::locked_stopQueryIp()";
	std::cerr << std::endl;

	if (mQueryStatus != ZC_SERVICE_ACTIVE)
	{
		return 0;
	}

	DNSServiceRefDeallocate(mQueryRef);
	mQueryStatus = ZC_SERVICE_STOPPED;
	return 1;
}




std::string p3ZeroConf::displayDNSServiceError(DNSServiceErrorType errcode)
{
	std::ostringstream str;
	switch(errcode)
	{
		default:
			str << "kDNSServiceErr_??? UNKNOWN-CODE";
 			break;
	case kDNSServiceErr_NoError:
			str << "kDNSServiceErr_NoError";
 			break;
	case kDNSServiceErr_Unknown:
			str << "kDNSServiceErr_Unknown";
 			break;
	case kDNSServiceErr_NoSuchName:
			str << "kDNSServiceErr_NoSuchName";
 			break;
	case kDNSServiceErr_NoMemory:
			str << "kDNSServiceErr_NoMemory";
 			break;
	case kDNSServiceErr_BadParam:
			str << "kDNSServiceErr_BadParam";
 			break;
	case kDNSServiceErr_BadReference:
			str << "kDNSServiceErr_BadReference";
 			break;
	case kDNSServiceErr_BadState:
			str << "kDNSServiceErr_BadState";
 			break;
	case kDNSServiceErr_BadFlags:
			str << "kDNSServiceErr_BadFlags";
 			break;
	case kDNSServiceErr_Unsupported:
			str << "kDNSServiceErr_Unsupported";
 			break;
	case kDNSServiceErr_NotInitialized:
			str << "kDNSServiceErr_NotInitialized";
 			break;
	case kDNSServiceErr_AlreadyRegistered:
			str << "kDNSServiceErr_AlreadyRegistered";
 			break;
	case kDNSServiceErr_NameConflict:
			str << "kDNSServiceErr_NameConflict";
 			break;
	case kDNSServiceErr_Invalid:
			str << "kDNSServiceErr_Invalid";
 			break;
	case kDNSServiceErr_Firewall:
			str << "kDNSServiceErr_Firewall";
 			break;
	case kDNSServiceErr_Incompatible:
			str << "kDNSServiceErr_Incompatible";
 			break;
	case kDNSServiceErr_BadInterfaceIndex:
			str << "kDNSServiceErr_BadInterfaceIndex";
 			break;
	case kDNSServiceErr_Refused:
			str << "kDNSServiceErr_Refused";
 			break;
	case kDNSServiceErr_NoSuchRecord:
			str << "kDNSServiceErr_NoSuchRecord";
 			break;
	case kDNSServiceErr_NoAuth:
			str << "kDNSServiceErr_NoAuth";
 			break;
	case kDNSServiceErr_NoSuchKey:
			str << "kDNSServiceErr_NoSuchKey";
 			break;
	case kDNSServiceErr_NATTraversal:
			str << "kDNSServiceErr_NATTraversal";
 			break;
	case kDNSServiceErr_DoubleNAT:
			str << "kDNSServiceErr_DoubleNAT";
 			break;
	case kDNSServiceErr_BadTime:
			str << "kDNSServiceErr_BadTime";
 			break;
	case kDNSServiceErr_BadSig:
			str << "kDNSServiceErr_BadSig";
 			break;
	case kDNSServiceErr_BadKey:
			str << "kDNSServiceErr_BadKey";
 			break;
	case kDNSServiceErr_Transient:
			str << "kDNSServiceErr_Transient";
 			break;
	case kDNSServiceErr_ServiceNotRunning:
			str << "kDNSServiceErr_ServiceNotRunning";
 			break;
	case kDNSServiceErr_NATPortMappingUnsupported:
			str << "kDNSServiceErr_NATPortMappingUnsupported";
 			break;
	case kDNSServiceErr_NATPortMappingDisabled:
			str << "kDNSServiceErr_NATPortMappingDisabled";
 			break;
#if 0
	case kDNSServiceErr_NoRouter:
			str << "kDNSServiceErr_NoRouter";
 			break;
	case kDNSServiceErr_PollingMode:
			str << "kDNSServiceErr_PollingMode";
 			break;
#endif
	}

	return str.str();
}



/***********************************************************************************
 * Handle Peer Data, list of friends etc.
 *
 * We need data structures to handle the Info from libretroshare, 
 * and the info from ZeroConf too.
 *
 ****/

/* This needs to be a separate class - as we don't know which peer it is
 * associated with until we have Resolved the details.
 */
	
class RsZCBrowseDetails
{
	public:
	RsZCBrowseDetails();

	uint32_t mBrowseState;
	time_t   mBrowseUpdate;

	uint32_t mBrowseInterfaceIndex;
	std::string mBrowserServiceName;
	std::string mBrowserRegType;
	std::string mBrowserReplyDomain;

	uint32_t mResolveInterfaceIndex;
	std::string mResolveFullname;
	std::string mResolveHostTarget;
	uint32_t mResolvePort;
	std::string mResolveTxtRecord;
};



class RsZCPeerDetails
{
	/* passed from libretroshare */

	std::string mGpgId;
	std::string mSslId;

	uint32_t mPeerStatus; /* FRIEND, FOF, ONLINE, OFFLINE */

	/* Browse Info */

	uint32_t mBrowseState;
	time_t   mBrowseUpdate;

	uint32_t mBrowseInterfaceIndex;
	std::string mBrowserServiceName;
	std::string mBrowserRegType;
	std::string mBrowserReplyDomain;

	/* Resolve Info */

	uint32_t mResolveInterfaceIndex;
	std::string mResolveFullname;
	std::string mResolveHostTarget;
	uint32_t mResolvePort;
	std::string mResolveTxtRecord;
};



	







