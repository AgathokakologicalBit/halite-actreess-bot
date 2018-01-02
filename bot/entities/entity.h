#pragma once


class Entity
{
public:
    int id;
    hlt::Entity obj;

    unsigned int life_state;
    TagList tags;

public:
    explicit Entity (hlt::Location location, double radius)
            : id(0)
              , obj(location, radius)
              , life_state(0)
              , tags()
    { }

    explicit Entity (hlt::Entity const & entity_)
            : id(entity_.entity_id)
              , obj(entity_)
              , life_state(2)
              , tags()
    { }

    void update (hlt::Entity const & entity)
    {
        this->life_state = 1;
        this->obj = entity;
    }
};
