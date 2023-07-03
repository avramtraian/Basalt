// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "RendererAPI.h"
#include "RendererInterface.h"

namespace Basalt
{

// Forward declarations.
class RenderingContext;
class Window;

/**
 * Structure that describes the renderer.
 */
struct RendererDescription
{
    /** The native API to be used. `ERendererAPI::None` is only available on server builds. */
    ERendererAPI renderer_api = ERendererAPI::None;
    
    /** Pointer to the game's primary window. Used when creating the primary rendering context. */
    Window* primary_window = nullptr;
};

/**
 * The renderer class is responsible for managing the rendering process.
 */
class BASALT_API Renderer
{
public:
    static bool Initialize(const RendererDescription& description);
    static void Shutdown();

    /** @return True if the renderer is initializes and can be used; False otherwise. */
    static bool IsInitialized();

public:
    /** @return The rendering API that is used for graphics. */
    static ERendererAPI GetRendererAPI();

    /** @return The currently active rendering context. Guaranteed to be a valid context. */
    static RenderingContext* GetActiveContext();

    /**
     * Sets the currently active rendering context.
     * By passing nullptr to the function, the renderer will set the default primary
     * rendering context as active.
     * 
     * @param new_context The rendering context to set as active. If this value is nullptr
     *                    the renderer will set the default primary rendering context as active.
     */
    static void SetActiveContext(RenderingContext* new_context);

public:
    static void BeginRenderPass(Ref<RenderPass> render_pass);

    static void EndRenderPass(Ref<RenderPass> render_pass);

private:
    /** The pointer to the native API renderer. */
    static RendererInterface* s_interface;
};

} // namespace Basalt
