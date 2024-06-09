J3D is a 3D graphics module for JUCE. It enables you to make a variety of 3D Graphics systems without having to directly interact with OpenGL (but you can if you want to). The aim is for it to work similarly to how browser-oriented 3D libraries like `three.js` work, where objects are held in hierarchical systems
## 3D Transforms
- At the base layer we need a class which just acts as a 3d transform. This class would contain a position vec3, a scale vec3, and a rotation quaternion.
- This class would have a bunch of functionality so that 3d math can be done easily (setting & adding rotation, Euler based rotation, `lookAt` functions, etc)
## 3D Objects
- 3D objects contain a 3D transform, a 3D material, a mesh, and a vector of 3D object children.
	- Rendering of the 3D object would be a rendering of the top level object (if a mesh exists) and then a traversal of the 3D object children, and those children's children (similarly to how JUCE components work)
- This is necessary for implementing things like object grouping. 
	- One instance would be when I want to implement a 3d drag gizmo (like what you see in various 3D editors), where the "top level" 3D object doesn't actually hold geometry but just holds geometry in the children (for each arrow, in this case).
	- The children of 3d objects inherit the 3D transforms and add onto it, so moving a top-level 3D object will have it's children adjusted relative to it (including positioning, scaling, and rotation)
## Meshes
- Mesh objects will hold OpenGL IDs for vertex / normal / uv / index arrays, and will be able to only work with a certain selection of these if needed. 
	- The actual internal vectors will not be stored in these meshes, and should be held externally. This is because holding local copies of all mesh data outside of OpenGL will consume a lot of memory for not much benefit in most cases
- Additionally, the mesh can be specified to render in a specific way aside from the conventional method of just drawing triangles (like using `GL_POINTS`, or `GL_LINES`).
- Meshes will generally be loaded by using an object importer (either assimp or a basic homemade .obj importer) or by using a geometry generator;
### Geometry Generators
- I already have a bunch of geometry generator functions used in Impulse, I can probably just write more as we need them (cube, torus, sphere, icosahedron, etc). There's a bunch of parameters that each function has so you can get specific with how the geometries need to look (mainly in terms of resolution / subdivision count, etc)
## Cameras
- A camera is just a 3D transform with a pre-specified projection matrix. This class would be highly abstract, and inheriting classes would implement specific types of cameras (like perspective & orthographic, maybe others).
- Here's an example of a base camera class, and a perspective camera inheriting it:
``` cpp
class J3DCamera: public J3DTransform {
  public: Camera();

  virtual
  const void render(juce::OpenGLShaderProgram * ) = 0; // sets the shader to be synced with the camera's transform

  virtual glm::mat4 getProjectionMatrix() = 0;

  glm::mat4 getViewMatrix() {
    return glm::lookAt(position, position + direction, up);
  }

  private: float nearPlane = 0.05 f;
  float farPlane = 100.0 f;

  float aspectRatio = 1.0 f;
}

class PerspectiveCamera {
  public: PerspectiveCamera();

  float fov = 90.0 f; // value is in degrees

  const void render(juce::OpenGLShaderProgram * currentProgram) {
    auto camera = getProjectionMatrix();
    auto view = getViewMatrix();

    currentProgram -> setUniformMat4("cameraProjection", & camera[0][0], 1, GL_FALSE);
    currentProgram -> setUniformMat4("cameraView", & view[0][0], 1, GL_FALSE);

    currentProgram -> setUniform("cameraPosition", position.x, position.y, position.z);
    currentProgram -> setUniform("cameraDirection", direction.x, direction.y, direction.z);
  }

  glm::mat4 getProjectionMatrix() override {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
  }
}
```
- These classes also need functionality for handling projection & un-projection of points to / from 3D / 2D space. I have already written this for Impulse, so we have it done.
## J3DComponent
- Ideally the main "viewport" would just be something that you would override and add 3D objects to similarly to how you can just inherit a `juce::Component` and add components to it. Here's a demo:
``` cpp
class 3DDemo : public J3DComponent
{
	public:
	3DDemo()
	{
		setCamera(camera_);

		add3DObject(cube_); // transform position is at {0, 0, 0} by default

		camera_.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
		camera_.lookAt(cube_);
	}

	private:
	PerspectiveCamera camera_; // inherits the Camera class
	Cube3D cube_; // inherits the 3D object class
}
```
- This will probably be written on top of a JUCE component to begin with (things like resizing and boundaries will need to be accessed).
- This class will probably need a camera to render anything to begin with so maybe we can have a default one?
## Layers
- I would like to implement a layer system somehow so that *maybe* certain 3d objects added to a J3DComponent would be given a different order in which to render (so something could always be in front of other stuff, or always behind, etc). 
- At the class level each layer might be a vector of 3d scenes that all exist inside of a J3D component (so you could have one layer that's always on top and would be a 3d arrow, and the next layer would be an imported mesh). 
- Additionally, given that this will be done in OpenGL and fully capable, we could also implement layer blending settings, similarly to how image editing software allows layers to be differently blended (i.e multiply, add, etc). 
	- We could even make use of OpenGL's stenciling capabilities to make it so that certain layers can cause other layers to only selectively render where elements on a different layer are / are not.
## OpenGL Context Manager
- In JUCE, the OpenGL context is designed for being attached to one `OpenGLRenderer` at a time. The workaround for this is that we have a renderer at a high level that handles all components being rendered in a single frame, taking into account the necessary rendering viewport changes relative to the top level component. I have written something for Impulse that already does this, so we can use it & build off of it.
