#include <p2d/app/app.h>
#include <p2d/app/object.h>

#include <renderer.h>
#include <pipeline/pipeline.h>

sad::p2d::app::App::App() : m_layer(0), m_world(NULL), m_steptask(NULL), m_renderer(NULL)
{
}

sad::p2d::app::App::~App()
{
    if (m_world)
        delete m_world;
}

void sad::p2d::app::App::initApp(unsigned int layer, sad::Renderer * r)
{
    m_layer = layer;
    // Fetch global renderer, if needed
    if (r == NULL)
    {
        r = sad::Renderer::ref();
    }

    m_renderer = r;
    m_world = NULL;
    createWorld();

    // Inserts world step task to an end of pipeline
    m_steptask = new p2d::WorldStepTask(m_world, r);
    m_steptask->mark("sad::p2d::WorldStepTask");
    r->pipeline()->append(m_steptask);
}

sad::Scene * sad::p2d::app::App::scene()
{
    return m_renderer->scenes()[m_layer];
}

void sad::p2d::app::App::createWorld()
{
    m_world = new sad::p2d::World();
    m_world->setDetector(new p2d::SimpleCollisionDetector());
}

void sad::p2d::app::App::quit()
{
    m_renderer->quit();
}


void sad::p2d::app::App::run()
{

}

void sad::p2d::app::App::removeObject(sad::p2d::app::Object * o)
{
    sad::p2d::Body * b = o->body();
    m_renderer->scenes()[m_layer]->remove(o);
    m_world->removeBody(b);
}


void sad::p2d::app::App::addObject(sad::p2d::app::Object * o)
{
    o->setApp(this);
    m_renderer->scenes()[m_layer]->add(o);
    m_world->addBody(o->body());
}
