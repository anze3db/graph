#include "Graph.h"
#include <sstream>

// Declare our game instance
Graph game;

Graph::Graph()
    : _scene(NULL)
{
}

void Graph::initialize()
{
    // Load game scene from file
    _scene = Scene::load("res/box.gpb");
    _font = Font::create("res/airstrip28.gpb");
    //FILE* file = FileSystem::openFile("res/data", "r");

    char* file = FileSystem::readAll("res/data");

    if(file == NULL){
        Logger::log(Logger::LEVEL_INFO, "Error reading file");
    }

    std::stringstream str(file);

    for(int i = 0; i < NUM_DAYS; i++){
        for(int j = 0; j < NUM_HOURS; j++){
            str >> data[i][j];
        }
    }

    delete[] file;

    // Set the aspect ratio for the scene's camera to match the current resolution
    Camera * camera = _scene->getActiveCamera();


    Matrix::createPerspective(45.0f, (float)getWidth() / (float)getHeight(), 0.25f, 100.0f, &_projectionMatrix);
    Matrix::createLookAt(0, 50, 30.0f, 0.0, 0, 0.0f, 0, 1, 0.0f, &_viewMatrix);
    Matrix::multiply(_projectionMatrix, _viewMatrix, &_viewProjectionMatrix);

    camera->setProjectionMatrix(_viewProjectionMatrix);
    //camera->_view



    //initLabels();
    initBlocks();

}

void Graph::initBlocks(){
    // Get light node
    Node* lightNode = _scene->findNode("directionalLight");
    Light* light = lightNode->getLight();


    Node* boxNode = _scene->findNode("box");

    for(int i = 0; i < NUM_DAYS; i++){
        for(int j = 0; j < NUM_HOURS; j++){
            Node* boxNode2 = boxNode->clone();
            Model* boxModel2 = boxNode->getModel();

            int scale = 500;
            float translateFactor = 1.5f;
            boxNode2->setTranslation((j-NUM_HOURS/2)*translateFactor, data[i][j]/scale/2, (i+NUM_DAYS/2)*translateFactor);
            boxNode2->setScaleY(data[i][j]/scale);

            Material* material = boxModel2->setMaterial("res/box.material");
            material->getParameter("u_ambientColor")->setValue(Vector3(0.1f, 0.1f, 0.1f));
            material->getParameter("u_lightColor")->setValue(light->getColor());
            material->getParameter("u_lightDirection")->setValue(Vector3(-10, -10, -10));


            material->getParameter("u_diffuseColor")->setValue(Vector4(1 - data[i][j]/5000, 1.0f, 1.0f, 1.0f));
            _scene->addNode(boxNode2);

        }

    }
    _scene->removeNode(boxNode);
}
void Graph::initLabels(){
   // Create a large quad for the labels
   Mesh* textureMesh = Mesh::createQuad(Vector3(-10, 0, -10),
                                       Vector3(-10, 0, 10),
                                       Vector3(10, 0, -10),
                                       Vector3(10, 0, 10));

   // Create the ground model
   Node* labelNode = _scene->addNode("days");
   Node* block  = _scene->findNode("box");

   labelNode->setRotation(block->getRotation());
   labelNode->setTranslation(block->getTranslation());

   mDays = Model::create(textureMesh);
   labelNode->setModel(mDays);
   // Create the ground material
   Material* daysMaterial = mDays->setMaterial("res/textured-unlit.vert", "res/textured-unlit.frag", "TEXTURE_REPEAT;TEXTURE_OFFSET");

   // Set render state block
   RenderState::StateBlock* _stateBlock = RenderState::StateBlock::create();
   _stateBlock->setCullFace(true);
   _stateBlock->setBlend(true);
   _stateBlock->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
   _stateBlock->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
   daysMaterial->setStateBlock(_stateBlock);


   // Bind ground material parameters
   Texture::Sampler* daysSampler = daysMaterial->getParameter("u_diffuseTexture")->setValue("res/asphalt.png", true);
   daysSampler->setWrapMode(Texture::REPEAT, Texture::REPEAT);
   //daysMaterial->getParameter("u_worldViewProjectionMatrix")->setValue(&_viewProjectionMatrix);
   daysMaterial->getParameter("u_textureRepeat")->setValue(Vector2(0, 0));
   daysMaterial->getParameter("u_textureOffset")->setValue(Vector2(0, 1));

   // Release objects that are owned by mesh instances
   SAFE_RELEASE(textureMesh);

}

void Graph::finalize()
{
    SAFE_RELEASE(_scene);
}

void Graph::update(float elapsedTime)
{
    Node* lightNode = _scene->findNode("directionalLight");
    lightNode->translateForward(1.0f);
}

void Graph::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::one(), 1.0f, 0);

    // Visit all the nodes in the scene for drawing
    _scene->visit(this, &Graph::drawScene);
    //mDays->draw();

    _font->start();
    char text[1024];
    sprintf(text, "Hours (00:00 - 24:00)");
    _font->drawText(text, getWidth()/2, getHeight()*4/5, Vector4(0, 0, 0, 1), _font->getSize());


    sprintf(text, "Days (Monday - Sunday)");
    _font->drawText(text, 50, 200, Vector4(0, 0, 0, 1), _font->getSize());


    _font->finish();



}

bool Graph::drawScene(Node* node)
{
    // If the node visited contains a model, draw it
    Model* model = node->getModel(); 
    if (model)
    {
        model->draw();
    }
    return true;
}

void Graph::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;
        }
    }
}

void Graph::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}
