#ifndef DVD_ANIMATION_H
#define DVD_ANIMATION_H

#include "image.h"

#include <string>
#include <vector>

/// @brief Animation class represented by a sequence of images
/// @details Based on gif
class Animation {
public:
  Animation();
  Animation(Animation &&other) noexcept;
  Animation &operator=(Animation &&other) noexcept;
  ~Animation();

  Animation(const Animation &other) = delete;
  Animation &operator=(const Animation &other) = delete;

  // Load a GIF
  Animation(const std::string &filename);

  // Change/get status of animation
  void setPlaying(bool playing);
  bool isPlaying() const;

  void draw(int x, int y) const;

private:
  int nFrames;
  std::vector<Image> frames;
  std::vector<int> frameTimes;

  bool playing;
  unsigned int startFrame;
};

#endif // DVD_ANIMATION_H
