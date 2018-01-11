#pragma once

#include <set>
#include <string>

/**
 * Class for managing tags
 */
struct TagList
{
private:
    std::set<std::string> tags;

public:
    /**
     * Creates empty tags list ready to be filled with values
     */
    TagList ()
            : tags()
    { }

    /**
     * Adds new tag to the list
     * @param tag
     * @return is element was added
     */
    bool add (std::string & tag)
    {
        if (tags.count(tag)) return true;
        return tags.insert(tag), false;
    }

    /**
     * Removes tag from the list
     * @param tag
     * @return is element was removed
     */
    bool remove (std::string & tag)
    {
        return tags.erase(tag) > 0;
    }

    /**
     * Toggles tag's state
     * @param tag
     * @return new state of the tag
     */
    bool toggle (std::string & tag)
    {
        if (tags.count(tag))
            return tags.erase(tag), false;
        return tags.insert(tag), true;
    }

    /**
     * Checks whether the tag is in the list or not
     * @param tag
     * @return current tag state
     */
    bool get (std::string & tag)
    {
        return tags.count(tag) > 0;
    }
};
