<div align="center">

### Compile & run

<pre>
git clone https://github.com/x3ric/grafenic
cd grafenic
./build run
</pre>

### Examples

**main** -> ./src/main.c (default when ./build run)

**gamepad** -> ./src/gamepad.c (when ./build run gamepad)

**image** -> ./src/image.c (when ./build run image)

**cursor** -> ./src/cursor.c (when ./build run cursor)

**pong** -> ./src/pong.c (when ./build run pong)

**3d** -> ./src/3d.c (when ./build run 3d)

<details>
<summary><h3>Variables</h3></summary>

**window.width:** Initial width of the window (output)

**window.height:** Initial height of the window (output)

**window.screen_width:** Current width of the screen (output)

**window.screen_height:** Current height of the screen (output)

**window.title:** Window title (input/output)

**window.samples:** Aliasing (input/output)

**window.refresh_rate:** Refresh rate (input/output)

**window.floating:** Window is floating (false by !default)

**window.fullscreen:** Window is fullscreen (true by !default)

**window.vsync:** Vertical sync enabled (true by !default)

**window.hidecursor:** Cursor visibility (true by !default)

**window.transparent:** Transparency enabled (true by !default)

**window.decorated:** Window decoration (false by !default)

**window.hided:** Window visibility (true by !default)

**window.fpslimit:** FPS limit (60 by !default)

**window.fps:** Frames per second (output)

**window.deltatime:** Delta time (output)

**mouse:** Mouse position (x, y) (output)

**mouse.scroll:** Mouse scroll (x, y) (output)

**mouse.moving:** Mouse movement (output)

**window.debug.input:** Debug input (true by !default)

**window.debug.wireframe:** Debug wireframe (true by !default)

**window.debug.fps:** Debug FPS (true by !default)

</details>

<details>
<summary><h3>Colors</h3></summary>
<table>
<tr><td><img src="https://via.placeholder.com/50x30/C8C8C8"></td><td>LIGHTGRAY (Color){ 200, 200, 200}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/828282"></td><td>GRAY (Color){ 130, 130, 130}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/505050"></td><td>DARKGRAY (Color){ 80, 80, 80}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/FDF900"></td><td>YELLOW (Color){ 253, 249, 0}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/FFCB00"></td><td>GOLD (Color){ 255, 203, 0}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/FFA100"></td><td>ORANGE (Color){ 255, 161, 0}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/FF6DC2"></td><td>PINK (Color){ 255, 109, 194}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/E62937"></td><td>RED (Color){ 230, 41, 55}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/BE2137"></td><td>MAROON (Color){ 190, 33, 55}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/00E430"></td><td>GREEN (Color){ 0, 228, 48}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/009E2F"></td><td>LIME (Color){ 0, 158, 47}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/00752C"></td><td>DARKGREEN (Color){ 0, 117, 44}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/66BFFF"></td><td>SKYBLUE (Color){ 102, 191, 255}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/0079F1"></td><td>BLUE (Color){ 0, 121, 241}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/0052AC"></td><td>DARKBLUE (Color){ 0, 82, 172}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/C87AFF"></td><td>PURPLE (Color){ 200, 122, 255}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/873CBE"></td><td>VIOLET (Color){ 135, 60, 190}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/701F7E"></td><td>DARKPURPLE (Color){ 112, 31, 126}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/D3B083"></td><td>BEIGE (Color){ 211, 176, 131}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/7F6A4F"></td><td>BROWN (Color){ 127, 106, 79}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/4C3F2F"></td><td>DARKBROWN (Color){ 76, 63, 47}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/FFFFFF"></td><td>WHITE (Color){ 255, 255, 255}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/000000"></td><td>BLACK (Color){ 0, 0, 0}</td></tr>
<tr><td><img src="https://via.placeholder.com/50x30/FF00FF"></td><td>MAGENTA (Color){ 255, 0, 255}</td></tr>
<tr><td></td><td>BLANK (Color){ 0, 0, 0, 0}</td></tr>
</table>
</details>

<details>
<summary><h3>Todo</h3></summary>

**Collision Detection**: Implement collision detection for 3D objects.

**3D Model Importer**: Develop functionality to import various 3D model formats.

**Cubemap**: Add support for cubemap textures for environment mapping.

**Lights & Shadows**: Implement dynamic lighting and shadow effects.

**Camera Rotation on 3D Objects**: Enable camera rotation functionality for 3D objects, such as cubes.

**Shadows**: Implement various shadow techniques like shadow mapping or shadow volumes.

**Skeletal Animations**: Implement skeletal animations for character movement and deformation.

**Geometry Culling**: Optimize rendering by excluding objects not in the camera’s view.

**Level of Detail (LOD)**: Implement LOD to adjust model detail based on distance from the camera.

**Physically-Based Rendering (PBR)**: Use PBR to simulate realistic materials with properties like albedo, roughness, and metalness.

**Normal Mapping**: Enhance surface detail by using normal maps to simulate complex textures.

**Image-Based Lighting (IBL)**: Use precomputed environment maps for realistic lighting and reflections.

**Global Illumination**: Implement techniques like radiosity or path tracing for realistic indirect lighting.

**Spherical Harmonics**: Use spherical harmonics to represent and compute complex lighting environments.

**Surface Scattering**: Simulate light scattering beneath surfaces for materials like skin and wax.

**Volumetric Rendering**: Render effects like fog, smoke, or clouds with techniques such as ray marching.

**Parallax Mapping**: Simulate depth and surface detail by adjusting texture coordinates based on viewing angles.

**Reflection**: Implement various reflection techniques such as cube maps or screen-space reflections.

**Refraction**: Simulate light bending as it passes through transparent materials.

**Diffraction**: Simulate the bending of light around edges or through small openings.

**Ambient Occlusion**: Implement ambient occlusion to add depth and realism by darkening crevices and occluded areas.

**Bindless Textures**: Use bindless texture handles to efficiently manage and access a large number of textures in shaders.

</details>

</p><a href="https://archlinux.org"><img alt="Arch Linux" src="https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=arch-linux&logoColor=D9E0EE&color=000000&labelColor=97A4E2"/></a><br>
