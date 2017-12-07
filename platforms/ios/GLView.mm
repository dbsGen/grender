//
//  GPView.m
//  hirender_iOS
//
//  Created by mac on 2017/1/14.
//  Copyright © 2017年 gen. All rights reserved.
//

#import "GLView.h"
#import "Define.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "Renderer.h"
#include "FileSystem.h"
#include "GL2RendererIMP.h"

#if true//(defined(__APPLE__) && !defined(DEBUG))
#define GL_ASSERT( gl_code ) gl_code
#else
#define GL_ASSERT( gl_code ) do \
{ \
gl_code; \
GLenum __gl_error_code = glGetError(); \
assert(__gl_error_code == GL_NO_ERROR); \
} while(0)
#endif


@interface GLView (Private)
- (BOOL)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation GLView {
    BOOL _needsDisplay;
    void *_renderer;
}

@synthesize updating;
@synthesize context;

+ (void)load {
    FileSystem::getInstance()->setResourcePath([[NSBundle mainBundle] resourcePath].UTF8String);
    NSArray *arr = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask,YES);
    FileSystem::getInstance()->setStoragePath([arr.firstObject UTF8String]);
}

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame {
    self = [self initWithFrame:frame context:nil];
    if (self) {
        
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame context:(EAGLContext*)c
{
    if ((self = [super initWithFrame:frame]))
    {
        // A system version of 3.1 or greater is required to use CADisplayLink.
        NSString *reqSysVer = @"3.1";
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
        {
            // Log the system version
            //NSLog(@"System Version: %@", currSysVer);
        }
        else
        {
            LOG(e, "Invalid OS Version: %s\n", (currSysVer == NULL?"NULL":[currSysVer cStringUsingEncoding:NSASCIIStringEncoding]));
            return nil;
        }
        
        // Check for OS 4.0+ features
        if ([currSysVer compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
        {
            oglDiscardSupported = YES;
        }
        else
        {
            oglDiscardSupported = NO;
        }
        
        // Configure the CAEAGLLayer and setup out the rendering context
        CGFloat scale = [[UIScreen mainScreen] scale];
        CAEAGLLayer* layer = (CAEAGLLayer *)self.layer;
        layer.opaque = TRUE;
        layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        self.contentScaleFactor = scale;
        layer.contentsScale = scale;
        //        layer.backgroundColor = [UIColor greenColor].CGColor;
        
        if (c)
            context = c;
        else context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!context || ![EAGLContext setCurrentContext:context])
        {
            LOG(e ,"Failed to make context current.");
            return nil;
        }
        
        // Initialize Internal Defaults
        updateFramebuffer = YES;
        defaultFramebuffer = 0;
        colorRenderbuffer = 0;
        depthRenderbuffer = 0;
        framebufferWidth = 0;
        framebufferHeight = 0;
        multisampleFramebuffer = 0;
        multisampleRenderbuffer = 0;
        multisampleDepthbuffer = 0;
        
        // Set the resource path and initalize the game
        //        NSString* bundlePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"];
        //        FileSystem::setResourcePath([bundlePath fileSystemRepresentation]);
    }
    return self;
}

- (void) dealloc
{
    [self deleteFramebuffer];
    
    if ([EAGLContext currentContext] == context)
    {
        [EAGLContext setCurrentContext:nil];
    }
}

- (BOOL)canBecomeFirstResponder
{
    // Override so we can control the keyboard
    return YES;
}

- (void) layoutSubviews
{
    // Called on 'resize'.
    // Mark that framebuffer needs to be updated.
    // NOTE: Current disabled since we need to have a way to reset the default frame buffer handle
    // in FrameBuffer.cpp (for FrameBuffer:bindDefault). This means that changing orientation at
    // runtime is currently not supported until we fix this.
    //updateFramebuffer = YES;
}

- (BOOL)createFramebuffer
{
    // iOS Requires all content go to a rendering buffer then it is swapped into the windows rendering surface
    assert(defaultFramebuffer == 0);
    
    // Create the default frame buffer
    GL_ASSERT( glGenFramebuffers(1, &defaultFramebuffer) );
    GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer) );
    
    // Create a color buffer to attach to the frame buffer
    GL_ASSERT( glGenRenderbuffers(1, &colorRenderbuffer) );
    GL_ASSERT( glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer) );
    
    // Associate render buffer storage with CAEAGLLauyer so that the rendered content is display on our UI layer.
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    higraphics::GL2RendererIMP *rimp = ((hirender::Renderer*)_renderer)->getIMP()->cast_to<higraphics::GL2RendererIMP>();
    if (rimp)
        rimp->setDefaultFrameBuffer(defaultFramebuffer);
    
    // Attach the color buffer to our frame buffer
    GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer) );
    
    // Retrieve framebuffer size
    GL_ASSERT( glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth) );
    GL_ASSERT( glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight) );
    
    // If multisampling is enabled in config, create and setup a multisample buffer
    
    int multisampling = 0;
    if (multisampling)
    {
        // Create multisample framebuffer
        GL_ASSERT( glGenFramebuffers(1, &multisampleFramebuffer) );
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer) );
        
        // Create multisample render and depth buffers
        GL_ASSERT( glGenRenderbuffers(1, &multisampleRenderbuffer) );
        GL_ASSERT( glGenRenderbuffers(1, &multisampleDepthbuffer) );
        
        // Try to find a supported multisample configuration starting with the defined sample count
        while (multisampling)
        {
            GL_ASSERT( glBindRenderbuffer(GL_RENDERBUFFER, multisampleRenderbuffer) );
            GL_ASSERT( glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, multisampling, GL_RGBA8_OES, framebufferWidth, framebufferHeight) );
            GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisampleRenderbuffer) );
            
            GL_ASSERT( glBindRenderbuffer(GL_RENDERBUFFER, multisampleDepthbuffer) );
            GL_ASSERT( glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, multisampling, GL_DEPTH_COMPONENT24_OES, framebufferWidth, framebufferHeight) );
            GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampleDepthbuffer) );
            
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
                break; // success!
            
            //NSLog(@"Creation of multisample buffer with samples=%d failed. Attempting to use configuration with samples=%d instead: %x", samples, samples / 2, glCheckFramebufferStatus(GL_FRAMEBUFFER));
            multisampling /= 2;
        }
        
        //todo: __multiSampling = samples > 0;
        
        // Re-bind the default framebuffer
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer) );
        
        if (multisampling == 0)
        {
            // Unable to find a valid/supported multisample configuratoin - fallback to no multisampling
            GL_ASSERT( glDeleteRenderbuffers(1, &multisampleRenderbuffer) );
            GL_ASSERT( glDeleteRenderbuffers(1, &multisampleDepthbuffer) );
            GL_ASSERT( glDeleteFramebuffers(1, &multisampleFramebuffer) );
            multisampleFramebuffer = multisampleRenderbuffer = multisampleDepthbuffer = 0;
        }
    }
    
    // Create default depth buffer and attach to the frame buffer.
    // Note: If we are using multisample buffers, we can skip depth buffer creation here since we only
    // need the color buffer to resolve to.
    if (multisampleFramebuffer == 0)
    {
        GL_ASSERT( glGenRenderbuffers(1, &depthRenderbuffer) );
        GL_ASSERT( glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer) );
        GL_ASSERT( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, framebufferWidth, framebufferHeight) );
        GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer) );
    }
    
    // Sanity check, ensure that the framebuffer is valid
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //NSLog(@"ERROR: Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        [self deleteFramebuffer];
        return NO;
    }
    
    // If multisampling is enabled, set the currently bound framebuffer to the multisample buffer
    // since that is the buffer code should be drawing into (and FrameBuffr::initialize will detect
    // and set this bound buffer as the default one during initialization.
    if (multisampleFramebuffer)
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer) );
    
    return YES;
}

- (void)deleteFramebuffer
{
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        if (defaultFramebuffer)
        {
            GL_ASSERT( glDeleteFramebuffers(1, &defaultFramebuffer) );
            defaultFramebuffer = 0;
        }
        if (colorRenderbuffer)
        {
            GL_ASSERT( glDeleteRenderbuffers(1, &colorRenderbuffer) );
            colorRenderbuffer = 0;
        }
        if (depthRenderbuffer)
        {
            GL_ASSERT( glDeleteRenderbuffers(1, &depthRenderbuffer) );
            depthRenderbuffer = 0;
        }
        if (multisampleFramebuffer)
        {
            GL_ASSERT( glDeleteFramebuffers(1, &multisampleFramebuffer) );
            multisampleFramebuffer = 0;
        }
        if (multisampleRenderbuffer)
        {
            GL_ASSERT( glDeleteRenderbuffers(1, &multisampleRenderbuffer) );
            multisampleRenderbuffer = 0;
        }
        if (multisampleDepthbuffer)
        {
            GL_ASSERT( glDeleteRenderbuffers(1, &multisampleDepthbuffer) );
            multisampleDepthbuffer = 0;
        }
    }
}

- (void)swapBuffers
{
    if (context)
    {
        if (multisampleFramebuffer)
        {
            // Multisampling is enabled: resolve the multisample buffer into the default framebuffer
            GL_ASSERT( glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, defaultFramebuffer) );
            GL_ASSERT( glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, multisampleFramebuffer) );
            GL_ASSERT( glResolveMultisampleFramebufferAPPLE() );
            
            if (oglDiscardSupported)
            {
                // Performance hint that the GL driver can discard the contents of the multisample buffers
                // since they have now been resolved into the default framebuffer
                const GLenum discards[]  = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT };
                GL_ASSERT( glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, discards) );
            }
        }
        else
        {
            if (oglDiscardSupported)
            {
                // Performance hint to the GL driver that the depth buffer is no longer required.
                const GLenum discards[]  = { GL_DEPTH_ATTACHMENT };
                GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer) );
                GL_ASSERT( glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards) );
            }
        }
        
        // Present the color buffer
        GL_ASSERT( glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer) );
        [context presentRenderbuffer:GL_RENDERBUFFER];
    }
}

- (void)display {
    [self update];
    _needsDisplay = NO;
}

- (void)setNeedsDisplay {
    if (!_needsDisplay) {
        _needsDisplay = YES;
        [self performSelectorOnMainThread:@selector(willDisplay)
                               withObject:nil
                            waitUntilDone:NO];
    }
}

- (void)willDisplay {
    if (_needsDisplay) {
        [self display];
    }
}

- (void)update
{
    if (context != nil)
    {
        // Ensure our context is current
        [EAGLContext setCurrentContext:context];
        
        // If the framebuffer needs (re)creating, do so
        if (updateFramebuffer)
        {
            updateFramebuffer = NO;
            [self deleteFramebuffer];
            [self createFramebuffer];
            
        }
        
        // Bind our framebuffer for rendering.
        // If multisampling is enabled, bind the multisample buffer - otherwise bind the default buffer
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer ? multisampleFramebuffer : defaultFramebuffer) );
        GL_ASSERT( glViewport(0, 0, framebufferWidth, framebufferHeight)); //
        
        // Execute a single game frame
        
        [self.delegate glkView:self
                    drawInRect:self.bounds];
        
        // Present the contents of the color buffer
        [self swapBuffers];
    }
}

- (void)setFrame:(CGRect)frame {
    [super setFrame:frame];
    CGFloat scale = [UIScreen mainScreen].scale;
    if (_renderer)
        ((hirender::Renderer*)_renderer)->setSize(HSize(frame.size.width*scale, frame.size.height*scale));
    updateFramebuffer = YES;
    [self display];
}

@end
