/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RenderContext.h
 * Author: thor
 *
 * Created on February 22, 2017, 11:22 AM
 */

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
        virtual ~RenderContext() {}        
    };


    class Renderable;
    
    /**
     * All Renderable objects must have a Renderer that is applicable
     */
    class Renderer {        
    public:
        virtual ~Renderer() {}
        virtual void render(RenderContext* ctx, core::Object* obj, Renderable* renderable) = 0;
    };

    /**
     * All renderable objects must inherit this class
     */
    class Renderable {
    public:
        /**
         * Check if a Renderer is applicable to Renderable
         * @param render
         * @return
         */
        virtual bool isRenderer(Renderer* render)  = 0;

    };

    /**
     * Basic shape
     */
    class Shape : public Renderable {
        virtual bool isRenderer(Renderer* render);
    };

    class Texture : public Renderable {
        
    };

    class Text : public Renderable {
        
    };
}}

#endif /* DS_RENDER_RENDERCONTEXT_H */

