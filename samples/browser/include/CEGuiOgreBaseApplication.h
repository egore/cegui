/***********************************************************************
    created:    9/3/2004
    author:     Paul D Turner
                David Reepmeyer (modified for android)
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#ifndef _CEGuiOgreBaseApplication_h_
#define _CEGuiOgreBaseApplication_h_

#include "CEGuiBaseApplication.h"
#include "CEGUI/CEGUI.h"

#include "CEGUI/RendererModules/Ogre/Renderer.h"
#include <Ogre.h>
#if (OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0))
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#else
#include <OIS.h>
#include <OgreFrameListener.h>
#include <OgreBuildSettings.h>
#ifdef OGRE_STATIC_LIB
#   ifdef OGRE_BUILD_RENDERSYSTEM_D3D9
#       define OGRE_STATIC_Direct3D9
#   endif
#   ifdef OGRE_BUILD_RENDERSYSTEM_D3D11
#       define OGRE_STATIC_Direct3D11
#   endif
#   ifdef OGRE_BUILD_RENDERSYSTEM_GL
#       define OGRE_STATIC_GL
#   endif
#   ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#       define OGRE_STATIC_GL3Plus
#   endif
#   ifdef OGRE_BUILD_RENDERSYSTEM_GLES
#   define OGRE_STATIC_GLES
#   endif
#   ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#       define OGRE_STATIC_GLES2
#   endif
#   include <OgreStaticPluginLoader.h>
#endif

// Frame listener forward ref (see class below)
class CEGuiDemoFrameListener;
// Window event listener forward ref (see class below)
class WndEvtListener;
#endif

#if (OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0))
class CEGuiOgreBaseApplication : public CEGuiBaseApplication, public OgreBites::ApplicationContext, public OgreBites::InputListener
#else
class CEGuiOgreBaseApplication : public CEGuiBaseApplication, public Ogre::FrameListener
#endif
{
public:
    //! Constructor.
    CEGuiOgreBaseApplication();

    //! Destructor.
    virtual ~CEGuiOgreBaseApplication();

    bool init(SampleBrowserBase* sampleApp, const CEGUI::String &logFile,
              const CEGUI::String &dataPathPrefixOverride);

    void destroyRenderer();

    //! called by the frame listener to perform requried per-frame updates.
    void doFrameUpdate(float elapsed);

    //! Ogre frame listener callback that will trigger our specific context rendering
    bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    bool isInitialised();
    void run();

#if (OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0))
    void setup();

    bool frameStarted(const Ogre::FrameEvent& evt);

    bool keyPressed(const OgreBites::KeyboardEvent& evt);
    bool keyReleased(const OgreBites::KeyboardEvent& evt);
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);
    bool mousePressed(const OgreBites::MouseButtonEvent& evt);
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt);

    void windowResized(Ogre::RenderWindow* rw);

    void locateResources() {}
    void loadResources() {}
#endif

protected:
    // override from base class since we use a non-default resource provider.
    void initialiseResourceGroupDirectories();
    // retrieve the full path of the provided resource folder
    CEGUI::String getResourcePath(CEGUI::String resource);

    // Implementation of base class abstract methods.
    void destroyWindow();
    void beginRendering(const float elapsed);
    void endRendering();

#if (OGRE_VERSION < ((1 << 16) | (10 << 8) | 0))
    // We set up our Ogre default config to match our other renderer's default values if possible
    // and if no config available already
    void setupDefaultConfigIfNeeded();
#endif

    /*************************************************************************
        Data Fields
    *************************************************************************/
#if (OGRE_VERSION < ((1 << 16) | (10 << 8) | 0))
    Ogre::Root* d_ogreRoot;
#endif
#if (OGRE_VERSION >= ((1 << 16) | (13 << 8) | 0))
    Ogre::SceneNode* d_camNode;
#endif
    Ogre::Camera* d_camera;
#if (OGRE_VERSION < ((1 << 16) | (10 << 8) | 0))
    Ogre::RenderWindow* d_window;
#endif
    bool d_initialised;

#if (OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0))
    // convert an SDL mouse button into a CEGUI mouse button
    CEGUI::MouseButton convertButtonToCegui(int buttonID);
#else
#ifdef OGRE_STATIC_LIB
    Ogre::StaticPluginLoader* d_staticPluginLoader;
#endif

    CEGuiDemoFrameListener* d_frameListener;
    WndEvtListener* d_windowEventListener;
#endif
};

#if (OGRE_VERSION < ((1 << 16) | (10 << 8) | 0))
/*!
\brief
    Ogre FrameListener class where we deal with input processing and the like.
*/
class CEGuiDemoFrameListener : public Ogre::FrameListener, public OIS::KeyListener,
#ifdef __ANDROID__
    public OIS::MultiTouchListener
#else
    public OIS::MouseListener
#endif
{
public:
    // Construction and Destruction
    CEGuiDemoFrameListener(CEGuiOgreBaseApplication* baseApp, SampleBrowserBase*& sampleApp, Ogre::RenderWindow* win, Ogre::Camera* cam);
    ~CEGuiDemoFrameListener();

    // Processing to be done at start and end of each frame.
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

    // Raw input handlers that we care about
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

#ifdef __ANDROID__
    virtual bool touchPressed (const OIS::MultiTouchEvent& evt);
    virtual bool touchMoved (const OIS::MultiTouchEvent& evt);
    virtual bool touchReleased (const OIS::MultiTouchEvent& evt);
    virtual bool touchCancelled (const OIS::MultiTouchEvent & evt) {}
#else
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    OIS::Mouse* getOISMouse();
#endif

protected:
    // convert an OIS mouse button into a CEGUI mouse button
    CEGUI::MouseButton convertOISButtonToCegui(int buttonID);

    /*************************************************************************
        Data Fields
    *************************************************************************/
    OIS::InputManager* d_inputManager;
    OIS::Keyboard* d_keyboard;

#ifdef __ANDROID__
    OIS::MultiTouch* d_touch;
#else
    OIS::Mouse* d_mouse;
#endif
    Ogre::Camera* d_camera;
    Ogre::RenderWindow* d_window;

    CEGuiOgreBaseApplication*   d_baseApp;
    SampleBrowserBase*&         d_sampleApp;
};

//! window event listener class we use to hear abour window resizing
class WndEvtListener : public Ogre::WindowEventListener
{
public:
#ifdef __ANDROID__
    WndEvtListener();
#else
    WndEvtListener(OIS::Mouse* mouse);
#endif

    virtual void windowResized(Ogre::RenderWindow* rw) ;
    virtual void windowClosed (Ogre::RenderWindow*) {}
    virtual bool windowClosing (Ogre::RenderWindow*) { return true; }
    virtual void windowFocusChange (Ogre::RenderWindow*) {}
    virtual void windowMoved (Ogre::RenderWindow*) {}

protected:
#ifndef __ANDROID__
    OIS::Mouse* d_mouse;
#endif
};
#endif

#endif  // end of guard _CEGuiOgreBaseApplication_h_
