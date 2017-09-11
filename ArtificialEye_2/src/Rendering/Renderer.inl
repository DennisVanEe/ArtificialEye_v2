template<typename T>
T* ee::Renderer::addTexturePack(std::string name, T* pack)
{
    if (checkTextPackMap(name))
    {
        T* ptr = new T(*pack);
        insertTextPackIntoMap(name, ptr);
        return ptr;
    }

    return nullptr;
}
