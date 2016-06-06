//
//  AppDelegate.cpp
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#include "KKApplication.h"
#include "KKApplicationProtocol.h"
#include "KKTimer.h"
#include "KKAutoreleasePool.h"
#include "KKStringUtil.hpp"
#include "KKSceneManager.hpp"
#include "KKDirector.h"
#include "KKTexture.hpp"
#include "KKResourceManager.hpp"
#include "KKGLMatrixStack.h"

#include "KKTransform2DComponent.hpp"
#include "KKSpriteComponent.hpp"
#include "KKColorComponent.hpp"
#include "KKColorAdvanceComponent.hpp"
#include "KKClippingComponent.hpp"
#include "KKScissorComponent.hpp"
#include "KKShaderComponent.hpp"
#include "KKShaderProgram.hpp"

USING_NS_KK

void beginRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //glClearColor(1,0,0,1);
    
    KKGLMatrixStack* matrixStack = KKGLMatrixStack::currentMatrixStack();
    
    static KKTransform2DComponent* transComp = new KKTransform2DComponent;
    matrixStack->pushMatrix();
    if (transComp)
    {
        static kmMat4* transform = transComp->get_transform();
        matrixStack->multMatrix(transform);
    }
    
    
    KKTexture *texture = KKTexture::getTexture("button.png");
    KKPoint point = KKPoint(30,80);
    texture->drawAtPoint(point);
}

void endRender()
{
    KKGLMatrixStack* matrixStack = KKGLMatrixStack::currentMatrixStack();
    matrixStack->popMatrix();
}

void callback(KKTimer *, float dt)
{
    KKDirector *director = KKDirector::getSingleton();
    director->onRenderBegin += newDelegate(&beginRender);
    director->onRenderEnd += newDelegate(&endRender);
}

class ApplicationDelegate : public KKApplicationDelegate {
    
public:
    virtual bool applicationDidFinishLaunching()
    {
        //TODO  Insert code here to start your application
        
        KKSceneManager *sceneMgr = new KKSceneManager();
        
        KKDirector *director = KKDirector::getSingleton();
        
        director->setDesignWinSize(KKSize(320, 0));
        KKResourceManager::getSingleton()->setResourceScale(director->getWinSizeScale());
        
        KKResourceManager::getSingleton()->initializeResPath();

//        KKEntity *entity = sceneMgr->getEntityManager()->createAnonymousEntity("KKTransform2DComponent", "KKSpriteComponent", "KKClippingComponent", nullptr);
//        
//        //entity->getComponent<KKTransform2DComponent>()->set_size(KKSize(130, 80));
//        entity->getComponent<KKTransform2DComponent>()->set_position(KKPoint(30, 40));
//        
//        KKTexture *texture = KKTexture::getTexture("grass.png");
//        
//        entity->getComponent<KKSpriteComponent>()->set_texture(texture);
//        
//        entity->getComponent<KKSpriteComponent>()->set_brightness(1.0);
//        
//        //entity->setProperty("gray", true);
//        
//        KKEntity *entity2 = entity->getEntityManager()->createAnonymousEntity("KKTransform2DComponent", "KKSpriteComponent", nullptr);
//        
//        entity2->setProperty("texture", KKTexture::getTexture("tree.png"));
//        
//        entity->setProperty("stencil", entity2);
//        
//        sceneMgr->addEntity(entity);
        
//      director->onRenderBegin += newDelegate(&beginRender);
//      director->onRenderEnd += newDelegate(&endRender);
        
        KKEntity *clippingEntity = sceneMgr->getEntityManager()->createAnonymousEntity("KKTransform2DComponent", "KKClippingComponent", nullptr);
        clippingEntity->setProperty("position", KKPoint(100, 100));
        
        KKEntity* entity = sceneMgr->getEntityManager()->createEntity("mask", "KKTransform2DComponent", "KKSpriteComponent", "KKShaderComponent", NULL);
//        KKProgram *program = KKProgramManager::currentManager()->getProgram(kShaderPositionTextureColorAlphaTest);
//        entity->setProperty("shaderProgram", program);
//        program->getUniform(kUniformAlphaThreshold).setUniform1f(0.05f);
        entity->setProperty("size", KKSize(128, 128));
        entity->setProperty("texture", KKTexture::getTexture("ChatBubbleGreen.png"));
        clippingEntity->setProperty("stencil", entity);
        
        
        entity = sceneMgr->getEntityManager()->createEntity("tttttt1", "KKTransform2DComponent", "KKSpriteComponent", NULL);
        entity->setProperty("size", KKSize(128, 128));
        entity->setProperty("texture", KKTexture::getTexture("grass.png"));
        clippingEntity->getComponent<KKTransform2DComponent>()->addChild(entity);

        
        sceneMgr->addEntity(clippingEntity);
        
        director->runWithSceneManager(sceneMgr);
        
        
        return true;
    }
    
    virtual void applicationDidEnterBackground()
    {
        
    }
    
    virtual void applicationWillEnterForeground()
    {
        
    }
    
    virtual void applicationWillTerminate()
    {
        
    }
    
};