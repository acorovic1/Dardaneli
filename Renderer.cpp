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
    auto& selectedObjects = gui.getObjectIndex();
    camera.Update();

    glStencilMask(0x00);
 
    for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
    {
        Object* object = objectSingleton->getObject(i);
        if (std::any_of(selectedObjects.begin(), selectedObjects.end(), [i](int a) {return i == a; }))
        {
            if (gui.getMode() == Mode::OBJECT)
            {
    
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
                glPointSize(5.0f);
                object->Draw(shaderSingleton->getShader("Basic"), camera);
    
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilMask(0x00);
                glDisable(GL_DEPTH_TEST);
                if(i == selectedObjects[selectedObjects.size()-1])
                    object->Draw(shaderSingleton->getShader("ActiveSelect"), camera);//OUTLINE
                else
                    object->Draw(shaderSingleton->getShader("Select"), camera);//OUTLINE
    
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glEnable(GL_DEPTH_TEST);
    
            }
            else if (gui.getMode() == Mode::EDIT)
            {
                object->Draw(shaderSingleton->getShader("Basic"), camera);
    
    
                object->Draw(shaderSingleton->getShader("EditMode"), camera, GL_LINES);
                
    
    
                if (gui.getSelectMode() == SelectMode::VERTEX)
                {
                    glPointSize(5.0f);
                    object->Draw(shaderSingleton->getShader("EditMode"), camera, GL_POINTS);
                    auto& selectedVertices = static_cast<Mesh*>(object)->getSelectedVertices();

                    shaderSingleton->getShader("SelectEdit").Activate();
                    object->bindVAO();
                    auto x = object->getModelReference();

                    shaderSingleton->getShader("SelectEdit").setMat4(true, "model", x);
                    camera.CameraUniform(shaderSingleton->getShader("SelectEdit"), "cameraMatrix");

                    for(int j=0;j<selectedVertices.size();j++)
                    {                       
                        if (j == selectedVertices.size() - 1)
                        {
                            shaderSingleton->getShader("ActiveEdit").Activate();
                            shaderSingleton->getShader("ActiveEdit").setMat4(true, "model", x);
                            camera.CameraUniform(shaderSingleton->getShader("ActiveEdit"), "cameraMatrix");
                        }
                        glDrawArrays(GL_POINTS, selectedVertices[j], 1);
                    }

                    glPointSize(1.0f);
                }
    
            }
    
        }
        
        object->Draw(shaderSingleton->getShader("Basic"), camera);
    
    }

  

    
}
