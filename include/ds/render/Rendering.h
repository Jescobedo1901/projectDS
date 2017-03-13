#ifndef DS_RENDER_RENDERCONTEXT_H
#define DS_RENDER_RENDERCONTEXT_H


namespace ds { namespace core {
    class Object; //forward declare
}}
namespace ds { namespace render {

    /**
     * Render Context class
     */
    class RenderContext {
    public:
        RenderContext() {}
        virtual ~RenderContext() 
        {
        }
    };


    class Renderable;
    
    /**
     * All Renderable objects must have a Renderer that is applicable
     */
    class Renderer {        
    public:
        virtual ~Renderer()
        {
        }
        virtual void render(RenderContext* ctx, core::Object* obj, Renderable* renderable) = 0;
        
        /**
         * Check if a Renderer is applicable to Renderable
         * @param render
         * @return
         */
        virtual bool isRenderer(Renderable* render)  = 0;
    };

    typedef std::shared_ptr<Renderer> RendererPtr;

    /**
     * All renderable objects must inherit this class
     */
    struct Renderable {
        virtual ~Renderable()
        {
        }
    };

    /**
     * Basic shape
     */
    struct Shape : public Renderable {
            virtual ~Shape()
            {
            }
    };

    struct Sphere : public Shape {
        virtual ~Sphere()
        {
        }
    };

    struct Texture : public Renderable {
    };

    struct Text : public Renderable {

        std::string text;
    };
}}

#endif /* DS_RENDER_RENDERCONTEXT_H */

