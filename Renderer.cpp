#include "Renderer.h"

#include "ShaderManager.h"
#include "MyGUI.h"




Renderer::Renderer() {}

void Renderer::Init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //glStencilFunc(GL_ALWAYS, 1, 0xFF);

    std::cout << "NUMBER OF SHADERS " << shaderSingleton->getNumberOfShaders();
}


void Renderer::Render(Window& window,MyGUI& gui)
{
    glClearColor(0.23f,0.33f,0.33f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto camera = window.getCamera();

    camera.Update();

    glStencilMask(0x00);
 
    for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
    {
        if (i == gui.getObjectIndex())
        {
            if (gui.getMode() == Mode::OBJECT)
            {
    
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
                glPointSize(5.0f);
                objectSingleton->getObject(i)->Draw(shaderSingleton->getShader("Basic"), camera);
    
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilMask(0x00);
                glDisable(GL_DEPTH_TEST);
                objectSingleton->getObject(i)->Draw(shaderSingleton->getShader("Select"), camera);
    
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glEnable(GL_DEPTH_TEST);
    
            }
            else if (gui.getMode() == Mode::EDIT)
            {
                objectSingleton->getObject(i)->Draw(shaderSingleton->getShader("Basic"), camera);
    
    
                objectSingleton->getObject(i)->Draw(shaderSingleton->getShader("EditMode"), camera, GL_LINES);
    
    
    
                if (gui.getSelectMode() == SelectMode::VERTEX)
                {
                    glPointSize(5.0f);
                    objectSingleton->getObject(i)->Draw(shaderSingleton->getShader("EditMode"), camera, GL_POINTS);
                    if (gui.getVertexIndex() != -1)
                    {
                       
    
                        shaderSingleton->getShader("SelectEdit").Activate();
                        objectSingleton->getObject(i)->bindVAO();
                        auto x = objectSingleton->getObject(i)->getModelReference();
                        shaderSingleton->getShader("SelectEdit").setMat4(true,"model",x);
                        
                        camera.CameraUniform(shaderSingleton->getShader("SelectEdit"), "cameraMatrix");
                        glDrawArrays(GL_POINTS, gui.getVertexIndex(), 1);

 
 
    
                    }
                    glPointSize(1.0f);
                }
    
            }
    
        }
        else
        {
        }
        objectSingleton->getObject(i)->Draw(shaderSingleton->getShader("Basic"), camera);
    
    }

  

    
}
