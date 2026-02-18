# Image Convolution Benchmark

Implementation of a convolution algorithm in C++ with the intention of using it as a simple means to benchmark CPU performance.

### Index

- [Box Blur](#kernel-box_blur)
- [Emboss](#kernel-emboss)
- [Gaussian](#kernel-gaussian)
- [Ghost](#kernel-ghost)
- [Glitch X](#kernel-glitch_x)
- [Glitch Y](#kernel-glitch_y)
- [Identity](#kernel-identity)
- [Laplacian 3](#kernel-laplacian_3)
- [Laplacian 5](#kernel-laplacian_5)
- [Motion Blur X](#kernel-motion_blur_x)
- [Motion Blur Y](#kernel-motion_blur_y)
- [Sharpen 3](#kernel-sharpen_3)
- [Sharpen 5](#kernel-sharpen_5)
- [Sobel X](#kernel-sobel_x)
- [Sobel Y](#kernel-sobel_y)

<div id="kernel-box_blur"></div>
<details>
<summary><strong>Kernel: box_blur</strong></summary>

| Original | Processed (box_blur) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/box_blur/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/box_blur/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/box_blur/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/box_blur/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/box_blur/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/box_blur/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/box_blur/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/box_blur/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/box_blur/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/box_blur/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/box_blur/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/box_blur/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/box_blur/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/box_blur/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/box_blur/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/box_blur/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/box_blur/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/box_blur/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/box_blur/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/box_blur/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-emboss"></div>
<details>
<summary><strong>Kernel: emboss</strong></summary>

| Original | Processed (emboss) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/emboss/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/emboss/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/emboss/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/emboss/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/emboss/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/emboss/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/emboss/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/emboss/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/emboss/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/emboss/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/emboss/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/emboss/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/emboss/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/emboss/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/emboss/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/emboss/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/emboss/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/emboss/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/emboss/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/emboss/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-gaussian"></div>
<details>
<summary><strong>Kernel: gaussian</strong></summary>

| Original | Processed (gaussian) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/gaussian/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/gaussian/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/gaussian/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/gaussian/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/gaussian/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/gaussian/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/gaussian/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/gaussian/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/gaussian/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/gaussian/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/gaussian/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/gaussian/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/gaussian/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/gaussian/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/gaussian/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/gaussian/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/gaussian/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/gaussian/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/gaussian/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/gaussian/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-ghost"></div>
<details>
<summary><strong>Kernel: ghost</strong></summary>

| Original | Processed (ghost) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/ghost/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/ghost/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/ghost/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/ghost/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/ghost/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/ghost/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/ghost/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/ghost/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/ghost/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/ghost/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/ghost/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/ghost/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/ghost/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/ghost/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/ghost/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/ghost/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/ghost/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/ghost/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/ghost/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/ghost/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-glitch_x"></div>
<details>
<summary><strong>Kernel: glitch_x</strong></summary>

| Original | Processed (glitch_x) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/glitch_x/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/glitch_x/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/glitch_x/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/glitch_x/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/glitch_x/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/glitch_x/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/glitch_x/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/glitch_x/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/glitch_x/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/glitch_x/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/glitch_x/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/glitch_x/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/glitch_x/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/glitch_x/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/glitch_x/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/glitch_x/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/glitch_x/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/glitch_x/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/glitch_x/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/glitch_x/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-glitch_y"></div>
<details>
<summary><strong>Kernel: glitch_y</strong></summary>

| Original | Processed (glitch_y) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/glitch_y/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/glitch_y/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/glitch_y/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/glitch_y/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/glitch_y/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/glitch_y/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/glitch_y/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/glitch_y/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/glitch_y/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/glitch_y/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/glitch_y/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/glitch_y/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/glitch_y/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/glitch_y/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/glitch_y/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/glitch_y/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/glitch_y/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/glitch_y/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/glitch_y/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/glitch_y/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-identity"></div>
<details>
<summary><strong>Kernel: identity</strong></summary>

| Original | Processed (identity) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/identity/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/identity/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/identity/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/identity/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/identity/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/identity/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/identity/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/identity/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/identity/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/identity/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/identity/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/identity/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/identity/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/identity/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/identity/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/identity/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/identity/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/identity/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/identity/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/identity/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-laplacian_3"></div>
<details>
<summary><strong>Kernel: laplacian_3</strong></summary>

| Original | Processed (laplacian_3) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/laplacian_3/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/laplacian_3/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/laplacian_3/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/laplacian_3/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/laplacian_3/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/laplacian_3/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/laplacian_3/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/laplacian_3/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/laplacian_3/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/laplacian_3/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/laplacian_3/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/laplacian_3/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/laplacian_3/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/laplacian_3/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/laplacian_3/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/laplacian_3/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/laplacian_3/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/laplacian_3/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/laplacian_3/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/laplacian_3/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-laplacian_5"></div>
<details>
<summary><strong>Kernel: laplacian_5</strong></summary>

| Original | Processed (laplacian_5) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/laplacian_5/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/laplacian_5/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/laplacian_5/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/laplacian_5/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/laplacian_5/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/laplacian_5/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/laplacian_5/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/laplacian_5/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/laplacian_5/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/laplacian_5/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/laplacian_5/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/laplacian_5/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/laplacian_5/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/laplacian_5/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/laplacian_5/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/laplacian_5/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/laplacian_5/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/laplacian_5/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/laplacian_5/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/laplacian_5/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-motion_blur_x"></div>
<details>
<summary><strong>Kernel: motion_blur_x</strong></summary>

| Original | Processed (motion_blur_x) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/motion_blur_x/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/motion_blur_x/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/motion_blur_x/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/motion_blur_x/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/motion_blur_x/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/motion_blur_x/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/motion_blur_x/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/motion_blur_x/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/motion_blur_x/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/motion_blur_x/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/motion_blur_x/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/motion_blur_x/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/motion_blur_x/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/motion_blur_x/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/motion_blur_x/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/motion_blur_x/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/motion_blur_x/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/motion_blur_x/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/motion_blur_x/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/motion_blur_x/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-motion_blur_y"></div>
<details>
<summary><strong>Kernel: motion_blur_y</strong></summary>

| Original | Processed (motion_blur_y) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/motion_blur_y/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/motion_blur_y/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/motion_blur_y/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/motion_blur_y/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/motion_blur_y/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/motion_blur_y/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/motion_blur_y/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/motion_blur_y/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/motion_blur_y/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/motion_blur_y/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/motion_blur_y/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/motion_blur_y/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/motion_blur_y/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/motion_blur_y/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/motion_blur_y/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/motion_blur_y/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/motion_blur_y/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/motion_blur_y/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/motion_blur_y/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/motion_blur_y/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-sharpen_3"></div>
<details>
<summary><strong>Kernel: sharpen_3</strong></summary>

| Original | Processed (sharpen_3) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/sharpen_3/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/sharpen_3/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/sharpen_3/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/sharpen_3/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/sharpen_3/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/sharpen_3/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/sharpen_3/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/sharpen_3/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/sharpen_3/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/sharpen_3/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/sharpen_3/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/sharpen_3/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/sharpen_3/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/sharpen_3/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/sharpen_3/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/sharpen_3/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/sharpen_3/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/sharpen_3/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/sharpen_3/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/sharpen_3/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-sharpen_5"></div>
<details>
<summary><strong>Kernel: sharpen_5</strong></summary>

| Original | Processed (sharpen_5) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/sharpen_5/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/sharpen_5/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/sharpen_5/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/sharpen_5/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/sharpen_5/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/sharpen_5/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/sharpen_5/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/sharpen_5/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/sharpen_5/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/sharpen_5/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/sharpen_5/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/sharpen_5/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/sharpen_5/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/sharpen_5/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/sharpen_5/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/sharpen_5/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/sharpen_5/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/sharpen_5/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/sharpen_5/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/sharpen_5/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-sobel_x"></div>
<details>
<summary><strong>Kernel: sobel_x</strong></summary>

| Original | Processed (sobel_x) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/sobel_x/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/sobel_x/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/sobel_x/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/sobel_x/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/sobel_x/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/sobel_x/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/sobel_x/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/sobel_x/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/sobel_x/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/sobel_x/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/sobel_x/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/sobel_x/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/sobel_x/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/sobel_x/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/sobel_x/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/sobel_x/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/sobel_x/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/sobel_x/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/sobel_x/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/sobel_x/windows11_background.png" width="100%" /> |
</details>

<div id="kernel-sobel_y"></div>
<details>
<summary><strong>Kernel: sobel_y</strong></summary>

| Original | Processed (sobel_y) |
| :------: | :-------: |
| <img src="./input/abstract_shape.jpg" width="100%" /> | <img src="./output/sobel_y/abstract_shape.png" width="100%" /> |
| <img src="./input/arrows_up.jpg" width="100%" /> | <img src="./output/sobel_y/arrows_up.png" width="100%" /> |
| <img src="./input/building_windows.jpg" width="100%" /> | <img src="./output/sobel_y/building_windows.png" width="100%" /> |
| <img src="./input/cyan_fractal.jpg" width="100%" /> | <img src="./output/sobel_y/cyan_fractal.png" width="100%" /> |
| <img src="./input/geometric_shapes.jpg" width="100%" /> | <img src="./output/sobel_y/geometric_shapes.png" width="100%" /> |
| <img src="./input/grey_fractal.jpg" width="100%" /> | <img src="./output/sobel_y/grey_fractal.png" width="100%" /> |
| <img src="./input/lone_coffe.jpg" width="100%" /> | <img src="./output/sobel_y/lone_coffe.png" width="100%" /> |
| <img src="./input/lone_lamp.jpg" width="100%" /> | <img src="./output/sobel_y/lone_lamp.png" width="100%" /> |
| <img src="./input/lone_leaf.jpg" width="100%" /> | <img src="./output/sobel_y/lone_leaf.png" width="100%" /> |
| <img src="./input/milky_way.jpg" width="100%" /> | <img src="./output/sobel_y/milky_way.png" width="100%" /> |
| <img src="./input/only_blue.jpg" width="100%" /> | <img src="./output/sobel_y/only_blue.png" width="100%" /> |
| <img src="./input/pedestrian_crossing.jpg" width="100%" /> | <img src="./output/sobel_y/pedestrian_crossing.png" width="100%" /> |
| <img src="./input/person.jpg" width="100%" /> | <img src="./output/sobel_y/person.png" width="100%" /> |
| <img src="./input/romanesco.jpg" width="100%" /> | <img src="./output/sobel_y/romanesco.png" width="100%" /> |
| <img src="./input/small_keyboard.jpg" width="100%" /> | <img src="./output/sobel_y/small_keyboard.png" width="100%" /> |
| <img src="./input/strange_sphere.jpg" width="100%" /> | <img src="./output/sobel_y/strange_sphere.png" width="100%" /> |
| <img src="./input/teal_fractal.jpg" width="100%" /> | <img src="./output/sobel_y/teal_fractal.png" width="100%" /> |
| <img src="./input/voxel_cube.jpg" width="100%" /> | <img src="./output/sobel_y/voxel_cube.png" width="100%" /> |
| <img src="./input/wavy_background.jpg" width="100%" /> | <img src="./output/sobel_y/wavy_background.png" width="100%" /> |
| <img src="./input/windows11_background.jpg" width="100%" /> | <img src="./output/sobel_y/windows11_background.png" width="100%" /> |
</details>

---
*Readme generated on mié 18 feb 2026 18:05:26 CET*
