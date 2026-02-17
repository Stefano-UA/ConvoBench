# Image Convolution Benchmark

Implementation of a convolution algorithm in C++ with the intention of using it as a simple means to benchmark CPU performance.

### Index

- [Box Blur](#kernel-Box Blur)
- [Emboss](#kernel-Emboss)
- [Gaussian](#kernel-Gaussian)
- [Ghost](#kernel-Ghost)
- [Glitch X](#kernel-Glitch X)
- [Glitch Y](#kernel-Glitch Y)
- [Identity](#kernel-Identity)
- [Laplacian 3](#kernel-Laplacian 3)
- [Laplacian 5](#kernel-Laplacian 5)
- [Motion Blur X](#kernel-Motion Blur X)
- [Motion Blur Y](#kernel-Motion Blur Y)
- [Sharpen 3](#kernel-Sharpen 3)
- [Sharpen 5](#kernel-Sharpen 5)
- [Sobel X](#kernel-Sobel X)
- [Sobel Y](#kernel-Sobel Y)

## Kernel: box_blur

| Original | Processed (box_blur) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/box_blur/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/box_blur/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/box_blur/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/box_blur/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/box_blur/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/box_blur/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/box_blur/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/box_blur/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/box_blur/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/box_blur/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/box_blur/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/box_blur/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/box_blur/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/box_blur/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/box_blur/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/box_blur/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/box_blur/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/box_blur/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/box_blur/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/box_blur/windows11_background.jpg" width="300" /> |

## Kernel: emboss

| Original | Processed (emboss) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/emboss/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/emboss/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/emboss/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/emboss/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/emboss/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/emboss/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/emboss/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/emboss/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/emboss/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/emboss/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/emboss/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/emboss/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/emboss/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/emboss/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/emboss/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/emboss/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/emboss/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/emboss/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/emboss/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/emboss/windows11_background.jpg" width="300" /> |

## Kernel: gaussian

| Original | Processed (gaussian) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/gaussian/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/gaussian/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/gaussian/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/gaussian/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/gaussian/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/gaussian/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/gaussian/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/gaussian/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/gaussian/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/gaussian/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/gaussian/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/gaussian/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/gaussian/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/gaussian/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/gaussian/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/gaussian/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/gaussian/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/gaussian/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/gaussian/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/gaussian/windows11_background.jpg" width="300" /> |

## Kernel: ghost

| Original | Processed (ghost) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/ghost/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/ghost/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/ghost/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/ghost/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/ghost/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/ghost/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/ghost/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/ghost/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/ghost/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/ghost/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/ghost/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/ghost/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/ghost/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/ghost/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/ghost/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/ghost/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/ghost/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/ghost/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/ghost/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/ghost/windows11_background.jpg" width="300" /> |

## Kernel: glitch_x

| Original | Processed (glitch_x) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/glitch_x/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/glitch_x/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/glitch_x/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/glitch_x/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/glitch_x/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/glitch_x/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/glitch_x/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/glitch_x/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/glitch_x/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/glitch_x/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/glitch_x/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/glitch_x/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/glitch_x/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/glitch_x/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/glitch_x/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/glitch_x/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/glitch_x/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/glitch_x/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/glitch_x/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/glitch_x/windows11_background.jpg" width="300" /> |

## Kernel: glitch_y

| Original | Processed (glitch_y) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/glitch_y/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/glitch_y/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/glitch_y/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/glitch_y/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/glitch_y/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/glitch_y/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/glitch_y/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/glitch_y/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/glitch_y/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/glitch_y/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/glitch_y/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/glitch_y/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/glitch_y/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/glitch_y/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/glitch_y/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/glitch_y/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/glitch_y/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/glitch_y/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/glitch_y/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/glitch_y/windows11_background.jpg" width="300" /> |

## Kernel: identity

| Original | Processed (identity) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/identity/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/identity/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/identity/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/identity/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/identity/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/identity/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/identity/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/identity/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/identity/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/identity/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/identity/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/identity/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/identity/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/identity/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/identity/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/identity/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/identity/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/identity/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/identity/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/identity/windows11_background.jpg" width="300" /> |

## Kernel: laplacian_3

| Original | Processed (laplacian_3) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/laplacian_3/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/laplacian_3/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/laplacian_3/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/laplacian_3/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/laplacian_3/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/laplacian_3/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/laplacian_3/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/laplacian_3/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/laplacian_3/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/laplacian_3/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/laplacian_3/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/laplacian_3/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/laplacian_3/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/laplacian_3/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/laplacian_3/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/laplacian_3/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/laplacian_3/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/laplacian_3/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/laplacian_3/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/laplacian_3/windows11_background.jpg" width="300" /> |

## Kernel: laplacian_5

| Original | Processed (laplacian_5) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/laplacian_5/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/laplacian_5/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/laplacian_5/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/laplacian_5/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/laplacian_5/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/laplacian_5/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/laplacian_5/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/laplacian_5/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/laplacian_5/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/laplacian_5/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/laplacian_5/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/laplacian_5/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/laplacian_5/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/laplacian_5/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/laplacian_5/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/laplacian_5/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/laplacian_5/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/laplacian_5/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/laplacian_5/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/laplacian_5/windows11_background.jpg" width="300" /> |

## Kernel: motion_blur_x

| Original | Processed (motion_blur_x) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/motion_blur_x/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/motion_blur_x/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/motion_blur_x/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/motion_blur_x/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/motion_blur_x/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/motion_blur_x/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/motion_blur_x/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/motion_blur_x/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/motion_blur_x/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/motion_blur_x/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/motion_blur_x/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/motion_blur_x/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/motion_blur_x/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/motion_blur_x/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/motion_blur_x/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/motion_blur_x/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/motion_blur_x/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/motion_blur_x/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/motion_blur_x/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/motion_blur_x/windows11_background.jpg" width="300" /> |

## Kernel: motion_blur_y

| Original | Processed (motion_blur_y) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/motion_blur_y/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/motion_blur_y/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/motion_blur_y/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/motion_blur_y/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/motion_blur_y/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/motion_blur_y/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/motion_blur_y/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/motion_blur_y/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/motion_blur_y/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/motion_blur_y/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/motion_blur_y/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/motion_blur_y/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/motion_blur_y/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/motion_blur_y/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/motion_blur_y/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/motion_blur_y/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/motion_blur_y/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/motion_blur_y/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/motion_blur_y/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/motion_blur_y/windows11_background.jpg" width="300" /> |

## Kernel: sharpen_3

| Original | Processed (sharpen_3) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/sharpen_3/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/sharpen_3/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/sharpen_3/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/sharpen_3/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/sharpen_3/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/sharpen_3/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/sharpen_3/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/sharpen_3/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/sharpen_3/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/sharpen_3/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/sharpen_3/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/sharpen_3/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/sharpen_3/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/sharpen_3/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/sharpen_3/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/sharpen_3/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/sharpen_3/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/sharpen_3/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/sharpen_3/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/sharpen_3/windows11_background.jpg" width="300" /> |

## Kernel: sharpen_5

| Original | Processed (sharpen_5) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/sharpen_5/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/sharpen_5/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/sharpen_5/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/sharpen_5/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/sharpen_5/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/sharpen_5/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/sharpen_5/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/sharpen_5/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/sharpen_5/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/sharpen_5/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/sharpen_5/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/sharpen_5/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/sharpen_5/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/sharpen_5/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/sharpen_5/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/sharpen_5/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/sharpen_5/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/sharpen_5/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/sharpen_5/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/sharpen_5/windows11_background.jpg" width="300" /> |

## Kernel: sobel_x

| Original | Processed (sobel_x) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/sobel_x/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/sobel_x/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/sobel_x/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/sobel_x/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/sobel_x/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/sobel_x/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/sobel_x/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/sobel_x/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/sobel_x/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/sobel_x/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/sobel_x/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/sobel_x/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/sobel_x/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/sobel_x/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/sobel_x/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/sobel_x/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/sobel_x/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/sobel_x/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/sobel_x/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/sobel_x/windows11_background.jpg" width="300" /> |

## Kernel: sobel_y

| Original | Processed (sobel_y) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="300" /> | <img src="./output/sobel_y/abstract_shape.jpg" width="300" /> |
| <img src="./input/arrows_up.jpg" width="300" /> | <img src="./output/sobel_y/arrows_up.jpg" width="300" /> |
| <img src="./input/building_windows.jpg" width="300" /> | <img src="./output/sobel_y/building_windows.jpg" width="300" /> |
| <img src="./input/cyan_fractal.jpg" width="300" /> | <img src="./output/sobel_y/cyan_fractal.jpg" width="300" /> |
| <img src="./input/geometric_shapes.jpg" width="300" /> | <img src="./output/sobel_y/geometric_shapes.jpg" width="300" /> |
| <img src="./input/grey_fractal.jpg" width="300" /> | <img src="./output/sobel_y/grey_fractal.jpg" width="300" /> |
| <img src="./input/lone_coffe.jpg" width="300" /> | <img src="./output/sobel_y/lone_coffe.jpg" width="300" /> |
| <img src="./input/lone_lamp.jpg" width="300" /> | <img src="./output/sobel_y/lone_lamp.jpg" width="300" /> |
| <img src="./input/lone_leaf.jpg" width="300" /> | <img src="./output/sobel_y/lone_leaf.jpg" width="300" /> |
| <img src="./input/milky_way.jpg" width="300" /> | <img src="./output/sobel_y/milky_way.jpg" width="300" /> |
| <img src="./input/only_blue.jpg" width="300" /> | <img src="./output/sobel_y/only_blue.jpg" width="300" /> |
| <img src="./input/pedestrian_crossing.jpg" width="300" /> | <img src="./output/sobel_y/pedestrian_crossing.jpg" width="300" /> |
| <img src="./input/person.jpg" width="300" /> | <img src="./output/sobel_y/person.jpg" width="300" /> |
| <img src="./input/romanesco.jpg" width="300" /> | <img src="./output/sobel_y/romanesco.jpg" width="300" /> |
| <img src="./input/small_keyboard.jpg" width="300" /> | <img src="./output/sobel_y/small_keyboard.jpg" width="300" /> |
| <img src="./input/strange_sphere.jpg" width="300" /> | <img src="./output/sobel_y/strange_sphere.jpg" width="300" /> |
| <img src="./input/teal_fractal.jpg" width="300" /> | <img src="./output/sobel_y/teal_fractal.jpg" width="300" /> |
| <img src="./input/voxel_cube.jpg" width="300" /> | <img src="./output/sobel_y/voxel_cube.jpg" width="300" /> |
| <img src="./input/wavy_background.jpg" width="300" /> | <img src="./output/sobel_y/wavy_background.jpg" width="300" /> |
| <img src="./input/windows11_background.jpg" width="300" /> | <img src="./output/sobel_y/windows11_background.jpg" width="300" /> |

---
*Readme generated on mar 17 feb 2026 23:17:09 CET*
