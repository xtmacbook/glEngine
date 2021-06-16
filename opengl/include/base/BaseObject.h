//
//  base.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef base_h
#define base_h

#include "decl.h"

namespace base {

	class LIBENIGHT_EXPORT BaseObject
	{
	public:
		BaseObject() : reference_(0) { }

		virtual ~BaseObject() { }

		void            newRef() { ++reference_; }

    void            delRef()
    {
        if (--reference_ == 0)
            delete this;
    }

		unsigned int getRefCount() const { return reference_; }
	private:
		unsigned int reference_;
	};

}
#endif /* base_h */


