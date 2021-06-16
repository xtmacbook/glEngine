
#include "gLQuery.h"
#include "gls.h"

#define _QC_

float GLQuery::getCurrentTime()
{
	GLint64 timer;

	/*
		he time after all previous commands have reached the GPU,that these previous commands may or may not have been fully completed. glGet returns as soon as the time is obtained, without waiting for the completion of any other previous OpenGL commands.
	*/
	glGetInteger64v(GL_TIMESTAMP,&timer);

	//milliseconds
	return timer / 1000000.0;
}

GLQuery::GLQuery()
{
	queryBackBuffer_ = 0;
	queryFrontBuffer_ = 1;
}

void GLQuery::genQueries()
{
	glGenQueries(1, queryID_[queryBackBuffer_]);
	glGenQueries(1, queryID_[queryFrontBuffer_]);

	// dummy query to prevent OpenGL errors from popping out
	// issue the first query
	// Records the time only after all previous 
	// commands have been completed
	//glQueryCounter(queryID_[queryFrontBuffer_][0], GL_TIMESTAMP);
}

void GLQuery::swapQueryBuffers()
{
	if (queryBackBuffer_)
	{
		queryBackBuffer_ = 0;
		queryFrontBuffer_ = 1;
	}
	else 
	{
		queryBackBuffer_ = 1;
		queryFrontBuffer_ = 0;
	}
}

void GLQuery::begin()
{
	glBeginQuery(GL_TIME_ELAPSED, queryID_[queryBackBuffer_][0]);

}

void GLQuery::end()
{
	glEndQuery(GL_TIME_ELAPSED);
}

math::uint64 GLQuery::time()
{
	if (first_)
	{
		first_ = !first_;
		return 1;
	}

	GLint done = false;

	while (!done) {
		glGetQueryObjectiv(queryID_[queryFrontBuffer_][0],
			GL_QUERY_RESULT_AVAILABLE,
			&done);
	}

	// get the query result
	
	GLuint64 timer;
	glGetQueryObjectui64v(queryID_[queryFrontBuffer_][0],GL_QUERY_RESULT,&timer);

	return timer;
}

void GLQuery::deleteQuery()
{
	glDeleteQueries(1, queryID_[queryFrontBuffer_]);
	glDeleteQueries(1, queryID_[queryBackBuffer_]);

}
