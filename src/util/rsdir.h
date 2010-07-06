
/*
 * "$Id: rsdir.h,v 1.1 2007-02-19 20:08:30 rmf24 Exp $"
 *
 * RetroShare C++ Interface.
 *
 * Copyright 2004-2007 by Robert Fernie.
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


#ifndef RSUTIL_DIRFNS_H
#define RSUTIL_DIRFNS_H

#include <string>
#include <list>
#include <stdint.h>

namespace RsDirUtil {

std::string 	getTopDir(std::string);
std::string 	getRootDir(std::string);
std::string 	removeRootDir(std::string path);
std::string     removeTopDir(std::string dir);
std::string 	removeRootDirs(std::string path, std::string root);

bool		hashFile(const std::string& full_path,std::string& hash) ;

// Renames file from to file to. Files should be on the same file system.
//	returns true if succeed, false otherwise.
bool		renameFile(const std::string& from,const std::string& to) ;
bool		createBackup (std::string sFilename, unsigned int nCount = 5);

uint32_t rs_CRC32(const unsigned char *data,uint32_t len) ;

int     	breakupDirList(std::string path,
                        	std::list<std::string> &subdirs);

bool    	checkDirectory(std::string dir);
bool    	checkCreateDirectory(std::string dir);
bool    	cleanupDirectory(std::string dir, std::list<std::string> keepFiles);

bool 		hashFile(std::string filepath,                
		  std::string &name, std::string &hash, uint64_t &size);

bool 		getFileHash(std::string filepath,                
			std::string &hash, uint64_t &size);


std::wstring 	getWideTopDir(std::wstring);
std::wstring 	getWideRootDir(std::wstring);
std::wstring 	removeWideRootDir(std::wstring path);
std::wstring     removeWideTopDir(std::wstring dir);
std::wstring 	removeWideRootDirs(std::wstring path, std::wstring root);

// Renames file from to file to. Files should be on the same file system.
//	returns true if succeed, false otherwise.
bool		renameWideFile(const std::wstring& from,const std::wstring& to) ;

int     	breakupWideDirList(std::wstring path,
                        	std::list<std::wstring> &subdirs);

bool    	checkWideDirectory(std::wstring dir);
bool    	checkWideCreateDirectory(std::wstring dir);
bool    	cleanupWideDirectory(std::wstring dir, std::list<std::wstring> keepFiles);

bool 		hashWideFile(std::wstring filepath,                
		  std::wstring &name, std::string &hash, uint64_t &size);

bool 		getWideFileHash(std::wstring filepath,                
			std::string &hash, uint64_t &size);

}

	
#endif
