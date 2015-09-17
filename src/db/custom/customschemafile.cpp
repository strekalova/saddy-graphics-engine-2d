#include "db/custom/customschemafile.h"

#include "util/fs.h"

#include "renderer.h"

#include <fstream>
#include <cassert>

sad::db::custom::SchemaFile::SchemaFile(const sad::String& name) 
: sad::resource::PhysicalFile(name)
{
    m_factory = new sad::db::StoredPropertyFactory();
}

sad::db::custom::SchemaFile::~SchemaFile()
{
    if (m_factory)
    {
        delete m_factory;
    }
}

void sad::db::custom::SchemaFile::setFactory(sad::db::StoredPropertyFactory * factory)
{
    assert( m_factory );
    m_factory = factory->clone();
}

sad::db::StoredPropertyFactory * sad::db::custom::SchemaFile::factory() const
{
    return m_factory;
}

sad::Vector<sad::resource::Error*> sad::db::custom::SchemaFile::load(sad::resource::Folder * parent)
{
    sad::Vector<sad::resource::Error*> errors;
    sad::db::custom::SchemaFile::parse_result result;
    this->tryParsePartial(result, errors, parent);
    if (errors.size() == 0)
    {
        this->convertNonUniqueResourceNamesToErrors(result, errors);
        if (errors.size() == 0)
        {
            sad::resource::ResourceEntryList resourcelist;
            this->fillOptionsList(result, resourcelist);
            this->replaceResources(resourcelist);
            parent->addResources(resourcelist);
        }
    }
    return errors;
}

sad::Vector<sad::resource::Error*>  sad::db::custom::SchemaFile::reload()
{
    sad::Vector<sad::resource::Error*> errors;
    sad::db::custom::SchemaFile::parse_result result;
    this->tryParsePartial(result, errors, NULL);
    if (errors.size() == 0)
    {
        sad::resource::ResourceEntryList resourcelist,  oldresourcelist;
        this->fillOptionsList(result, resourcelist);
        this->createOldResourceList(oldresourcelist);
        sad::resource::ResourceEntryList tobeadded, tobereplaced, toberemoved;
        this->diffResourcesLists(oldresourcelist, resourcelist, tobeadded, tobereplaced, toberemoved);
        errors << this->tree()->duplicatesToErrors(this->tree()->root()->duplicatesBetween(tobeadded));
        convertReferencedOptionsToBeRemovedToErrors(toberemoved, errors);
        if (errors.size() == 0)
        {
            sad::resource::Folder * root = this->tree()->root();
            // Add an added resources
            root->addResources(tobeadded);
            // Replace replaced resources
            root->replaceResources(tobereplaced);
            // Remove removable resources
            root->removeResources(toberemoved, true);
            // Replace own resource list, setting correct reference to this
            this->replaceResources(resourcelist);
        }
        else
        {
            sad::resource::free(resourcelist);
        }
    }
    return errors;
}

void sad::db::custom::SchemaFile::tryParsePartial(
        sad::db::custom::SchemaFile::parse_result & result,
        sad::Vector<sad::resource::Error *> & errors,
        sad::resource::Folder * parent
    )
{
    sad::Maybe<sad::String> maybecontent = this->tryReadToString();
    if (maybecontent.exists())
    {
        if (maybecontent.value().consistsOfWhitespaceCharacters())
        {
            errors << new sad::resource::JSONParseError();
            return;
        }
        picojson::value rootvalue = picojson::parse_string(maybecontent.value());
        if (picojson::get_last_error().size() == 0)
        {
            if (rootvalue.is<picojson::array>())
            {
                picojson::array parse_item_list = rootvalue.get<picojson::array>();
                for(size_t i = 0; i < parse_item_list.size(); i++)
                {
                    sad::db::custom::SchemaFile::parse_result_entry entry;
                    if (this->tryParseEntry(entry, errors, parse_item_list[i]))
                    {
                        // Validate whether we have a resource with name
                        if (this->validateTreeReferences(entry, errors, parent))
                        {
                            result.push_back(entry);
                        }
                    }
                }
            }
            else
            {
                errors << new sad::resource::MalformedResourceEntry(rootvalue);
            }
        }
        else
        {
            errors << new sad::resource::JSONParseError();
        }
    }
    else
    {
        errors << new sad::resource::FileLoadError(m_name);
    }

    // If no errors, check schemas for unique names
    if (errors.size() == 0)
    {
        sad::Hash<sad::String, char> names;
        for(size_t i = 0; i < result.size(); i++)
        {
            if (names.contains(result[i]._1()) == false)
            {
                names.insert(result[i]._1(), 1);
            }
            else
            {
                errors << new sad::resource::ResourceAlreadyExists(result[i]._1());
            }
        }
    }
}


namespace sad
{

namespace db 
{

namespace custom
{

/*! Tests, whether properties are not inherited. Defined in db/custom/customschema.cpp
    \param[in] prop property
    \return whether property is not inherited
 */
bool is_not_inherited(const sad::String & prop);

}

}

}

bool sad::db::custom::SchemaFile::tryParseEntry(
        sad::db::custom::SchemaFile::parse_result_entry & parse_result,
        sad::Vector<sad::resource::Error *> & errors,
        const picojson::value & v
)
{
    bool result = false;
    if (v.is<picojson::object>())
    {
        sad::Maybe<sad::String> maybename = picojson::to_type<sad::String>(
                picojson::get_property(v, "name")
        );
        sad::Maybe<sad::String> mayberesourcename = picojson::to_type<sad::String>(
                picojson::get_property(v, "resource")
        );
        picojson::value const * maybeschema = picojson::get_property(v, "schema");
        if (maybeschema && maybename.exists() && mayberesourcename.exists())
        {
            if (maybeschema->is<picojson::object>())
            {
                parse_result.set1(maybename.value());
                parse_result.set2(mayberesourcename.value());
                sad::Vector< sad::Quadruplet<sad::String, sad::String, picojson::value, sad::Maybe<picojson::value> > > proplist;				
                bool loadresult = true;
                picojson::object o = maybeschema->get<picojson::object>();
                for(picojson::object::iterator it = o.begin(); it != o.end(); ++it)
                {
                    const picojson::value * maybetypeentry = picojson::get_property(it->second, "type");
                    const picojson::value * maybevalueentry = picojson::get_property(it->second, "value");
                    const picojson::value * maybedefaultentry = picojson::get_property(it->second, "default");
                    sad::Maybe<picojson::value> mbdefault;
                    if (maybetypeentry && maybevalueentry && sad::db::custom::is_not_inherited(it->first))
                    {
                        sad::Maybe<sad::String> maybeproptype = picojson::to_type<sad::String>(*maybetypeentry);
                        if (maybeproptype.exists())
                        {
                            bool propresult = m_factory->canCreate(maybeproptype.value());
                            if (propresult)
                            {
                                sad::db::Property * p = m_factory->create(maybeproptype.value());
                                sad::db::Variant v;
                                p->get(NULL, v);
                                bool canbeloadedandset = v.load(*maybevalueentry);
                                canbeloadedandset = canbeloadedandset && p->set(NULL, v);
                                if (maybedefaultentry && canbeloadedandset)
                                {
                                    sad::db::Variant testdefault;
                                    p->get(NULL, testdefault);
                                    canbeloadedandset = canbeloadedandset && testdefault.load(*maybedefaultentry);
                                    if (canbeloadedandset)
                                    {
                                        canbeloadedandset = canbeloadedandset && p->couldBeSetFrom(testdefault);
                                        if (canbeloadedandset)
                                        {
                                            mbdefault.setValue(*maybedefaultentry);	
                                        }
                                    }
                                }
                                delete p;
                                
                                if (canbeloadedandset)
                                {
                                    proplist.push_back(sad::Quadruplet<sad::String, sad::String, picojson::value, sad::Maybe<picojson::value> >(
                                        it->first, maybeproptype.value(), *maybevalueentry, mbdefault
                                    ));
                                }
                                else
                                {
                                    errors << new sad::resource::MalformedResourceEntry(it->second);
                                    loadresult = false;
                                }
                            }
                            else
                            {
                                errors << new sad::resource::MalformedResourceEntry(it->second);
                                loadresult = false;
                            }
                        }
                        else
                        {
                            errors << new sad::resource::MalformedResourceEntry(it->second);
                            loadresult = false;
                        }
                    } 
                    else
                    {
                        errors << new sad::resource::MalformedResourceEntry(it->second);
                        loadresult = false;
                    }
                }
                if (loadresult)
                {
                    result = true;
                    parse_result.set3(proplist);
                }
            }
            else
            {
                errors << new sad::resource::MalformedResourceEntry(v);
            }
        }
        else
        {
            errors << new sad::resource::MalformedResourceEntry(v);
        }
    }
    else
    {
        errors << new sad::resource::MalformedResourceEntry(v);
    }
    return result;
}

bool sad::db::custom::SchemaFile::validateTreeReferences(
        sad::db::custom::SchemaFile::parse_result_entry & parse_result,
        sad::Vector<sad::resource::Error *> & errors,
        sad::resource::Folder * parent
)
{
    bool result = true;
    sad::resource::Resource * resource = NULL;
    if (this->tree()->temporaryRoot()) 
    {
        resource = this->tree()->temporaryRoot()->resource(parse_result._2());
    } 
    else
    {
        resource = this->tree()->root()->resource(parse_result._2());
    }
    if (resource)
    {
        if (resource->metaData()->name() == "sad::Sprite2D::Options" || resource->metaData()->canBeCastedTo("sad::Font"))
        {
            result = true;
        }
        else
        {
            errors << new sad::resource::MissingResource(parse_result._2());
        }
    }
    else
    {
        errors << new sad::resource::MissingResource(parse_result._2());
    }
    return result;
}


void sad::db::custom::SchemaFile::convertNonUniqueResourceNamesToErrors(
        sad::db::custom::SchemaFile::parse_result & parse_result,
        sad::Vector<sad::resource::Error *> & errors
)
{
    sad::resource::Folder * r = this->tree()->root();
    if (this->tree()->temporaryRoot() != NULL)
    {
        r = this->tree()->temporaryRoot();
    }
    for(size_t i = 0; i < parse_result.size(); i++)
    {
        sad::resource::Resource* res = r->resource(parse_result[i]._1());
        if (res != NULL)
        {
            errors << new sad::resource::ResourceAlreadyExists(parse_result[i]._1());
        }
    }
}


void sad::db::custom::SchemaFile::fillOptionsList(
        sad::db::custom::SchemaFile::parse_result & parsed,
        sad::resource::ResourceEntryList & resources
)
{
    for(size_t i = 0; i < parsed.size(); i++)
    {
        sad::db::custom::Schema* schema = new sad::db::custom::Schema();
        schema->setFactory(m_factory);
        schema->setTreeItemName(parsed[i]._2());
        schema->setPhysicalFile(this);
        const sad::Vector<
            sad::Quadruplet<
                sad::String, 
                sad::String, 
                picojson::value,
                sad::Maybe<picojson::value>
            > 
        > & props = parsed[i]._3();
        for(size_t j = 0; j < props.size(); j++)
        {
            sad::db::Property * p = m_factory->create(props[j].p2());
            sad::db::Variant v;
            p->get(NULL, v);
            v.load(props[j].p3());
            p->set(NULL, v);
            const sad::Maybe<picojson::value>& mbdefault = props[j].p4();
            if (mbdefault.exists())
            {
                sad::db::Variant* defaultvalue = new sad::db::Variant();
                p->get(NULL, *defaultvalue);
                defaultvalue->load(mbdefault.value());
                assert( p->makeNonRequiredWithDefaultValue(defaultvalue) );
                assert( p->hasDefaultValue() );
            }
            schema->add(props[j].p1(), p);
        }
        if (this->tree()->shouldStoreLinks())
        {
            schema->enableStoringLinks();
        }
        resources.push_back(sad::resource::ResourceEntry(parsed[i]._1(), schema));
    }
}
