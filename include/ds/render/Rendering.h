#ifndef DS_RENDER_RENDERCONTEXT_H
#define DS_RENDER_RENDERCONTEXT_H


namespace ds { namespace core {
    class Object; //forward declare
}}
namespace ds { namespace render {

    
    struct Color {
        Color() : r(), g(), b(), a()
        {            
        }
        Color(  unsigned char _r,
                unsigned char _g, 
                unsigned char _b, 
                unsigned char _a)
            : r(_r), g(_g), b(_b), a(_a)
        {            
        }
        int toInt()
        {
            return (r << 24) + (r << 16) + (b << 8) + b;
        }
        
        unsigned char r, g, b, a;
    };
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
        Color color;
    };

    struct Texture : public Renderable {
    };

    enum TextStyle {
        plain6,
        plain7,
        plain8,
        bold8,
        plain10,
        plain12,
        plain13,
        plain16,
        plain17,
        plain40
    };
    struct Text : public Renderable {
        //Currently supported styles

                
        Text() : style(plain6), text()
        {            
        }        

        TextStyle style;
        Color color;
        std::string text;
    };
}}

#endif /* DS_RENDER_RENDERCONTEXT_H */

