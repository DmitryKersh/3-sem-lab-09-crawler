//
// Created by dmitrykersh on 18.01.2021.
//
#include <gumbo.h>

#include <ResponseParser.hpp>

std::ostream& operator<<(std::ostream& os, ParsingResult const& result) {
    os << "Result:\n\tImages:\n";
    for (auto const& url : result.image_urls) { os << "\t\t" << url << std::endl; }

    os << "\tChild URLs:\n";
    for (auto const& url : result.child_urls) { os << "\t\t" << url << std::endl; }

    return os;
}

std::set<std::string> recursive_search(GumboNode const* const node, GumboTag const tag, char const* const name) {
    if (node->type != GUMBO_NODE_ELEMENT) return {};

    std::set<std::string> refs;
    if (node->v.element.tag == tag) {
        auto const href_tag = gumbo_get_attribute(&node->v.element.attributes, name);
        if (href_tag) { refs.emplace(href_tag->value); }
    }

    auto children = &node->v.element.children;
    auto const length = children->length;
    for (::std::size_t i = 0; i < length; ++i) {
        auto const child_refs = recursive_search(static_cast<GumboNode const*>(children->data[i]), tag, name);

        refs.insert(child_refs.begin(), child_refs.end());
    }
    return refs;
}

ParsingResult parse_response(http_response_t const& response, bool parse_children) {
    GumboOutput* output = gumbo_parse(response.body().c_str());

    return ParsingResult{
        recursive_search(output->root, GUMBO_TAG_IMG, "src"),
        parse_children ? recursive_search(output->root, GUMBO_TAG_A, "href") : ::std::set<::std::string>{}};
}
