
template<typename T>
T* ee::SBSimulation::addConstraint(T* constraint)
{
    T* ptr = new T(*constraint);
    std::unique_ptr<SBConstraint> smartPtr(ptr);
    m_constraints.push_back(std::move(smartPtr));
    return ptr;
}

template<typename T>
T* ee::SBSimulation::addLocalForceGen(T* force)
{
    T* ptr = new T(*force);
    std::unique_ptr<SBLocalForceGen> smartPtr(ptr);
    m_localForceGens.push_back(std::move(smartPtr));
    return ptr;
}