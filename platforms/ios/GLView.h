//
//  GPView.h
//  hirender_iOS
//
//  Created by mac on 2017/1/14.
//  Copyright © 2017年 gen. All rights reserved.
//

#import <UIKit/UIKit.h>

@class GLView;

@protocol GLViewDelegate <NSObject>

@required
- (void)glkView:(GLView *)view drawInRect:(CGRect)rect;

@end

@interface GLView : UIView
{
    EAGLContext* context;
    BOOL updateFramebuffer;
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    GLint framebufferWidth;
    GLint framebufferHeight;
    GLuint multisampleFramebuffer;
    GLuint multisampleRenderbuffer;
    GLuint multisampleDepthbuffer;
    BOOL oglDiscardSupported;
}

@property (nonatomic, assign) void *renderer;
@property (nonatomic, weak) id<GLViewDelegate> delegate;
@property (readonly, nonatomic, getter=isUpdating) BOOL updating;
@property (readonly, nonatomic, getter=getContext) EAGLContext* context;


- (void)display;
- (void)setNeedsDisplay;

- (void)swapBuffers;
- (void)zero;

- (id)initWithFrame:(CGRect)rect context:(EAGLContext*)context;

@end
