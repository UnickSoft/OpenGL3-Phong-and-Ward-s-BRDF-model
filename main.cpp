/**
 *
 * This sample shows differences between Phong,
 * Ward's BRDF and Ward's BRDF with map for coefficients.
 *
 * This samples used: glew, glfw, oglplus, freeimage.
 *
 * Created by Oleg Shiakhatarov
 */


#define MAIN_CPP_FILE_PATH __FILE__

#include "BaseApplication.h"
#include <oglplus/shapes/sphere.hpp>
#undef GL_PRIMITIVE_RESTART
#include <oglplus/shapes/torus.hpp>
#include <oglplus/shapes/spiral_sphere.hpp>
#include "ShaderProgram.h"
#include "RenderObject.h"
#include <vector>
#include <oglplus/images/image.hpp>
#include <oglplus/images/load.ipp>
#include <FreeImage.h>
#include <functional>

class Application : public BaseApplication
{
public:
    
    ShaderProgram program;
    ShaderProgram programBRDF;
    ShaderProgram programBRDFMap;
    
    oglplus::Buffer shperePositions;
    oglplus::Buffer shpereNormals;
    oglplus::Buffer shpereTangents;
    oglplus::Buffer shpereTextureCoords;
    oglplus::Buffer shpereIndexes;
    oglplus::VertexArray sphere;
    int nShpereVertexCount;
    oglplus::shapes::Sphere make_sphere;
    
    
    oglplus::Buffer torusPositions;
    oglplus::Buffer torusNormals;
    oglplus::Buffer torusTangents;
    oglplus::Buffer torusTextureCoords;
    oglplus::Buffer torusIndexes;
    oglplus::VertexArray torus;
    int nTorusVertexCount;
    oglplus::shapes::Torus make_torus;

    std::vector<RenderObject> spheres;
    std::vector<RenderObject> toruses;
    
    std::vector<RenderObject>* activeObjects;
    
    oglplus::Vec4f            lightPosition;
    oglplus::Mat4f            cameraMatrix;
    unsigned long             prevTime;
    bool                      rotation;
    oglplus::Texture          mapTexture;
    
    Application() : BaseApplication (900, 300, "Phong + Ward BRDF + Ward BRDF with map : Press space to play/stop. Press 1 or 2 to change object"), program("vertex.vert", "fragmetPhong.frag"),
        programBRDF("vertex.vert", "fragmetWardBRDF.frag"), programBRDFMap("vertex.vert", "fragmetWardBRDFMap.frag"), make_torus(0.8, 0.5, 40, 40), make_sphere(1.0, 40.0, 40.0), lightPosition(2.3, 1.3, -7.0, 1.0), rotation(true)
    {
        using namespace oglplus;
        
        program.Use();
        
        int width = 0;
        int height = 0;
        GetWidnowSize(width, height);
        
        oglplus::Mat4f projection_matrix = oglplus::CamMatrixf::PerspectiveX(
                                                     Degrees(60),
                                                     double(width / 3)/height,
                                                     1, 10.0
                                                     );
        oglplus::Mat4f camera_matrix = oglplus::CamMatrixf::LookingAt (oglplus::Vec3f(0.0, 0.0, -2.5), oglplus::Vec3f());
        
        CreateSphere();
        CreateTorus();
        
        pGLContext->ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        pGLContext->ClearDepth(1.0f);
        pGLContext->Enable(Capability::DepthTest);

        oglplus::Vec3f ambient(0.329412,	0.223529,	0.027451);
        oglplus::Vec3f diffusion(0.780392,	0.568627,	0.113725);
        oglplus::Vec3f specular(0.992157,	0.941176,	0.807843);
        
        oglplus::Mat4f cameraMatrix = camera_matrix;
        oglplus::Mat4f sphereModelMatrix;
        
        // Create shapes.
        
        // Phong shading
        {
            RenderObject sphereObject(sphere, program, shpereIndexes);
            sphereObject.Init(PrimitiveType::TriangleStrip, nShpereVertexCount);
            
            SetupCommonParams(sphereObject, ambient,diffusion,specular, cameraMatrix, sphereModelMatrix, projection_matrix, lightPosition);
            sphereObject.AddFloatParam("shininess", 25.0);
            
            spheres.push_back(sphereObject);
        }
        
        // Ward's BRDF Model: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Brushed_Metal
        {
            RenderObject sphereObject(sphere, programBRDF, shpereIndexes);
            sphereObject.Init(PrimitiveType::TriangleStrip, nShpereVertexCount);
            oglplus::Mat4f modelMatrix;
            
            SetupCommonParams(sphereObject, ambient,diffusion,specular, cameraMatrix, sphereModelMatrix, projection_matrix, lightPosition);
            
            spheres.push_back(sphereObject);
        }
        
        // Ward's SBRDF Model + map
        {
            RenderObject sphereObject(sphere, programBRDFMap, shpereIndexes);
            sphereObject.Init(PrimitiveType::TriangleStrip, nShpereVertexCount);
            oglplus::Mat4f modelMatrix;
            
            SetupCommonParams(sphereObject, ambient,diffusion,specular, cameraMatrix, sphereModelMatrix, projection_matrix, lightPosition);
            
            spheres.push_back(sphereObject);
        }
        
        // Create torus
        oglplus::Mat4f torusModelMatrix = oglplus::ModelMatrixf::RotationX(oglplus::Degrees(90));
        
        // Phong shading
        {
            RenderObject torusObject(torus, program, torusIndexes);
            torusObject.Init(PrimitiveType::TriangleStrip, nTorusVertexCount);
            
            SetupCommonParams(torusObject, ambient, diffusion, specular, cameraMatrix, torusModelMatrix, projection_matrix, lightPosition);
            torusObject.AddFloatParam("shininess", 25.0);
            
            toruses.push_back(torusObject);
        }
        
        // Ward's BRDF Model: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Brushed_Metal
        {
            RenderObject torusObject(torus, programBRDF, torusIndexes);
            torusObject.Init(PrimitiveType::TriangleStrip, nTorusVertexCount);
            
            SetupCommonParams(torusObject, ambient, diffusion, specular, cameraMatrix, torusModelMatrix, projection_matrix, lightPosition);
            
            toruses.push_back(torusObject);
        }
        
        // Ward's SBRDF Model + map
        {
            RenderObject torusObject(torus, programBRDFMap, torusIndexes);
            torusObject.Init(PrimitiveType::TriangleStrip, nTorusVertexCount);
            
            SetupCommonParams(torusObject, ambient, diffusion, specular, cameraMatrix, torusModelMatrix, projection_matrix, lightPosition);
            
            toruses.push_back(torusObject);
        }
        
        //Render spheres by default.
        activeObjects = &spheres;
        
        LoadBRDFMap(Resource::getResoucesDir() + "LoadBRDFMap.png");
        
        programBRDFMap.Use();
        UniformSampler(programBRDFMap.GetProgram(), "MapTexture").Set(0);
        
        prevTime = GetMsFromEpoch();
    }
    
    
    void Render()
    {
        using namespace oglplus;
        
        pGLContext->Clear().ColorBuffer().DepthBuffer();
        
        int nIndex = 0;
        int width = 0;
        int height = 0;
        
        unsigned long milliseconds_since_epoch = GetMsFromEpoch();
        
        if (rotation)
        {
            oglplus::Mat4f rotateLightMatrix = oglplus::ModelMatrixf::RotationA(oglplus::Vec3f(0.0f, 0.5f, 0.3f), oglplus::Degrees(75) * (milliseconds_since_epoch - prevTime) / 1000.0f);
            lightPosition =  rotateLightMatrix * lightPosition;
        }
        
        prevTime = milliseconds_since_epoch;
        
        GetWidnowSize(width, height);
        for (auto & renderObject : *activeObjects)
        {
            pGLContext->Viewport(width * nIndex / 3, 0, width / 3, height);
            
            renderObject.SetVector3fParam("lightPosition", cameraMatrix * lightPosition);

            renderObject.Render(pGLContext);
            nIndex++;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(99));
    }
    
    void CreateSphere()
    {
        using namespace oglplus;
        
        sphere.Bind();
    
        std::vector<GLfloat> data;
        make_sphere.Positions(data);
        nShpereVertexCount = data.size();
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_sphere.Positions(vector);
                     },
                     shperePositions, "Position");
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_sphere.Normals(vector);
                     },
                     shpereNormals, "Normal");
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_sphere.Tangents(vector);
                     },
                     shpereTangents, "Tangent");
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_sphere.TexCoordinates(vector);
                     },
                     shpereTextureCoords, "TexCoord");
        
        // Create indexes buffer.
        std::vector<GLuint> shape_indices = make_sphere.Indices();
        shpereIndexes.Bind(Buffer::Target::ElementArray);
        Buffer::Data(Buffer::Target::ElementArray, shape_indices);
    }
    
    void CreateTorus()
    {
        using namespace oglplus;
        
        torus.Bind();
        
        std::vector<GLfloat> data;
        make_torus.Positions(data);
        nTorusVertexCount = data.size();
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_torus.Positions(vector);
                     },
                     torusPositions, "Position");
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_torus.Normals(vector);
                     },
                     torusNormals, "Normal");
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_torus.Tangents(vector);
                     },
                     torusTangents, "Tangent");
        
        AddDataToVBO([&](std::vector<GLfloat>& vector)->GLuint
                     {
                         return make_torus.TexCoordinates(vector);
                     },
                     torusTextureCoords, "TexCoord");
        
        // Create indexes buffer.
        std::vector<GLuint> torus_indices = make_torus.Indices();
        torusIndexes.Bind(Buffer::Target::ElementArray);
        Buffer::Data(Buffer::Target::ElementArray, torus_indices);
    }
    
    void AddDataToVBO(std::function<GLuint(std::vector<GLfloat>&)> function, oglplus::Buffer& buffer, const std::string& paramName)
    {
        using namespace oglplus;
        std::vector<GLfloat> data;
        GLuint n_per_vertex = function(data);
        
        buffer.Bind(BufferTarget::Array);
        Buffer::Data(BufferTarget::Array, data);
        
        VertexArrayAttrib vert_attr(program.GetProgram(), paramName.data());
        vert_attr.Setup<GLfloat>(n_per_vertex);
        vert_attr.Enable();
    }
    
    void SetupCommonParams(RenderObject& shape,
                           const oglplus::Vec3f& ambient,
                           const oglplus::Vec3f& diffusion,
                           const oglplus::Vec3f& specular,
                           const oglplus::Mat4f& cameraMatrix,
                           const oglplus::Mat4f& modelMatrix,
                           const oglplus::Mat4f& projectionMatrix,
                           const oglplus::Vec4f& lightPosition)
    {
        shape.AddMatrix4fParam("ModelMatrix",  modelMatrix);
        shape.AddMatrix4fParam("CameraMatrix", cameraMatrix);
        shape.AddMatrix4fParam("ProjectionMatrix", projectionMatrix);
        shape.AddMatrix4fParam("NormalMatrix", Transposed(Inverse(cameraMatrix * modelMatrix)));
        shape.AddVector3fParam("ambientColor",  ambient);
        shape.AddVector3fParam("diffuseColor",  diffusion);
        shape.AddVector3fParam("specularColor", specular);
        shape.AddVector3fParam("lightPosition", cameraMatrix * lightPosition);
    }

    virtual void OnResize(int width, int height)
    {
        BaseApplication::OnResize(width, height);
        
        for (auto& shape : spheres)
        {
            shape.SetMatrix4fParam("ProjectionMatrix", oglplus::CamMatrixf::PerspectiveX(oglplus::Degrees(60), double(width / 3)/height,  1, 10.0));
        }
        for (auto& torus : toruses)
        {
            torus.SetMatrix4fParam("ProjectionMatrix", oglplus::CamMatrixf::PerspectiveX(oglplus::Degrees(60), double(width / 3)/height,  1, 10.0));
        }
    }
    
    virtual void OnKeyPress(int key)
    {
        if (key == GLFW_KEY_SPACE)
        {
            rotation = !rotation;
        }
        else if (key == GLFW_KEY_1)
        {
            activeObjects = &spheres;
        }
        else if (key == GLFW_KEY_2)
        {
            activeObjects = &toruses;
        }
    }
    
    unsigned long GetMsFromEpoch()
    {
        return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);;
    }
    
    void LoadBRDFMap(const std::string& filename)
    {
        FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filename.c_str(), 0);
        FIBITMAP* imagen = FreeImage_Load(formato, filename.c_str());
        FIBITMAP* temp = imagen;
        imagen = FreeImage_ConvertTo24Bits(imagen);
        FreeImage_Unload(temp);
        
        int w = FreeImage_GetWidth(imagen);
        int h = FreeImage_GetHeight(imagen);
        
        GLubyte* textura = new GLubyte[3 * w * h];
        memset(textura, 0, 3 * w * h);
        
        char* pixeles = (char*)FreeImage_GetBits(imagen);
        size_t span   = FreeImage_GetLine(imagen);
        
        for(int y = 0; y < h; y++)
        {
            char* pCurrentImageLine   = &pixeles[y * span];
            GLubyte* pCurrentTextureLine = &textura [y * w * 3];
            for(int x = 0; x < w; x++)
            {
                pCurrentTextureLine[x * 3 + 2] = pCurrentImageLine[x * 3 + 0];
                pCurrentTextureLine[x * 3 + 1] = pCurrentImageLine[x * 3 + 1];
                pCurrentTextureLine[x * 3 + 0] = pCurrentImageLine[x * 3 + 2];
            }
        }
        
        oglplus::Texture::Active(0);
        mapTexture.Bind(oglplus::TextureTarget::_2D);

        oglplus::Texture::Image2D  (oglplus::TextureTarget::_2D, 0, oglplus::PixelDataInternalFormat::RGB, w, h, 0, oglplus::PixelDataFormat::RGB, oglplus::DataType::UnsignedByte, textura);
        
        oglplus::Texture::MinFilter(oglplus::TextureTarget::_2D, oglplus::TextureMinFilter::Linear);
        oglplus::Texture::MagFilter(oglplus::TextureTarget::_2D, oglplus::TextureMagFilter::Linear);
        oglplus::Texture::WrapS(oglplus::TextureTarget::_2D, oglplus::TextureWrap::Repeat);
        oglplus::Texture::WrapT(oglplus::TextureTarget::_2D, oglplus::TextureWrap::Repeat);
        
        delete[] textura;
        FreeImage_Unload(imagen);
    }
};

int main(void)
{
    Resource::setRootDir(MAIN_CPP_FILE_PATH);
    Application app;
    
    app.Run();
    
    return 0;
}