
#include <scene.h>
#include "resource.h"
#include <gLApplication.h>
#include <camera.h>
#include <windowManager.h>
#include <glinter.h>
#include <gli.h>
#include <log.h>
#include <renderNode.h>
#include <engineLoad.h>
#include <texture.h>
#include <helpF.h>
#include <flashBuffer.h>
#include <vertex.h>


class DownloadScene :public Scene
{
public:
	const int screen_size;
	static const int RB_BUFFERS_SIZE = 4;
	static const int format_;
	static const int type_;
	struct Screen_buffer 
	{
		unsigned buffer_;
		unsigned tmp_buffer_;
		unsigned tmp_texture_;
		int frame_num_;
		GLsync fence_;
		unsigned queries_[3];
		int stage_;
		unsigned showtime_buffer_;
	};

private:
	void	initCaptured_screen();

	void    process_captured_screen();

	void	captured_screen();

	Screen_buffer	screen_buffers_rb_[RB_BUFFERS_SIZE];
	
	int rb_head_, rb_tail_;

	int screen_Width_, screen_Heigh_;
};


void DownloadScene::initCaptured_screen()
{
	Screen_buffer * const e = screen_buffers_rb_ + RB_BUFFERS_SIZE;
	for (Screen_buffer *i = screen_buffers_rb_; i != e; ++i) 
	{
		glGenBuffers(1, &i->buffer_);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, i->buffer_);
		glBufferData(GL_PIXEL_PACK_BUFFER, screen_size, 0, GL_STREAM_READ);

		glGenBuffers(1, &i->tmp_buffer_);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, i->tmp_buffer_);
		glBufferData(GL_PIXEL_PACK_BUFFER, screen_size, 0, GL_STREAM_COPY);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		/*i->tmp_texture_ = base::create_texture(
			get_wnd_width(),
			get_wnd_height(),
			CAPTURE_PF,
			0);*/

		glGenQueries(3, i->queries_);

	}

	rb_head_=rb_tail_ = 0;

}


void DownloadScene::process_captured_screen()
{
	while (rb_tail_ != rb_head_) 
	{
		
		const int tmp_tail = (rb_tail_ + 1) &  (RB_BUFFERS_SIZE - 1);

		Screen_buffer *sc = screen_buffers_rb_ + tmp_tail;

		GLenum res = glClientWaitSync(sc->fence_, 0, 0);
		if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) { // AMD returns GL_CONDITION_SATISFIED only BUG?
			glDeleteSync(sc->fence_);

			glBindBuffer(GL_PIXEL_PACK_BUFFER, sc->buffer_);
			char *ptr_dst = reinterpret_cast<char*>(
				glMapBufferRange(
					GL_PIXEL_PACK_BUFFER,
					0,
					screen_size,
					GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));

			//memcpy(ptr_dst, ptr_src, screen_size);

			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);


			__int64 result[3] = { 0, };
			glGetQueryObjecti64v(sc->queries_[2], GL_QUERY_RESULT, result + 2);
			glGetQueryObjecti64v(sc->queries_[1], GL_QUERY_RESULT, result + 1);
			glGetQueryObjecti64v(sc->queries_[0], GL_QUERY_RESULT, result);
			const double coef_n2m = 1.0 / 1000000.0;
			const double time0 = double(result[1] - result[0]) * coef_n2m;
			const double time1 = double(result[2] - result[1]) * coef_n2m;
			static int counter = 0;

			/*if ((counter++ % 64) == 0) {
				printf("read %.2f ms / %.2f GB/s copy: %.2f ms / %.2f GB/s lag: %u  map: %.2f ms / %.2f GB/s\n",
					time0,
					(double(screen_size) / time0) * (1.0 / double(0x100000)),
					time1,
					time1 > 0.01 ? (double(screen_size) / time1) * (1.0 / double(0x100000)) : 0.0,
					_frame_number - sc->_frame_num,
					map_time,
					map_time > 0.01 ? (double(screen_size) / map_time) * (1.0 / double(0x100000)) : 0.0);
			}
*/
			rb_tail_ = tmp_tail;
		}
		else {
			break;
		}
	}

}


void DownloadScene::captured_screen()
{
	
	const int tmp_head = (rb_head_ + 1) & (RB_BUFFERS_SIZE - 1);

	if (tmp_head == rb_tail_) {
		printf("we are too fast there is no free screen buffer!\n");
	}
	else 
	{
		Screen_buffer *sc = screen_buffers_rb_ + tmp_head;

		glReadBuffer(GL_BACK);

		glBindTexture(GL_TEXTURE_2D, sc->tmp_texture_);
		glCopyTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0, 0,
			0, 0,
			screen_Width_, screen_Heigh_);
		glBindTexture(GL_TEXTURE_2D, 0);

		glQueryCounter(sc->queries_[0], GL_TIMESTAMP);

		glBindBuffer(
			GL_PIXEL_PACK_BUFFER,
			sc->buffer_);
		glReadPixels(
			0,
			0,
			screen_Width_,
			screen_Heigh_, 
			format_,
			type_,
			0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		glQueryCounter(sc->queries_[1], GL_TIMESTAMP);

		glQueryCounter(sc->queries_[2], GL_TIMESTAMP);

		sc->fence_ = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		// update ring buffer head
		rb_head_ = tmp_head;
	}
}

int main()
{
	 
	return 0;
}
