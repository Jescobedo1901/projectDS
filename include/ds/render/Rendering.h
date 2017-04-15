#ifndef DS_RENDER_RENDERCONTEXT_H
#define DS_RENDER_RENDERCONTEXT_H

#include <climits>

namespace ds { namespace core {
    class Object; //forward declare
}}
namespace ds { namespace render {

    //ZOrder enum
    //
    struct ZOrder {
        enum ZorderValues {            
            min = INT_MIN,
            background = INT_MIN + 5,
            def = 0,
            text = 10000,            
            effects = 100000,
            max = INT_MAX
        };
    };
    
    struct Color {
        Color() : r(0), g(0), b(0), a(0)
        {            
        }
        Color(  unsigned char _r,
                unsigned char _g, 
                unsigned char _b, 
                unsigned char _a)
            : r(_r), g(_g), b(_b), a(_a)
        {            
        }
        
        //To RGBA
        int toInt()
        {
            return (r << 24) + (g << 16) + (b << 8) + a;
        }
        //To RGB no alpha
        int toRGBInt()
        {
            return (r << 16) + (g << 8) + b;
        }
        
        int r, g, b, a;
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
        
        Renderable(int zo) : zOrder(zo)
        {
        }
        
        Renderable() : zOrder(ZOrder::def)
        {
        }
        
        virtual ~Renderable()
        {
        }

        virtual void setZOrder(int zOrder)
        {
            this->zOrder = zOrder;
        }
        
        virtual int getZOrder() const
        {
            return 0;
        }
        
    private:
        int zOrder;
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
        Sphere() : color()
        {
        }
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

                
        Text() : Renderable(ZOrder::text), style(plain6), text()
        {            
        }        

        TextStyle style;
        Color color;
        std::string text;
    };
}}

#endif /* DS_RENDER_RENDERCONTEXT_H */

