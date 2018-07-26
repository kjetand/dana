#include <gtest/gtest.h>

#include <dana/canvas.h>
#include <dana/pencil.h>

using namespace dana;

TEST(PencilTest, transform) {
  const TransformMatrix expected{1, 2, 3, 4, 5, 6};

  // Initialize OpenGL by instantiating a canvas
  Canvas canvas(100, 100, "Pencil test");
  Pencil pencil;

  pencil.transform(1, 2, 3, 4, 5, 6);

  const auto actual{pencil.getCurrentTransform()};

  ASSERT_EQ(actual.horizontal_scaling, expected.horizontal_scaling);
  ASSERT_EQ(actual.horizontal_skewing, expected.horizontal_skewing);
  ASSERT_EQ(actual.vertical_skewing, expected.vertical_skewing);
  ASSERT_EQ(actual.vertical_scaling, expected.vertical_scaling);
  ASSERT_EQ(actual.horizontal_moving, expected.horizontal_moving);
  ASSERT_EQ(actual.vertical_moving, expected.vertical_moving);
}
