//
//  exception.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef exception_h
#define exception_h

#include <stdio.h>
#include <exception>
#include <string>

namespace base {

	class BaseException : public std::exception {
	protected:
		std::string msg;
	public:
		BaseException() { };
		BaseException(const std::string& vMsg) { msg = vMsg; };
		virtual const char *what() const throw() { return msg.c_str(); }
		const std::string getMsg() const throw() { return msg; }
	};

	struct ExistException : public BaseException {
		ExistException(const std::string& vMsg) : BaseException(vMsg) { };
	};

	struct SingeltonException : public BaseException {
		SingeltonException() : BaseException("tried to create Invalid Instance") { };
		SingeltonException(const std::string& vMsg) : BaseException(vMsg) { };
	};

	struct GLException : public BaseException
	{
		GLException(const std::string&vMsg) :BaseException(vMsg) {}
	};

}

#endif /* exception_h */
