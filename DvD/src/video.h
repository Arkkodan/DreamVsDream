#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

class Video {
public:
	double frameTimer;

	void think();
	void draw(int x, int y);
	void reset();

	Video(avi_t* avi, char compressor);
	~Video();

	static Video* create(const char* filename);
	//static Video* createFromMemory(const char* data);

private:
	avi_t* avi;

	int tw, th;

	unsigned int texture;
	unsigned char* texBuff;

	char compressor;
};

#endif // VIDEO_H_INCLUDED
