#include <algorithm>

#include "pixelboost/debug/assert.h"
#include "pixelboost/debug/log.h"
#include "pixelboost/resource/resourceManager.h"
#include "pixelboost/resource/resource.h"

using namespace pb;

ResourcePool::ResourcePool()
    : _Priority(0)
{
    
}

ResourcePool::~ResourcePool()
{
    
}

int ResourcePool::GetPriority()
{
    return _Priority;
}

void ResourcePool::SetPriority(int priority)
{
    _Priority = priority;
}

void ResourcePool::LoadResource(const std::string& filename)
{
    auto resource = _Resources.find(filename);
    
    if (resource != _Resources.end())
    {
        if (auto resourceHandle = resource->second.lock())
        {
            std::lock(resourceHandle->_ProcessingMutex, _ResourceMutex);
            if (resourceHandle->_State == kResourceStateLoading)
            {
                if (std::find(_Pending.begin(), _Pending.end(), resourceHandle) == _Pending.end())
                {
                    _Pending.push_back(resourceHandle);
                }
                resourceHandle->SetState(kResourceStateLoading);
            }
            resourceHandle->_ProcessingMutex.unlock();
            _ResourceMutex.unlock();
        }
    }
}

void ResourcePool::UnloadResource(const std::string& filename)
{
    auto resource = _Resources.find(filename);
    
    if (resource != _Resources.end())
    {
        if (auto resourceHandle = resource->second.lock())
        {
            std::lock(resourceHandle->_ProcessingMutex, _ResourceMutex);
            if (resourceHandle->_State != kResourceStateLoading)
            {
                if (std::find(_Pending.begin(), _Pending.end(), resourceHandle) == _Pending.end())
                {
                    _Pending.push_back(resourceHandle);
                }
                resourceHandle->SetState(kResourceStateLoading);
            }
            resourceHandle->_ProcessingMutex.unlock();
            _ResourceMutex.unlock();
        }
    }
}

void ResourcePool::DiscardResource(const std::string& filename)
{
    _CachedResources.erase(filename);
}

void ResourcePool::DiscardAllResources(const std::string& filename)
{
    _CachedResources.clear();
}

bool ResourcePool::HasPending()
{
    return _Pending.size();
}

std::shared_ptr<Resource> ResourcePool::GetPending(ResourceState resourceState)
{
    if (!_Pending.size())
        return 0;
    
    auto it = _Pending.begin();
    do
    {
        ResourceState state = (*it)->GetState();
        
        if (state == kResourceStateReady || state == kResourceStateError)
        {
            if (state == kResourceStateError)
            {
                std::string errorType;
                switch ((*it)->GetError())
                {
                    case kResourceErrorNone:
                        errorType = "None";
                        break;
                    case kResourceErrorNoSuchResource:
                        errorType = "No such resource";
                        break;
                    case kResourceErrorSystemError:
                        errorType = "System error";
                        break;
                    case kResourceErrorUnknown:
                        errorType = "Unknown";
                        break;
                }
                PbLogError("pb.resource", "Failed to load resource (%s), with error '%s' (%s)", (*it)->_Filename.c_str(), errorType.c_str(), (*it)->GetErrorDetails().c_str());
            }
            
            it = _Pending.erase(it);
        } else if (state == resourceState)
        {
            std::string errorDetails;
            if ((*it)->IsReadyToProcess() == kResourceReadyStateReady)
            {
                break;
            } else if ((*it)->IsReadyToProcess() == kResourceReadyStateError)
            {
                PbLogError("pb.resource", "Failed to load resource (%s), with error 'Resource not ready' (%s)", (*it)->_Filename.c_str(), (*it)->GetErrorDetails().c_str());
                it = _Pending.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    } while (it != _Pending.end());
    
    if (it != _Pending.end())
        return *it;
    
    return 0;
}

void ResourcePool::ResourceDeallocator(Resource* resource)
{
    resource->_Pool->_CachedResources.erase(resource->_Filename);
    resource->_Pool->_Resources.erase(resource->_Filename);
    ResourceManager::Instance()->AddDeletedResource(resource);
}

ResourceManager* ResourceManager::Instance()
{
    static ResourceManager* instance = new ResourceManager();
    
    return instance;
}

ResourceManager::ResourceManager()
    : _IsLoading(false)
    , _IsProcessing(false)
    , _IsPostProcessing(false)
    , _IsUnloading(false)
    , _LoadingThread(0)
    , _ProcessingThread(0)
    , _PostProcessingThread(0)
    , _UnloadingThread(0)
{

}

ResourceManager::~ResourceManager()
{
    delete _LoadingThread;
    delete _ProcessingThread;
    delete _PostProcessingThread;
    delete _UnloadingThread;
}

void ResourceManager::Update(float timeDelta)
{
    Purge();
    Notify();
    Process(kResourceStateLoading, _IsLoading, &_LoadingThread);
    Process(kResourceStateProcessing, _IsProcessing, &_ProcessingThread);
    Process(kResourceStatePostProcessing, _IsPostProcessing, &_PostProcessingThread);
    Process(kResourceStateUnloading, _IsUnloading, &_UnloadingThread);
    Notify();
}

ResourcePool* ResourceManager::GetPool(const std::string& name)
{
    auto pool = _Pools.find(name);
    
    if (pool != _Pools.end())
        return pool->second;
    
    return 0;
}

ResourcePool* ResourceManager::CreatePool(const std::string& name)
{
    if (_Pools.find(name) != _Pools.end())
    {
        PbLogWarn("pb.resource.pool", "Trying to create pool %s that already exists, returning existing pool", name.c_str());
        return _Pools.find(name)->second;
    }

    ResourcePool* pool = new ResourcePool();
    
    _Pools[name] = pool;
    
    return pool;
}

void ResourceManager::DestroyPool(const std::string& name)
{
    auto pool = _Pools.find(name);
    
    if (pool == _Pools.end())
    {
        PbLogWarn("pb.resource.pool", "Unable to destroy pool %s as it doesn't exist", name.c_str());
        return;
    }
    
    delete pool->second;
    _Pools.erase(pool);
}

void ResourceManager::RegisterResourceCreator(Uid type, ResourceCreator resourceCreator)
{
    _ResourceCreation[type] = resourceCreator;
}

void ResourceManager::Process(ResourceState state, bool& handleVariable, std::thread** thread)
{
    if (!handleVariable)
    {
        #ifndef PIXELBOOST_DISABLE_THREADING
            if (*thread && (*thread)->joinable())
            {
                (*thread)->join();
                delete *thread;
                *thread = 0;
            }
        #endif
        
        std::shared_ptr<Resource> resource = 0;
        
        for (auto it = _Pools.begin(); it != _Pools.end(); ++it)
        {
            resource = it->second->GetPending(state);
            
            if (resource)
                break;
        }
        
        #ifndef PIXELBOOST_DISABLE_THREADING
            // Note, we don't need to unlock this mutex as it's unlocked in ProcessResource once processing is complete
            if (resource && resource->_ProcessingMutex.try_lock())
            {
                handleVariable = true;
                
                if (resource->GetThread(resource->GetProcessForState(state)) == kResourceThreadMain)
                {
                    ProcessResource(resource, handleVariable);
                } else {
                    *thread = new std::thread(&ResourceManager::ProcessResource, this, resource, std::ref(handleVariable));
                }
            }
        #else
            if (resource)
            {
                ProcessResource(resource, handleVariable);
            }
        #endif
    }
}

void ResourceManager::ProcessResource(std::shared_ptr<Resource> resource, bool& handleVariable)
{
    resource->Process();
    handleVariable = false;
    resource->_ProcessingMutex.unlock();
}

void ResourceManager::AddStateChange(Resource* resource)
{
    _StateMutex.lock();
    _StateResources.insert(resource);
    _StateMutex.unlock();
}

void ResourceManager::AddDeletedResource(Resource* resource)
{
    _StateMutex.lock();
    _DeletedResources.push_back(resource);
    _StateMutex.unlock();
}

void ResourceManager::Notify()
{
    _StateMutex.lock();
    for (const auto& state : _StateResources)
    {
        state->NotifyStateChange();
    }
    _StateResources.clear();
    _StateMutex.unlock();
}

void ResourceManager::Purge()
{
    _StateMutex.lock();
    auto deleted = _DeletedResources;
    _DeletedResources.clear();
    _StateMutex.unlock();

    for (const auto& resource : deleted)
    {
        PurgeResource(resource);
    }
}

void ResourceManager::PurgeResource(Resource *resource)
{
    resource->_ProcessingMutex.lock();
    
    _StateMutex.lock();
    _StateResources.erase(resource);
    _StateMutex.unlock();
    
    // Because we're manually deleting here, we need to signal the resource unloading handlers
    resource->SignalResourceUnloading(resource);
    
    PbLogDebug("pb.resource", "Purging resource (%s)", resource->_Filename.c_str());
    
    std::string errorDetails;
    resource->ProcessResource(0, kResourceProcessUnload, "", errorDetails);
    
    resource->_ProcessingMutex.unlock();
    
    delete resource;
}

Resource* ResourceManager::CreateResourceData(int typeId)
{
    auto resourceCreator = _ResourceCreation.find(typeId);
    
    if (resourceCreator == _ResourceCreation.end())
        return 0;
    
    return resourceCreator->second();
}
